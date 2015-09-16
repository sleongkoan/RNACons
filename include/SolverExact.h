#ifndef SOLVEREXACT_H_
#define SOLVEREXACT_H_


#include <assert.h>

#include <algorithm>
#include <limits>
#include <queue>

#include "Solution.h"
#include "Solver.h"
#include "SubSolution.h"


class SolverExact: public Solver
{  // Branch & Bound solver

public:
    // constructors and destructors
    SolverExact(double suboptimal_threshold, bool silent);
    ~SolverExact();


    // solver call
    std::vector<Solution> solve(std::vector< std::vector<double> > distance_matrix,
                                std::vector<Range> ranges) const;


private:
    double suboptimal_threshold_;
};


#endif // SOLVEREXACT_H_

