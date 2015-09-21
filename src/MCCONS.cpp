
#include "../include/MCCONS.h"


struct pairs_sort_predicate
{  // sorting predicate for pairs of size_t, double by double
    bool operator()(const std::pair<int,double> &left,
                    const std::pair<int,double> &right) {
        return left.second < right.second;
    }
};


std::vector< std::vector<RNATree> > strings_to_trees(std::vector< std::vector<std::string> > dot_brackets_vectors)
{   // get the list of trees from the dot bracket strings

    std::vector< std::vector<RNATree> > trees = std::vector< std::vector<RNATree> >();

    for (size_t i = 0; i < dot_brackets_vectors.size(); ++i)
    {
        // initialize a vector of trees to add later
        std::vector<std::string> current_unique_brackets = std::vector<std::string>();
        std::vector<RNATree> current_unique_trees = std::vector<RNATree>();

        for(size_t j = 0; j < dot_brackets_vectors[i].size(); ++j)
        {
            // calculate the bracket
            std::string bracket = only_paired(dot_brackets_vectors[i][j]);

            // check if it has already been processed
            if (std::find(current_unique_brackets.begin(), current_unique_brackets.end(), bracket) == current_unique_brackets.end())
            {
                current_unique_brackets.push_back(bracket);
                current_unique_trees.push_back(RNATree(bracket, '(', ')', '.'));
            }
        }
        // add to the trees
        trees.push_back(current_unique_trees);
    }
    return trees;
}


std::vector< std::vector<std::string> > filter_dot_brackets(std::vector< std::vector<std::string> > dot_brackets_vectors,
                                                            std::vector<std::string> brackets)
{  // remove dot_brackets with the wrong base tree topology
    std::vector< std::vector<std::string> > result = std::vector< std::vector<std::string> >();
    for(unsigned int i = 0; i < dot_brackets_vectors.size(); ++i)
    {
        std::string bracket = brackets[i];
        std::vector<std::string> compliant_dot_brackets = std::vector<std::string>();

        for(unsigned int j = 0; j < dot_brackets_vectors[i].size(); ++j)
        {
            if(only_paired(dot_brackets_vectors[i][j]) == bracket)
            {
                compliant_dot_brackets.push_back(dot_brackets_vectors[i][j]);
            }
        }
        result.push_back(compliant_dot_brackets);
    }
    return result;
}





void MCCONS(std::string path,
            Solver* tree_solver,
            Solver* dot_bracket_solver)
{   // given a solver and a problem instance, apply the MC-Cons algorithm

    // PHASE 1: TREE CONSENSUS

    // data acquisition
    std::vector<std::vector<std::string> > dot_brackets = read_marna_file(path);
    std::vector<std::vector<RNATree> > trees = strings_to_trees(dot_brackets);
    ConsensusProblem<RNATree> tree_problem = ConsensusProblem<RNATree>(trees, *unit_tree_indel_distance_trees);
    int num_comparisons = dot_brackets.size() * (dot_brackets.size() - 1);  // n (n -1)

    // PHASE 1 SOLVING
    bool silent_output = tree_solver->is_silent();
    if (! silent_output)
    {
        // print out the size of the search space
        // long int 
        std::cerr << "Phase 1: Base Pair Tree Consensus (1)" << std::endl;
    }

    std::vector<Solution> tree_consensus = tree_solver->solve(tree_problem.get_distance_matrix(),
                                                              tree_problem.get_ranges());

    // sort the tree consensus by score (lower is better)
    std::vector< std::pair<int, double> > sorted_order = std::vector<std::pair<int, double> >();
    for (size_t i = 0; i != tree_consensus.size(); ++i)
    {
      sorted_order.push_back(std::pair<size_t, double>(i, tree_consensus[i].get_score()));
    }
    std::sort(sorted_order.begin(), sorted_order.end(), pairs_sort_predicate());
    std::vector<Solution> tree_consensus_tmp = std::vector<Solution>();
    for (size_t i = 0; i != sorted_order.size(); ++i)
    {
        tree_consensus_tmp.push_back(tree_consensus[sorted_order[i].first]);
    }
    tree_consensus = tree_consensus_tmp;


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
        dot_bracket_problems.push_back(ConsensusProblem<std::string>(prob2_data[i], *string_edit_distance));
    }


    // PHASE 2: STRING CONSENSUS
    if (! silent_output)
    {
        // update on the number of optimal trees found
        std::cerr << std::endl << "Phase 2: String Edit Distance on Vienna Dot Bracket (" << dot_bracket_problems.size() << ")" << std::endl;
    }
    // if too many output, make the solver silent and add a progress bar over all tree consensus instead of individually
    bool too_much_progress_bar = (tree_consensus.size() > 10) && (! silent_output);
    if (too_much_progress_bar)
    {
        dot_bracket_solver->flip_silent();
    }
    std::vector< std::vector<Solution> > dot_bracket_consensus = std::vector< std::vector<Solution> >();
    ProgressBar progress = ProgressBar();
    for (size_t i = 0; i != prob2_data.size(); ++i)
    {
        dot_bracket_consensus.push_back(dot_bracket_solver->solve(dot_bracket_problems[i].get_distance_matrix(),
                                                                 dot_bracket_problems[i].get_ranges()));
        progress.update((float) i / tree_consensus.size());
    }
    progress.clean();

    if (! silent_output)
    {
        std::cerr << std::endl;
    }

    // printing the results to stdout
    // print the best dotbracket consensus for each tree consensus
    int gene;
    double tree_score, best_dot_bracket_score;

    int sol_index = 0;
    for(size_t i = 0; i != dot_bracket_consensus.size(); ++i)
    {
        // figure out what the best scores are within each tree consensus
        tree_score = tree_consensus[i].get_score() / num_comparisons;
        best_dot_bracket_score = std::numeric_limits<double>::infinity();
        for(size_t j = 0; j != dot_bracket_consensus[i].size(); ++j)
        {
            if (dot_bracket_consensus[i][j].get_score() < best_dot_bracket_score)
            {
                best_dot_bracket_score = dot_bracket_consensus[i][j].get_score();
            }
        }
        for(size_t j = 0; j != dot_bracket_consensus[i].size(); ++j)
        {
            if (dot_bracket_consensus[i][j].get_score() == best_dot_bracket_score) {
                // output the consensus
                std::cout << "> " << sol_index << " " << tree_score << " " << best_dot_bracket_score / num_comparisons << std::endl;
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

