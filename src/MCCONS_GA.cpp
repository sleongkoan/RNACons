#include "../include/RNA.h"
#include "../include/RNAConsensus.h"
#include "../include/SolverGA.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;


void solveGA(string path, size_t popsize, size_t num_generations)
{
    // ouptut model parameters (some of them at least)
    std::cerr << "Calculating RNA secondary consensus" << std::endl;

    // get the data
    vector<vector<string> > prob1_data = read_data(path);
    vector<vector<Tree> > trees = get_tree_lists(prob1_data);
    unsigned long seeds[6] = {42, 42, 42, 42, 42, 42};


    // create the tree problem instance and solve it
    ConsensusProblem<Tree>* tree_problem = new ConsensusProblem<Tree>(trees, *unit_distance);
    vector<Solution> tree_consensus = solve(tree_problem->get_distance_matrix(),
                                            tree_problem->get_ranges(),
                                            popsize, num_generations, seeds);

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
        dot_bracket_consensus.push_back(solve(dot_bracket_problems[i]->get_distance_matrix(),
                                              dot_bracket_problems[i]->get_ranges(),
                                              popsize, num_generations, seeds));
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
    if (argc == 4) {
        string path = argv[1];
        size_t popsize = atoi(argv[2]);
        size_t num_generations = atoi(argv[3]);
        solveGA(path, popsize, num_generations);
    } else {

        std::cerr << "Wrong Command Line Arguments" << std::endl;
        exit (EXIT_FAILURE);
    }

}
