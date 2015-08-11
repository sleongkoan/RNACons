#ifndef RNA2D_H
#define RNA2D_H


#include <algorithm>
#include <assert.h>
#include <string>
#include <vector>

// asserts that the Vienna dot bracket is valid
// (balanced, no illegal symbols)
bool is_valid_dot_bracket(std::string dot_bracket);


// removes the dots from the vienna dot bracket
std::string only_paired(std::string dot_bracket);


// converts the dot bracket to a level 5 RNA abstract shape
std::string shape_level_5( std::string dot_bracket);


#endif  // RNA2D_H