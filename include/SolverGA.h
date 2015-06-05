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

void assign_pairwise_distance_score(Solution& sol, std::vector< std::vector<double> > distance_matrix);

std::vector<Solution> initialize_population(std::vector<Range> ranges, int pop_size, RngStream *prng);

void steepest_improvement(Solution& sol,
                          std::vector< std::vector<double> > distance_matrix,
                          std::vector<Range> ranges,
                          int max_iter=pow(2, 20));

std::vector<Solution> solve(std::vector< std::vector<double> > distance_matrix,
                        std::vector<Range> ranges,
                        size_t population_size,
                        size_t num_generations,
                        unsigned long seeds[6], // takes 6 seeds, unsigned long
                        size_t elite_size = 30,
                        double crossover_prob=0.5,
                        double mutation_prob=0.05,
                        double improvement_prob=0.05,
                        size_t improvement_depth=2,
                        std::string problem_name="",
                        bool silent=false );

#endif // SOLVERGA_H
