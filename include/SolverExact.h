#ifndef SOLVEREXACT_H_
#define SOLVEREXACT_H_

#include "Solver.h"  // abstract class and useful functions
#include "SolutionNode.h"

#include <assert.h>


class SolverExact: public Solver{
public:
    // constructors and destructors
    SolverExact(// misc parameters
                bool silent,
                // exact solver parameters
                double best_known_score);
    ~SolverExact();

    // solver call
    std::vector<Solution> solve(std::vector< std::vector<double> > distance_matrix,
                                std::vector<Range> ranges,
                                unsigned long seeds[6]) const;
    void set_threshold(double new_threshold);

private:
    double threshold_;
};




#endif // SOLVEREXACT_H_
