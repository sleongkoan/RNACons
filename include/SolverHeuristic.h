#ifndef SOLVERHEURISTIC_H_
#define SOLVERHEURISTIC_H_

#include <algorithm>
#include <assert.h>


#include "Solver.h"  // abstract class and useful functions


class SolverHeuristic : public Solver
{  // hybrid genetic algorithm / steepest descent

public:
    // constructors and destructors
    SolverHeuristic(// generic information
             bool silent,

             // heuristic parameters
             int population_size,
             int num_generations,
             int improvement_depth,
             int elite_size,
             double distance_threshold,

             double crossover_prob,
             double mutation_prob,
             double improvement_prob,
             unsigned long seeds[6]);
    ~SolverHeuristic();

    // solver call
    std::vector<Solution> solve(std::vector< std::vector<double> > distance_matrix,
                                std::vector<Range> ranges) const;


private:
    int population_size_;
    int elite_size_;
    int num_generations_;
    int improvement_depth_;

    double distance_threshold_;
    double crossover_prob_;
    double mutation_prob_;
    double improvement_prob_;
    unsigned long * seeds_;
};


#endif // SOLVERHEURISTIC_H_

