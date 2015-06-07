#ifndef SOLVERMC_H
#define SOLVERMC_H

#include <algorithm>
#include <vector>

#include <assert.h>
#include <math.h>

#include "ConsensusProblem.h"
#include "Solution.h"
#include "ProgressBar.h"
#include "RngStream.h"
#include "SolverGA.h"
#include "Tree.h"


class SolverMC {
public:
    // constructors and destructors
    SolverMC(
             // data
             std::vector< std::vector<double> > distance_matrix,
             std::vector<Range> ranges,

             // MC settings
             int sample_size,

             // misc parameters
             std::string problem_name,
             bool silent);

    ~SolverMC();

    // solver call
    std::vector<Solution> solve(unsigned long seeds[6]);

private:
    std::vector< std::vector<double> > _distance_matrix;
    std::vector<Range> _ranges;

    int _sample_size;

    std::string _problem_name;
    bool _silent;
};


#endif // SOLVERMC_H
