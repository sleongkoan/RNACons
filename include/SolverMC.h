#ifndef SOLVERMC_H
#define SOLVERMC_H

#include <algorithm>
#include <vector>

#include <assert.h>

#include "Solver.h"


class SolverMC : public Solver{
public:
    // constructors and destructors
    SolverMC(// MC specific settings
             int sample_size,

             // generic information
             bool silent);

    ~SolverMC();

    // solver call
    std::vector<Solution> solve(std::vector< std::vector<double> > distance_matrix,
                                std::vector<Range> ranges,
                                unsigned long seeds[6]) const;

private:
    int sample_size_;
};


#endif // SOLVERMC_H
