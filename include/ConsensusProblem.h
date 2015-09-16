#ifndef CONSENSUSPROBLEM_H
#define CONSENSUSPROBLEM_H

#include <algorithm>
#include <limits>
#include <map>
#include <string>
#include <vector>

#include "Tree.h"


struct Range
{  // to keep the boundaries of each group of objects
    int low;
    int high;
};


template <class T>
class ConsensusProblem
{  // abstract consensus problem class
public:
    // constructor and destructor
    ConsensusProblem(std::vector< std::vector<T> > data,
                     int (*distance_function)(T a, T b));
    ~ConsensusProblem();

    // getters and setters
    const std::vector<T> & get_objects();
    const std::vector<Range> & get_ranges();
    const std::vector< std::vector<double> > & get_distance_matrix();


private:
    std::vector<T> objects_;
    std::vector<Range> ranges_;
    std::vector< std::vector<double> > distance_matrix_;
};


#endif // CONSENSUSPROBLEM_H

