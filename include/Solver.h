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

class Solver
{
public:
    Solver(bool silent)
    {
        silent_ = silent;
    }

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


void assign_pairwise_distance_score(Solution& sol,
                                    std::vector< std::vector<double> > distance_matrix);


std::vector<Solution> initialize_population(std::vector<Range> ranges,
                                            int pop_size, RngStream *prng);


void steepest_improvement(Solution& sol,
                          std::vector< std::vector<double> > distance_matrix,
                          std::vector<Range> ranges,
                          int max_iter=std::numeric_limits<int>::infinity());

std::vector<int> select_random_genes(std::vector<Range> ranges, RngStream *prng);

#endif  // SOLVER_H
