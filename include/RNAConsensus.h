#ifndef RNACONSENSUS_H
#define RNACONSENSUS_H

#include "RNA.h"  // need some tree and dot bracket methods
#include <string>
#include <iostream> // file reading
#include <fstream>
#include <ctype.h>  // tolower()
#include <stdlib.h> // for exit

std::vector<std::vector<std::string> > read_data(std::string file_name);

std::vector< std::vector<Tree> > get_tree_lists(std::vector< std::vector<std::string> > dot_brackets_vectors);

std::vector< std::vector<std::string> > filter_dot_brackets(std::vector< std::vector<std::string> > dot_brackets_vectors, std::vector<std::string> brackets);

#endif // RNACONSENSUS_H
