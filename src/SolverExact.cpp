

#include "../include/SolverExact.h"


// constructors and destructors
SolverExact::SolverExact(bool silent) : Solver(silent) { }

SolverExact::~SolverExact() { }


double find_smallest_distance_range(int self_index,
                                    RangeIndex range,
                                    std::vector< std::vector<double> > distance_matrix)
{
    // find closest elements in other range and the distance
    double best_distance = std::numeric_limits<double>::infinity();
    double distance;

    for (int i = range.low; i != range.high; ++i)
    {
        distance = distance_matrix[self_index][i];
        if (distance < best_distance)
        {
            best_distance = distance;
        }
    }
    return best_distance;
}


std::vector< std::vector<double> > elem_range_distance_matrix(std::vector<RangeIndex> ranges,
                                                              std::vector< std::vector<double> > distance_matrix)
{
    // using the distance matrix, find closest neighbors for
    // each object, in each other groups (range)

    // matrix is (distance_matrix.size() x ranges.size()), initialize it
    std::vector< std::vector<double> > elem_range_dm = std::vector< std::vector<double> >();
    for(size_t i = 0; i != distance_matrix.size(); ++i)
    {
        elem_range_dm.push_back(std::vector<double>(ranges.size(), 0));
    }

    // for each elem, for each range, find the closest distance between the two
    for (size_t elem_index = 0; elem_index != distance_matrix.size(); ++ elem_index)
    {
        for (size_t range_index = 0; range_index != ranges.size(); ++ range_index)
        {
            elem_range_dm[elem_index][range_index] = find_smallest_distance_range(elem_index,
                                                                                  ranges[range_index],
                                                                                  distance_matrix);
        }
    }
    return elem_range_dm;
}


bool is_leaf(SubSolution sol)
{
    // checks wether or not a solution is complete
    return (sol.get_ranges().size() == 0);
}


void expand_solution(SubSolution solution,
                     std::vector< std::vector<double> > elem_range_dm,
                     std::vector< std::vector<double> > distance_matrix,
                     std::priority_queue< SubSolution > & search_space,
                     double cutoff)
{
    // used to create all children of a node
    // and add them, if they are promising enough, to the
    // search space (which is a max heap)
    assert((is_leaf(solution) == false) && "applying on complete solution");

    // separate the ranges, the one to explore and the rest
    RangeIndex range_to_explore = solution.get_ranges()[0];
    std::vector<RangeIndex> new_ranges = std::vector<RangeIndex>();
    for (size_t i = 1; i != solution.get_ranges().size(); ++i)
    {
        new_ranges.push_back(solution.get_ranges()[i]);
    }

    for (size_t new_gene = range_to_explore.low;
         new_gene != range_to_explore.high;
         ++new_gene)
    {
        // create the new genes
        std::vector<int> new_genes = std::vector<int>();
        for (size_t g = 0; g != solution.get_genes().size(); ++g)
        {
            new_genes.push_back(solution.get_genes()[g]);
        }
        new_genes.push_back(new_gene);

        // create the new solution, score it and add it if its worth it
        SubSolution new_solution = SubSolution(new_genes, new_ranges);
        new_solution.set_score(elem_range_dm, distance_matrix);

        if (new_solution.get_score() <= cutoff)
        {
            search_space.push(new_solution);
        }
    }
    return;
}


bool compare_range(RangeIndex A, RangeIndex B) { return (A.high - A.low) < (B.high - B.low); }


std::vector<Solution> SolverExact::solve(std::vector< std::vector<double> > distance_matrix,
                                         std::vector<Range> ranges,
                                         unsigned long seeds[6]) const
{
    // use a priority queue to represent the current search space
    std::priority_queue<SubSolution> search_space = std::priority_queue<SubSolution>();
    std::vector<SubSolution> satisfying_solutions = std::vector<SubSolution>();
    double best_score = std::numeric_limits<double>::max();

    // sort the ranges by their size (to avoid big branching at the start)
    std::vector<RangeIndex> sorted_ranges = std::vector<RangeIndex>();
    for (int i = 0; i != ranges.size(); ++i)
    {
        RangeIndex converted = {i, ranges[i].low, ranges[i].high};
        sorted_ranges.push_back(converted);
    }
    std::sort(sorted_ranges.begin(), sorted_ranges.end(), compare_range);


    // initialize the search space with the empty subsolution
    search_space.push(SubSolution(std::vector<int>(), sorted_ranges));
    std::vector< std::vector<double> > e_r_dm = elem_range_distance_matrix(sorted_ranges, distance_matrix);


    int iteration = 0;
    while (search_space.size() > 0)
    {

        // fetch current solution
        SubSolution current_sol = search_space.top();
        search_space.pop();

        // if optimal, add to the best
        if ((is_leaf(current_sol)) && (current_sol.get_score() <= best_score))
        {
            best_score = current_sol.get_score();
            satisfying_solutions.push_back(current_sol);
        } else {
            expand_solution(current_sol, e_r_dm, distance_matrix, search_space, best_score);
        }

        iteration += 1;
    }

    // filter out the solutions
    std::vector<Solution> final_solutions = std::vector<Solution>();
    for (size_t i = 0; i != satisfying_solutions.size(); ++i)
    {
        // add only those with optimal score
        if (satisfying_solutions[i].get_score() == best_score)
        {
            final_solutions.push_back(satisfying_solutions[i].return_solution());
        }
    }
    return final_solutions;
}
