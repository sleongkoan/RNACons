#include "../include/RNA.h"
#include "../include/RNAConsensus.h"
#include "../include/SolverGA.h"
#include "../include/Solution.h"


using std::vector;
using std::string;


double calculate_insertion_cost(std::vector<int> selected_indices,
                                int elem_index,
                                std::vector< std::vector<double> > distance_matrix)
{
    // calculate the cost of adding an element into the solution
    double cost = 0.;
    int index;
    for (size_t i = 0; i < selected_indices.size(); ++i)
    {
        index = selected_indices[i];
        cost += distance_matrix[index][elem_index];
    }
    return (2. * cost);  // symmetric
}


struct InsertionPoints {std::vector<int> best_insertions, std::vector<double> best_costs};


InsertionPoints find_best_insertion(std::vector<int> selected_indices,
                                    std::vector<Range> remaining_ranges,
                                    std::vector< std::vector<double> > distance_matrix)
{
    // will find, in the remaining ranges, the element with least cost insertion
    std::vector<int> best_insertions = std::vector<int>();
    std::vector<double> best_costs = std::vector<double>();

    int low, high, best_insertion;
    double cost, best_cost;
    for (size_t i = 0; i != remaining_ranges.size(); ++i)
    {
        low = remaining_ranges[i].low;
        high = remaining_ranges[i].high;
        best_insertion = -1;
        best_cost = std::numeric_limits<double>::infinity();

        for (int index = low, index != high; ++index)
        {
            cost = insertion_cost(selected_indices, index, distance_matrix);
            if (cost < best_cost) {
                best_cost = cost;
                best_insertion = index;
            }
        }
        best_insertions.push_back(best_insertion);
        best_costs.push_back(best_cost);
    }
    InsertionPoints result = InsertionPoints({best_insertions, best_costs});
    return result;
}


std::vector<Solution> generate_solutions(Solution solution,
                                    std::vector<Range> remaining_ranges,
                                    std::vector< std::vector<double> >distance_matrix,
                                    double lower_bound):
    std::vector<Solution> new_solutions = std::vector<Solution>();
    size_t num_fusions_left = remaining_ranges.size() -1;
    // create all possible derivatives of the solution
    genes = list(copy.copy(solution.genes))
    for index in range(remaining_ranges[0][0], remaining_ranges[0][1]):
        new_genes = copy.copy(genes)
        new_genes.append(index)
        new_solution = Solution(new_genes)
        assign_pairwise_distance_score(new_solution, distance_matrix)

        // prune if score cannot be better than the lower bound (lower)
        best_insertions, best_costs = find_best_insertion(new_genes, remaining_ranges[1:], distance_matrix)
        
        potential_score = sum(best_costs) + new_solution.score
        if len(remaining_ranges) == 1:  #at the end
            potential_score = new_solution.score
        if potential_score <= lower_bound:
            new_solutions.append(new_solution)
    return new_solutions
}

void solveGA(string path, size_t popsize, size_t num_generations)
{
    // ouptut model parameters (some of them at least)
    std::cerr << "Calculating RNA secondary consensus" << std::endl;

    // get the data
    std::vector<std::vector<string> > prob1_data = read_data(path);
    std::vector<std::vector<Tree> > trees = get_tree_lists(prob1_data);
    unsigned long seeds[6] = {42, 42, 42, 42, 42, 42};


    // create the tree problem instance and solve it
    ConsensusProblem<Tree>* tree_problem = new ConsensusProblem<Tree>(trees, *unit_distance);
    std::vector<Solution> tree_consensus = solve(tree_problem->get_distance_matrix(),
                                            tree_problem->get_ranges(),
                                            popsize, num_generations, seeds);

    // filter the dot brackets by the tree consensus
    std::vector<std::vector< std::vector<string> > > prob2_data = std::vector< std::vector< std::vector<string> > >();

    // get the brackets
    for(std::vector<Solution>::iterator it = tree_consensus.begin(); it != tree_consensus.end(); ++it) {
        std::vector<string> brackets = std::vector<string>();
        for(size_t j = 0; j != it->get_genes().size(); ++j) {
            int gene = it->get_genes()[j];
            brackets.push_back(tree_problem->get_objects()[gene].get_brackets());
        }
        // filter and add to the problem 2 data
        std::vector< std::vector<string> > filtered_brackets = filter_dot_brackets(prob1_data, brackets);
        prob2_data.push_back(filtered_brackets);
    }
    std::vector< ConsensusProblem<string>* > dot_bracket_problems = std::vector< ConsensusProblem<string>* >();
    // instantiate the problems
    for (size_t i = 0; i != prob2_data.size(); ++i) {
        dot_bracket_problems.push_back(new ConsensusProblem<string>(prob2_data[i], *levenshtein));
    }



    std::vector< std::vector<Solution> > dot_bracket_consensus = std::vector< std::vector<Solution> >();
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
