

#include <algorithm>
#include <limits>

#include "../include/SolutionNode.h"


// constructors and destructors
SolverExact::SolverExact(bool silent) : Solver(silent)
{
    threshold_ = std::numeric_limits<double>::infinity();
}


SolverExact::~SolverExact() { }


SolverExact::set_threshold(double new_threshold)
{
    threshold_ = new_threshold;
}


double find_smallest_distance_range(int self_index,
                                    RangeIndex range,
                                    std::vector< std::vector<double> > distance_matrix)
{
    // find closest elements in other range and the distance
    best_distance = std::numeric_limits<double>::infinity();
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

    // matrix is (distance_matrix.size() x ranges.size())
    // initialize it
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
                                                                                  ranges_[range_index],
                                                                                  distance_matrix);
        }
    }
    return elem_range_dm;
}


void expand_solution(SolutionNode solution,
                     std::vector< std::vector<double> > elem_range_dm,
                     std::vector< std::vector<double> > distance_matrix,
                     std::vector< SolutionNode > & search_space,
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
        SolutionNode new_solution = SolutionNode(new_genes, new_ranges);
        new_solution.set_score(elem_range_dm, distance_matrix);

        if (new_solution.get_score() <= cutoff)
        {
            std::push_heap(search_space, new_solution);
        }
    }
    return;
}


bool compare_range(RangeIndex A, RangeIndex B) { return (A.high - A.low) < (B.high - B.low); }


std::vector<Solution> SolverExact::solve(std::vector< std::vector<double> > distance_matrix,
                                         std::vector<Range> ranges,
                                         unsigned long seeds[6]) const
{

    // could explore solutions by order of lowest
    // possible cost (most promising) or otherwise
    // only difference is the size the search space will get to
    std::vector<SolutionNode> search_space = std::vector<SolutionNode>();
    std::vector<SolutionNode> satisfying_solutions = std::vector<SolutionNode>();

    // some basic stuff
    std::vector< std::vector<double> > e_r_dm = elem_range_distance_matrix(ranges, distance_matrix)


    // sort the ranges by their sizes
    std::sort(ranges, compare_range);

    // initialize a stack to keep track of search space
    expand(SolutionNode(std::vector<int>(), ranges),
           e_r_dm,
           distance_matrix,
           search_space,
           threshold_);

    SolutionNode current_sol;
    while (search_space.size() > 0)
    {
        // fetch current solution
        current_sol = std::pop_heap(search_space);

        // if optimal, add to the best
        if (is_leaf(current_sol))
        {
            satisfying_solutions.push_back(current_sol);
        } else {
            expand(current_sol, e_r_dm, distance_matrix, search_space, threshold_);
        }
    }

    std::sort(satisfying_solutions);
    return satisfying_solutions;
}
