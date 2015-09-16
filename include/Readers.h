#ifndef READERS_H
#define READERS_H


#include <stdlib.h> // for exit


#include <fstream>  // ifstream
#include <iostream> // file reading
#include <string>
#include <vector>


// read MC-Cons input files (marna-like formatted lists of Vienna dot brackets)
std::vector<std::vector<std::string> > read_marna_file(std::string file_name);


// read MC-Cons output files (consensus files)
std::vector<std::vector<std::string> > read_consensus_file(std::string file_name);


#endif // READERS_H