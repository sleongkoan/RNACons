#ifndef SOLUTIONNODE_H
#define SOLUTIONNODE_H


#include <algorithm>
#include <iostream>
#include <vector>
#include <limits>

#include "Solution.h"


struct RangeIndex
{  // modified range holder
    int index;
    int low;
    int high;
};


class SubSolution
{  // special solution type used for Branch & Bound

public:
    // constructor and destructor
    SubSolution(std::vector<int> genes,
                 std::vector<RangeIndex> ranges,
                 double score = std::numeric_limits<double>::max());
    SubSolution(const SubSolution& other);
    ~SubSolution();

    // getters and setters
    const std::vector<int> & get_genes() const;
    void set_gene(int position, int new_gene);

    std::vector<RangeIndex> get_ranges() const;

    double get_score() const;
    void set_score(std::vector< std::vector<double> > elem_range_dm,
                   std::vector< std::vector<double> > distance_matrix);

    // return a copy of it as a solution
    Solution return_solution();

    // some comparison operators
    bool operator <(const SubSolution &other) const;
    bool operator ==(const SubSolution &other);

    // pretty printer
    friend std::ostream& operator<< (std::ostream &out, const SubSolution &sol);


private:
    std::vector<int> genes_;
    std::vector<RangeIndex> ranges_;
    double score_;
};


#endif