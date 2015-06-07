#ifndef SOLVERGA_H
#define SOLVERGA_H

#include <algorithm>
#include <vector>


#include <assert.h>
#include <math.h>

#include "ConsensusProblem.h"
#include "Solution.h"
#include "ProgressBar.h"
#include "RngStream.h"
#include "Tree.h"


class SolverGA {
public:
    // constructors and destructors
    SolverGA(// data
             std::vector< std::vector<double> > distance_matrix,
             std::vector<Range> ranges,

             // GA settings
             int population_size,
             int num_generations,
             int improvement_depth,
             int elite_size,

             double crossover_prob,
             double mutation_prob,
             double improvement_prob,

             // misc parameters
             std::string problem_name,
             bool silent);
    ~SolverGA();

    // solver call
    std::vector<Solution> solve(unsigned long seeds[6]) const;

private:
    std::vector< std::vector<double> > _distance_matrix;
    std::vector<Range> _ranges;

    int _population_size;
    int _elite_size;
    int _num_generations;
    int _improvement_depth;

    double _crossover_prob;
    double _mutation_prob;
    double _improvement_prob;

    std::string _problem_name;
    bool _silent;
};


void assign_pairwise_distance_score(Solution& sol,
                                    std::vector< std::vector<double> > distance_matrix);


std::vector<Solution> initialize_population(std::vector<Range> ranges,
                                            int pop_size, RngStream *prng);


void steepest_improvement(Solution& sol,
                          std::vector< std::vector<double> > distance_matrix,
                          std::vector<Range> ranges,
                          int max_iter=pow(2, 20));


#endif // SOLVERGA_H
