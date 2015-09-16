#ifndef DISTANCES_H
#define DISTANCES_H

#include <string>
#include <vector>

#include "RNA2D.h"
#include "Tree.h"


// unit tree indel distance on strings (dot brackets)
int unit_tree_indel_distance_strings(const std::string first,
                                     const std::string second);


// unit tree indel distance on trees
int unit_tree_indel_distance_trees(const Tree first,
                                   const Tree second);


// string edit distance on ... strings
int string_edit_distance(std::string first,
                         std::string second);

#endif

