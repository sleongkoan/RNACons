#ifndef MCCONS_H
#define MCCONS_H

#include <utility>
#include <algorithm>
#include <string>


#include "Distances.h"
#include "ProgressBar.h"
#include "Readers.h"
#include "RNA2D.h"
#include "Solver.h"


// The whole two-step optimization procedure is done by this
// function which is controlled by feeding different solvers
void MCCONS(std::string path, Solver* tree_solver, Solver* dotbracket_solver);


#endif  // MCCONS_H