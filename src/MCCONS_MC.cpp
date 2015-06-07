#include "../include/ConsensusProblem.h"
#include "../include/RNA.h"
#include "../include/RNAConsensus.h"
#include "../include/SolverMC.h"
#include "../include/OptionParser.h"

using optparse::OptionParser;


void MCCONS_MC(std::string path,
               size_t num_trials,
               bool silent)
{
    // GLOBAL SETTINGS (MODIFY AT YOUR OWN RISKS (WHICH ARE MINIMAL))
    unsigned long SEEDS[6] = {42, 42, 42, 42, 42, 42};

    // TREE CONSENSUS
    std::vector<std::vector<std::string> > dot_brackets = read_data(path);
    std::vector<std::vector<Tree> > trees = get_tree_lists(dot_brackets);
    ConsensusProblem<Tree> tree_problem = ConsensusProblem<Tree>(trees, *unit_distance);
    SolverMC solver1 = SolverMC(tree_problem.get_distance_matrix(),
                                tree_problem.get_ranges(),
                                num_trials,
                                "Tree Indel Distance Consensus",
                                silent);
    std::vector<Solution> tree_consensus = solver1.solve(SEEDS);
    double tree_score = tree_consensus[0].get_score();


    // filter the dot brackets by the tree consensus
    std::vector<std::vector< std::vector<std::string> > > prob2_data = std::vector< std::vector< std::vector<std::string> > >();

    // get the brackets
    for(std::vector<Solution>::iterator it = tree_consensus.begin(); it != tree_consensus.end(); ++it) {
        std::vector<std::string> brackets = std::vector<std::string>();
        for(size_t j = 0; j != it->get_genes().size(); ++j) {
            int gene = it->get_genes()[j];
            brackets.push_back(tree_problem.get_objects()[gene].get_brackets());
        }
        // filter and add to the problem 2 data
        std::vector< std::vector<std::string> > filtered_brackets = filter_dot_brackets(dot_brackets, brackets);
        prob2_data.push_back(filtered_brackets);
    }
    std::vector< ConsensusProblem<std::string> > dot_bracket_problems = std::vector< ConsensusProblem<std::string> >();
    // instantiate the problems
    for (size_t i = 0; i != prob2_data.size(); ++i) {
        dot_bracket_problems.push_back(ConsensusProblem<std::string>(prob2_data[i], *levenshtein));
    }



    std::vector< std::vector<Solution> > dot_bracket_consensus = std::vector< std::vector<Solution> >();
    for (size_t i = 0; i != prob2_data.size(); ++i) {
        SolverMC solver2 = SolverMC(dot_bracket_problems[i].get_distance_matrix(),
                                    dot_bracket_problems[i].get_ranges(),
                                    num_trials,
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
                std::cout << "> " << sol_index << " " << tree_score << " " << best_score << std::endl;
                sol_index += 1;
                for (size_t gene_index = 0; gene_index != dot_bracket_consensus[i][j].get_genes().size(); ++gene_index) {
                    gene = dot_bracket_consensus[i][j].get_genes()[gene_index];
                    std::cout << dot_bracket_problems[i].get_objects()[gene] << std::endl;
                }
                std::cout << std::endl;
            }
        }
    }

    return;
}


int main(int argc, char *argv[])
{
    OptionParser parser = OptionParser().description("MC-Cons Consensus Optimizer Using a Monte Carlo and Steepest Descent");
    parser.add_option("-f", "--data").dest("data_file").help("path to MARNA-like input file");
    parser.add_option("-n", "--popsize").dest("num_trials").help("number of trials").type("size_t");
    parser.add_option("-s", "--silent").action("store_true").dest("silent").help("don't display status to stderr");
    optparse::Values options = parser.parse_args(argc, argv);
    std::vector<std::string> args = parser.args();

    // extract parameters and lauch the genetic algorithm (either silent or not)
    if (options.is_set("data_file") && options.is_set("num_trials"))
    {
      std::string path = options["data_file"];
      size_t num_trials = atoi(options["num_trials"].c_str());

      if (options["silent"] == "1")
      {
          // silent
          MCCONS_MC(path, num_trials, true);
      } else
      {
          // verbose
          MCCONS_MC(path, num_trials, false);
      }

    } else
    {

      std::cerr << parser.format_help() << std::endl;
      std::exit(0);
    }
}