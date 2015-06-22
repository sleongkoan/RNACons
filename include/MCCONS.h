#ifndef MCCONS_H
#define MCCONS_H

#include "Solver.h"
#include "RNAConsensus.h"

// The whole two-step optimization procedure is done by this
// function which is controlled by feeding different solvers
void MCCONS(std::string path,
            Solver* solver,
            unsigned long seeds[6]);


#endif  // MCCONS_H