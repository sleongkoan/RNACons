#ifndef SRC_CPP_RNA_H_
#define SRC_CPP_RNA_H_

#include "Tree.h"
#include "RngStream.h"

#include <algorithm>

// rna secondary structure general helpers (conversion and distances)


bool is_valid_dot_bracket(std::string dot_bracket);

std::string only_paired(std::string dot_bracket);

int levenshtein(std::string  a, std::string  b);

Node* dot_bracket_to_node(std::string  dot_bracket);  // don't forget to free it

#endif  // SRC_CPP_RNA_H_
