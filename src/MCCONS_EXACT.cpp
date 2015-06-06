// #include "../include/RNA.h"
// #include "../include/RNAConsensus.h"
// #include "../include/SolverGA.h"
// #include "../include/Solution.h"
// 
// 
// using std::vector;
// using std::string;
// using std::cout;
// using std::endl;
// 
// 
// void assign_pairwise_distance_score(Solution solution,
//                                     vector<vector<double> > distance_matrix){
//     double score = 0.;
//     size_t size = solution.get_genes().size();
//     for (size_t i = 0; i < size; ++i)
//     {
//       for(size_t j = 0; j < size; ++j)
//       {
//         score += distance_matrix[i][j];
//       }
//     }
// 
//     solution.set_score(score);
//     return;
// }
// 
// 
// double calculate_insertion_cost(vector<int> selected_indices,
//                                 int elem_index,
//                                 vector< vector<double> > distance_matrix)
// {
//     // calculate the cost of adding an element into the solution
//     double cost = 0.;
//     int index;
//     for (size_t i = 0; i < selected_indices.size(); ++i)
//     {
//         index = selected_indices[i];
//         cost += distance_matrix[index][elem_index];
//     }
//     return (2. * cost);  // symmetric
// }
// 
// 
// struct insertionPoints {vector<int> best_insertions, vector<double> best_costs};
// 
// 
// insertionPoints find_best_insertion(vector<int> selected_indices,
//                                     vector<Range> remaining_ranges,
//                                     vector< vector<double> > distance_matrix)
// {
//     // will find, in the remaining ranges, the element with least cost insertion
//     vector<int> best_insertions = vector<int>();
//     vector<double> best_costs = vector<double>();
// 
//     int low, high, best_insertion;
//     double cost, best_cost;
//     for (size_t i = 0; i != remaining_ranges.size(); ++i)
//     {
//         low = remaining_ranges[i].low;
//         high = remaining_ranges[i].high;
//         best_insertion = -1;
//         best_cost = std::numeric_limits<double>::infinity();
// 
//         for (int index = low, index != high; ++index)
//         {
//             cost = insertion_cost(selected_indices, index, distance_matrix);
//             if (cost < best_cost) {
//                 best_cost = cost;
//                 best_insertion = index;
//             }
//         }
//         best_insertions.push_back(best_insertion);
//         best_costs.push_back(best_cost);
//     }
//     insertionPoints result = insertionPoints({best_insertions, best_costs});
//     return result;
// }
// 
// 
// vector<Solution> generate_solutions(Solution solution,
//                                     vector<Range> remaining_ranges,
//                                     vector< vector<double> >distance_matrix,
//                                     double lower_bound):
//     vector<Solution> new_solutions = vector<Solution>();
//     size_t num_fusions_left = remaining_ranges.size() -1;
//     // create all possible derivatives of the solution
//     genes = list(copy.copy(solution.genes))
//     for index in range(remaining_ranges[0][0], remaining_ranges[0][1]):
//         new_genes = copy.copy(genes)
//         new_genes.append(index)
//         new_solution = Solution(new_genes)
//         assign_pairwise_distance_score(new_solution, distance_matrix)
// 
//         // prune if score cannot be better than the lower bound (lower)
//         best_insertions, best_costs = find_best_insertion(new_genes, remaining_ranges[1:], distance_matrix)
//         
//         potential_score = sum(best_costs) + new_solution.score
//         if len(remaining_ranges) == 1:  #at the end
//             potential_score = new_solution.score
//         if potential_score <= lower_bound:
//             new_solutions.append(new_solution)
//     return new_solutions
// }
// 
// void solveGA(string path, size_t popsize, size_t num_generations)
// {
//     // ouptut model parameters (some of them at least)
//     std::cerr << "Calculating RNA secondary consensus" << std::endl;
// 
//     // get the data
//     vector<vector<string> > prob1_data = read_data(path);
//     vector<vector<Tree> > trees = get_tree_lists(prob1_data);
//     unsigned long seeds[6] = {42, 42, 42, 42, 42, 42};
// 
// 
//     // create the tree problem instance and solve it
//     ConsensusProblem<Tree>* tree_problem = new ConsensusProblem<Tree>(trees, *unit_distance);
//     vector<Solution> tree_consensus = solve(tree_problem->get_distance_matrix(),
//                                             tree_problem->get_ranges(),
//                                             popsize, num_generations, seeds);
// 
//     // filter the dot brackets by the tree consensus
//     vector<vector< vector<string> > > prob2_data = vector< vector< vector<string> > >();
// 
//     // get the brackets
//     for(vector<Solution>::iterator it = tree_consensus.begin(); it != tree_consensus.end(); ++it) {
//         vector<string> brackets = vector<string>();
//         for(size_t j = 0; j != it->get_genes().size(); ++j) {
//             int gene = it->get_genes()[j];
//             brackets.push_back(tree_problem->get_objects()[gene].get_brackets());
//         }
//         // filter and add to the problem 2 data
//         vector< vector<string> > filtered_brackets = filter_dot_brackets(prob1_data, brackets);
//         prob2_data.push_back(filtered_brackets);
//     }
//     vector< ConsensusProblem<string>* > dot_bracket_problems = vector< ConsensusProblem<string>* >();
//     // instantiate the problems
//     for (size_t i = 0; i != prob2_data.size(); ++i) {
//         dot_bracket_problems.push_back(new ConsensusProblem<string>(prob2_data[i], *levenshtein));
//     }
// 
// 
// 
//     vector< vector<Solution> > dot_bracket_consensus = vector< vector<Solution> >();
//     for (size_t i = 0; i != prob2_data.size(); ++i) {
//         dot_bracket_consensus.push_back(solve(dot_bracket_problems[i]->get_distance_matrix(),
//                                               dot_bracket_problems[i]->get_ranges(),
//                                               popsize, num_generations, seeds));
//     }
// 
// 
//     // filter out suboptimal solutions
//     double best_score = std::numeric_limits<double>::infinity();
//     for(size_t i = 0; i != dot_bracket_consensus.size(); ++i) {
//         for(size_t j = 0; j != dot_bracket_consensus[i].size(); ++j) {
//             if (dot_bracket_consensus[i][j].get_score() < best_score) {
//                 best_score = dot_bracket_consensus[i][j].get_score();
//             }
//         }
//     }
// 
//     int sol_index = 0;
//     int gene;
//     for(size_t i = 0; i != dot_bracket_consensus.size(); ++i) {
//         for(size_t j = 0; j != dot_bracket_consensus[i].size(); ++j) {
//             if (dot_bracket_consensus[i][j].get_score() == best_score) {
//                 // output the consensus
//                 std::cout << "> " << sol_index << " " << best_score << std::endl;
//                 sol_index += 1;
//                 for (size_t gene_index = 0; gene_index != dot_bracket_consensus[i][j].get_genes().size(); ++gene_index) {
//                     gene = dot_bracket_consensus[i][j].get_genes()[gene_index];
//                     std::cout << dot_bracket_problems[i]->get_objects()[gene] << std::endl;
//                 }
//                 std::cout << std::endl;
//             }
//         }
//     }
// 
//     // cleanup
//     trees.clear();
//     delete tree_problem;
//     for(size_t i = 0; i < dot_bracket_problems.size(); ++i) {
//         delete dot_bracket_problems[i];
//     }
//     dot_bracket_problems.clear();
// 
//     return;
// }
// 
// 
// int main(int argc, char *argv[]) {
//     // fetch CLI arguments
//     if (argc == 4) {
//         string path = argv[1];
//         size_t popsize = atoi(argv[2]);
//         size_t num_generations = atoi(argv[3]);
//         solveGA(path, popsize, num_generations);
//     } else {
// 
//         std::cerr << "Wrong Command Line Arguments" << std::endl;
//         exit (EXIT_FAILURE);
//     }
// 
// }
