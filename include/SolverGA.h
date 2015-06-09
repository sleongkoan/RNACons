#ifndef SOLVERGA_H_
#define SOLVERGA_H_

#include "Solver.h"  // abstract class and useful functions

#include <algorithm>

#include <assert.h>


class SolverGA : public Solver{
public:
    // constructors and destructors
    SolverGA(// generic information
             bool silent,

             // GA settings
             int population_size,
             int num_generations,
             int improvement_depth,
             int elite_size,

             double crossover_prob,
             double mutation_prob,
             double improvement_prob);
    ~SolverGA();

    // solver call
    std::vector<Solution> solve(std::vector< std::vector<double> > distance_matrix,
                                std::vector<Range> ranges,
                                unsigned long seeds[6]) const;

private:
    int population_size_;
    int elite_size_;
    int num_generations_;
    int improvement_depth_;

    double crossover_prob_;
    double mutation_prob_;
    double improvement_prob_;
};




#endif // SOLVERGA_H_
