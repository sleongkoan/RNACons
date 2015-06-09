#ifndef MCCONS_H
#define MCCONS_H

#include "Solver.h"
#include "RNAConsensus.h"

void MCCONS(std::string path,
            Solver* solver,
            unsigned long seeds[6]);


#endif  // MCCONS_H