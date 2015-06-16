#ifndef SOLVEREXACT_H_
#define SOLVEREXACT_H_

#include "Solver.h"  // abstract class and useful functions
#include "Solution.h"
#include "SolutionNode.h"

#include <algorithm>
#include <limits>
#include <queue>

#include <assert.h>

class SolverExact: public Solver{
public:
    // constructors and destructors
    SolverExact(bool silent);

    ~SolverExact();

    // solver call
    std::vector<Solution> solve(std::vector< std::vector<double> > distance_matrix,
                                std::vector<Range> ranges,
                                unsigned long seeds[6]) const;
};




#endif // SOLVEREXACT_H_
