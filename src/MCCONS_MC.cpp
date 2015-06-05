#include "../include/RNA.h"
#include "../include/RNAConsensus.h"
#include "../include/SolverGA.h"


using std::vector;
using std::string;


vector<Solution> MCConsensus(vector< vector<double> > distance_matrix,
                             vector<Range> ranges,
                             size_t num_trials,
                             unsigned long seeds[6], // takes 6 seeds, unsigned long
                             bool silent=false,
                             size_t improvement_depth=pow(10, 5))
{
    // solve the cvrp problem using a simple genetic algorithm
    assert(num_trials > 0);

    // seed the pseudorandom generator (MRG32k3a from L'Ecuyer)
    RngStream * prng = new RngStream();
    prng->SetSeed(seeds);

    // create solutions and score them first
    // O(n) memory, not clever I know...
    vector<Solution> solutions = initialize_population(ranges, num_trials, prng);
    for (size_t index = 0; index != solutions.size(); ++index)
    {
        assign_pairwise_distance_score(solutions[index], distance_matrix);
    }

    // start the progress meter
    ProgressBar bar = ProgressBar();
    double best_score = std::numeric_limits<double>::infinity();


    // main loop
    for (size_t index = 0; index != num_trials; ++index)
    {
        // display progress
        if (!silent) {
            bar.update((float) index / num_trials);
        }

        // apply optimization
        steepest_improvement(solutions[index], distance_matrix, ranges, improvement_depth);
        assign_pairwise_distance_score(solutions[index], distance_matrix);
        if (solutions[index].get_score() < best_score) {
            best_score = solutions[index].get_score();
        }
    }

    vector<Solution> best_solutions = vector<Solution>();
    for (size_t index = 0; index != num_trials; ++index) {
        //
        if (solutions[index].get_score() == best_score) {
            // check if it already in the best solutions
            bool found = false;
            for (size_t i = 0; i < best_solutions.size(); ++i) {
                if (best_solutions[i] == solutions[index]) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                // add to the best solutions
                best_solutions.push_back(solutions[index]);
            }
        }
    }
    if (!silent) {
        bar.clean();
    }

    return best_solutions;
}

void solveMC(string path, size_t num_trials)
{
    // ouptut model parameters (some of them at least)
    std::cerr << "Calculating RNA secondary consensus" << std::endl;

    // get the data
    vector<vector<string> > prob1_data = read_data(path);
    vector<vector<Tree> > trees = get_tree_lists(prob1_data);
    unsigned long seeds[6] = {42, 42, 42, 42, 42, 42};


    // create the tree problem instance and solve it
    ConsensusProblem<Tree>* tree_problem = new ConsensusProblem<Tree>(trees, *unit_distance);
    vector<Solution> tree_consensus = MCConsensus(tree_problem->get_distance_matrix(),
                                                  tree_problem->get_ranges(),
                                                  num_trials, seeds);

    // filter the dot brackets by the tree consensus
    vector<vector< vector<string> > > prob2_data = vector< vector< vector<string> > >();

    // get the brackets
    for(vector<Solution>::iterator it = tree_consensus.begin(); it != tree_consensus.end(); ++it) {
        vector<string> brackets = vector<string>();
        for(size_t j = 0; j != it->get_genes().size(); ++j) {
            int gene = it->get_genes()[j];
            brackets.push_back(tree_problem->get_objects()[gene].get_brackets());
        }
        // filter and add to the problem 2 data
        vector< vector<string> > filtered_brackets = filter_dot_brackets(prob1_data, brackets);
        prob2_data.push_back(filtered_brackets);
    }
    vector< ConsensusProblem<string>* > dot_bracket_problems = vector< ConsensusProblem<string>* >();
    // instantiate the problems
    for (size_t i = 0; i != prob2_data.size(); ++i) {
        dot_bracket_problems.push_back(new ConsensusProblem<string>(prob2_data[i], *levenshtein));
    }



    vector< vector<Solution> > dot_bracket_consensus = vector< vector<Solution> >();
    for (size_t i = 0; i != prob2_data.size(); ++i) {
        dot_bracket_consensus.push_back(MCConsensus(dot_bracket_problems[i]->get_distance_matrix(),
                                              dot_bracket_problems[i]->get_ranges(),
                                              num_trials, seeds));
    }


    // filter out suboptimal solutions
    double best_score = std::numeric_limits<double>::infinity();
    for(size_t i = 0; i != dot_bracket_consensus.size(); ++i) {
        for(size_t j = 0; j != dot_bracket_consensus[i].size(); ++j) {
            if (dot_bracket_consensus[i][j].get_score() < best_score) {
                best_score = dot_bracket_consensus[i][j].get_score();
            }
        }
    }

    int sol_index = 0;
    int gene;
    for(size_t i = 0; i != dot_bracket_consensus.size(); ++i) {
        for(size_t j = 0; j != dot_bracket_consensus[i].size(); ++j) {
            if (dot_bracket_consensus[i][j].get_score() == best_score) {
                // output the consensus
                std::cout << "> " << sol_index << " " << best_score << std::endl;
                sol_index += 1;
                for (size_t gene_index = 0; gene_index != dot_bracket_consensus[i][j].get_genes().size(); ++gene_index) {
                    gene = dot_bracket_consensus[i][j].get_genes()[gene_index];
                    std::cout << dot_bracket_problems[i]->get_objects()[gene] << std::endl;
                }
                std::cout << std::endl;
            }
        }
    }

    // cleanup
    trees.clear();
    delete tree_problem;
    for(size_t i = 0; i < dot_bracket_problems.size(); ++i) {
        delete dot_bracket_problems[i];
    }
    dot_bracket_problems.clear();

    return;
}



int main(int argc, char *argv[]) {
   // fetch CLI arguments
   if (argc == 3) {
       string path = argv[1];
       size_t num_trials = atoi(argv[2]);
       solveMC(path, num_trials);
   } else {

       std::cerr << "Wrong Command Line Arguments" << std::endl;
       exit (EXIT_FAILURE);
   }

}

