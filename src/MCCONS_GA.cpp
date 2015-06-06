#include "../include/RNA.h"
#include "../include/RNAConsensus.h"
#include "../include/SolverGA.h"
#include "../include/OptionParser.h"


using std::vector;
using std::string;
using std::cout;
using std::endl;
using optparse::OptionParser;


void MCCONS_GA(string path,
               size_t popsize,
               size_t num_generations,
               bool silent)
{

    // GLOBAL SETTINGS (MODIFY AT YOUR OWN RISKS (WHICH ARE MINIMAL))
    int IMPROVEMENT_DEPTH= 2;
    int ELITE_SIZE = 30;
    double CROSSOVER_PROBABILITY = 0.5;
    double MUTATION_PROBABILITY = 0.05;
    double IMPROVEMENT_PROBABILITY = 0.05;
    //string PROBLEM_NAME = "";
    unsigned long SEEDS[6] = {42, 42, 42, 42, 42, 42};

    // TREE CONSENSUS
    // fetch the data and instantiate the tree consensus problem
    vector<vector<string> > dot_brackets = read_data(path);
    vector<vector<Tree> > trees = get_tree_lists(dot_brackets);
    ConsensusProblem<Tree>* tree_problem = new ConsensusProblem<Tree>(trees, *unit_distance);

    // instantiate the solver
    SolverGA solver1 = SolverGA(tree_problem->get_distance_matrix(),
                                tree_problem->get_ranges(),

                                popsize,
                                num_generations,
                                IMPROVEMENT_DEPTH,
                                ELITE_SIZE,

                                CROSSOVER_PROBABILITY,
                                MUTATION_PROBABILITY,
                                IMPROVEMENT_PROBABILITY,

                                "Tree Indel Distance Consensus",
                                silent);
    vector<Solution> tree_consensus = solver1.solve(SEEDS);


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
        vector< vector<string> > filtered_brackets = filter_dot_brackets(dot_brackets, brackets);
        prob2_data.push_back(filtered_brackets);
    }
    vector< ConsensusProblem<string>* > dot_bracket_problems = vector< ConsensusProblem<string>* >();
    // instantiate the problems
    for (size_t i = 0; i != prob2_data.size(); ++i) {
        dot_bracket_problems.push_back(new ConsensusProblem<string>(prob2_data[i], *levenshtein));
    }


    // TREE-STRING CONSENSUS
    vector< vector<Solution> > dot_bracket_consensus = vector< vector<Solution> >();
    for (size_t i = 0; i != prob2_data.size(); ++i) {
        SolverGA solver2 = SolverGA(dot_bracket_problems[i]->get_distance_matrix(),
                                    dot_bracket_problems[i]->get_ranges(),

                                    popsize,
                                    num_generations,
                                    IMPROVEMENT_DEPTH,
                                    ELITE_SIZE,

                                    CROSSOVER_PROBABILITY,
                                    MUTATION_PROBABILITY,
                                    IMPROVEMENT_PROBABILITY,

                                    "String Edit Distance Consensus",
                                    silent);
        dot_bracket_consensus.push_back(solver2.solve(SEEDS));
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

    // create the command line parser
    OptionParser parser = OptionParser().description("MC-Cons Consensus Optimizer Using a Genetic Algorithm");
    parser.add_option("-f", "--data").dest("dataFile").help("path to MARNA-like input file");
    parser.add_option("-p", "--popsize").dest("popSize").help("genetic algorithm population size").type("size_t");
    parser.add_option("-n", "--numgen").dest("numGenerations").type("size_t").help("genetic algorithm number of generations").;
    parser.add_option("-s", "--silent").action("store_true").dest("silent").help("don't display status to stderr");
    optparse::Values options = parser.parse_args(argc, argv);
    vector<string> args = parser.args();

    // extract parameters and lauch the genetic algorithm (either silent or not)
    if (options.is_set("dataFile") && options.is_set("popSize") && options.is_set("numGenerations")) {
      string path = options["dataFile"];
      size_t popsize = atoi(options["popSize"].c_str());
      size_t num_generations = atoi(options["numGenerations"].c_str());

      if (options["silent"] == "1") {
          // silent
          MCCONS_GA(path, popsize, num_generations, true);
      } else {
          // verbose
          MCCONS_GA(path, popsize, num_generations, false);
      }

    } else {

      std::cerr << parser.format_help() << std::endl;
      std::exit(0);
    }
}
