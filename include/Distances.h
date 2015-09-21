#ifndef DISTANCES_H
#define DISTANCES_H

#include <string>
#include <vector>

#include "RNA2D.h"
#include "Tree.h"


// unit tree indel distance on strings (dot brackets)
int unit_tree_indel_distance_strings(const std::string string1,
                                     const std::string string2);


// unit tree indel distance on trees
int unit_tree_indel_distance_trees(const OrderedLabeledTree tree1,
                                   const OrderedLabeledTree tree2);


// string edit distance on ... strings
int string_edit_distance(std::string string1,
                         std::string string2);

#endif

