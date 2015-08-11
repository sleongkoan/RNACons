#ifndef RNA2D_H
#define RNA2D_H


#include <algorithm>
#include <assert.h>
#include <string>
#include <vector>



std::string only_paired(std::string dot_bracket);


bool is_valid_dot_bracket(std::string dot_bracket);


std::string db2shape( std::string dbwithdots);


#endif  // RNA2D_H