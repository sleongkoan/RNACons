#ifndef RNACONSENSUS_H
#define RNACONSENSUS_H

#include "Tree.h"

#include <algorithm>
#include <string>
#include <iostream> // file reading
#include <fstream>  // ifstream
#include <ctype.h>  // tolower()
#include <stdlib.h> // for exit

// rna secondary structure general helpers (conversion and distances)


int levenshtein(std::string a, std::string b);


// file IO and MC-Cons utilities
std::vector<std::vector<std::string> > read_data(std::string file_name);

std::vector< std::vector<Tree> > get_tree_lists(std::vector< std::vector<std::string> > dot_brackets_vectors);

std::vector< std::vector<std::string> > filter_dot_brackets(std::vector< std::vector<std::string> > dot_brackets_vectors, std::vector<std::string> brackets);


#endif // RNACONSENSUS_H
