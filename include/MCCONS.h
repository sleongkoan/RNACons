#ifndef MCCONS_H
#define MCCONS_H

#include <utility>
#include "Solver.h"
#include "RNAConsensus.h"


// The whole two-step optimization procedure is done by this
// function which is controlled by feeding different solvers
void MCCONS(std::string path,
            Solver* tree_solver,
            Solver* dotbracket_solver,
            unsigned long seeds[6]);


#endif  // MCCONS_H