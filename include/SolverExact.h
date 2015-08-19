#ifndef SOLVEREXACT_H_
#define SOLVEREXACT_H_

#include "Solver.h"  // abstract class and useful functions
#include "Solution.h"
#include "SubSolution.h"

#include <algorithm>
#include <limits>
#include <queue>

#include <assert.h>


// solves the consensus problem using a branch and bound design
// calculation of the lower bound of a subsolution is described in the
// Solver.cpp implementation file
class SolverExact: public Solver{
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
