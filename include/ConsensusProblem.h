#ifndef CONSENSUSPROBLEM_H
#define CONSENSUSPROBLEM_H

#include <algorithm>
#include <limits>
#include <map>
#include <string>
#include <vector>

#include "Tree.h"

struct Range
{
    int low;
    int high;
}; // to keep the boundaries of each group of objects


template <class T>
class ConsensusProblem
{
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

// used to assert that the distance matrix passed
// is at least symmetric and has zero on the diagonal
bool distance_is_symmetric(const std::vector< std::vector<double> > & distance_matrix);


#endif // CONSENSUSPROBLEM_H
