#ifndef SOLVER_H
#define SOLVER_H

#include <assert.h>

#include "ConsensusProblem.h"
#include "Solution.h"
#include "ProgressBar.h"
#include "RngStream.h"
#include "Tree.h"
#include "Solution.h"

#include <limits>
#include <vector>

// interface for all solvers
// any new solver must inherit this class if you
// want to plug it easily in the MC-Cons algorithm
class Solver
{
public:
    Solver(bool silent)
    {
        silent_ = silent;
    }

    // used by MC-Cons, accesses the status of the solver
    bool is_silent();

    virtual std::vector<Solution> solve(std::vector< std::vector<double> > distance_matrix,
                                        std::vector<Range> ranges,
                                        unsigned long seeds[6]) const = 0; // pure virtual


protected:
    bool silent_;
};


struct int_double
{
    int best_gene;
    double cost_difference;
};


// calculates and assigns the sum of pairwise distances
// of a solution and assign it as score_
void assign_pairwise_distance_score(Solution& sol,
                                    std::vector< std::vector<double> > distance_matrix);


// randomly chooses numbers fitting the boundaries of the ranges
std::vector<int> select_random_genes(std::vector<Range> ranges, RngStream *prng);

// create a list of Solutions generated randomly
std::vector<Solution> initialize_population(std::vector<Range> ranges,
                                            int pop_size, RngStream *prng);


// applies the steepest descent procedure until either
// a local minima is found or max_iter iterations of the substitution are done
void steepest_improvement(Solution& sol,
                          std::vector< std::vector<double> > distance_matrix,
                          std::vector<Range> ranges,
                          int max_iter=std::numeric_limits<int>::infinity());


#endif  // SOLVER_H
