#ifndef SOLVER_H
#define SOLVER_H

#include <assert.h>

#include <limits>
#include <vector>

#include "ConsensusProblem.h"
#include "ProgressBar.h"
#include "RngStream.h"
#include "Solution.h"
#include "Tree.h"



class Solver
{  // abstract base class

public:
    // ctor
    Solver(bool silent) { silent_ = silent; }

    // silent status
    void flip_silent() { silent_ = !silent_; }
    bool is_silent() { return silent_; }

    // method to implement for inheriting classes
    virtual std::vector<Solution> solve(std::vector< std::vector<double> > distance_matrix,
                                        std::vector<Range> ranges) const = 0; // pure virtual


protected:
    bool silent_;
};



struct int_double
{  // simple data holder
    int best_gene;
    double cost_difference;
};


// calculates and assigns the sum of pairwise distances
// of a solution and assign it as score_
void assign_pairwise_distance_score(Solution& sol, std::vector< std::vector<double> > distance_matrix);


// randomly chooses numbers fitting the boundaries of the ranges
std::vector<int> select_random_genes(std::vector<Range> ranges, RngStream *prng);


// create a list of Solutions generated randomly
std::vector<Solution> initialize_population(std::vector<Range> ranges, int pop_size, RngStream *prng);


// applies the steepest descent procedure until either
// a local minima is found or max_iter iterations of the substitution are done
void steepest_improvement(Solution& sol,
                          std::vector< std::vector<double> > distance_matrix,
                          std::vector<Range> ranges,
                          int max_iter=std::numeric_limits<int>::infinity());


#endif  // SOLVER_H
