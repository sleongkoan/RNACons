
#include "../include/MCCONS.h"


void MCCONS(std::string path,
            Solver* solver,
            unsigned long seeds[6])
{

    // PHASE 1: TREE CONSENSUS

    // data acquisition
    std::vector<std::vector<std::string> > dot_brackets = read_data(path);
    std::vector<std::vector<Tree> > trees = get_tree_lists(dot_brackets);
    ConsensusProblem<Tree> tree_problem = ConsensusProblem<Tree>(trees, *unit_distance);

    // PHASE 1 SOLVING
    if (! solver->is_silent())
    {
        std::cerr << "Phase 1: Base Pair Tree Consensus (1)" << std::endl;
    }

    std::vector<Solution> tree_consensus = solver->solve(tree_problem.get_distance_matrix(),
                                                         tree_problem.get_ranges(),
                                                         seeds);

    // PHASE 1.5: FILTERING
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
    for (size_t i = 0; i != prob2_data.size(); ++i)
    {
        dot_bracket_problems.push_back(ConsensusProblem<std::string>(prob2_data[i], *levenshtein));
    }


    // PHASE 2: STRING CONSENSUS
    if (! solver->is_silent())
    {
        // update on the number of optimal trees found
        std::cerr << std::endl << "Phase 2: String Edit Distance on Vienna Dot Bracket (" << dot_bracket_problems.size() << ")" << std::endl;
    }
    std::vector< std::vector<Solution> > dot_bracket_consensus = std::vector< std::vector<Solution> >();
    for (size_t i = 0; i != prob2_data.size(); ++i)
    {
        dot_bracket_consensus.push_back(solver->solve(dot_bracket_problems[i].get_distance_matrix(),
                                                      dot_bracket_problems[i].get_ranges(),
                                                      seeds));
    }


    // filter out suboptimal solutions
    double best_dot_bracket_score = std::numeric_limits<double>::infinity();
    for(size_t i = 0; i != dot_bracket_consensus.size(); ++i) {
        for(size_t j = 0; j != dot_bracket_consensus[i].size(); ++j) {
            if (dot_bracket_consensus[i][j].get_score() < best_dot_bracket_score) {
                best_dot_bracket_score = dot_bracket_consensus[i][j].get_score();
            }
        }
    }
    if (! solver->is_silent())
    {
        std::cerr << std::endl;
    }

    // printing the results to stdout
    int gene;
    int num_relations = dot_brackets.size() * (dot_brackets.size() - 1);  // n (n -1)
    double tree_score = tree_consensus[0].get_score() / num_relations;
    double dot_bracket_score = best_dot_bracket_score / num_relations;

    int sol_index = 0;
    for(size_t i = 0; i != dot_bracket_consensus.size(); ++i) {
        for(size_t j = 0; j != dot_bracket_consensus[i].size(); ++j) {
            if (dot_bracket_consensus[i][j].get_score() == best_dot_bracket_score) {
                // output the consensus
                std::cout << "> " << sol_index << " " << tree_score << " " << dot_bracket_score << std::endl;
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