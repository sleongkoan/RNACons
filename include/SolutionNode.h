#ifndef SOLUTIONNODE_H
#define SOLUTIONNODE_H

#include "Solution.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <limits>

struct RangeIndex
{
    int index;
    int low;
    int high;
};


// special solution type used for exact solver
class SolutionNode
{
public:
    // constructor and destructor
    SolutionNode(std::vector<int> genes,
                 std::vector<RangeIndex> ranges,
                 double score = std::numeric_limits<double>::max());
    SolutionNode(const SolutionNode& other);
    ~SolutionNode();

    // getters and setters
    const std::vector<int> & get_genes() const;
    void set_gene(int position, int new_gene);
    void add_gene(int new_gene);

    std::vector<RangeIndex> get_ranges() const;

    double get_score() const;
    void set_score(std::vector< std::vector<double> > elem_range_dm,
                   std::vector< std::vector<double> > distance_matrix);

    // return a copy of it as a solution
    Solution return_solution();

    // some comparison operators
    bool operator <(const SolutionNode &other) const;
    bool operator ==(const SolutionNode &other);

    // pretty printer
    friend std::ostream& operator<< (std::ostream &out, const SolutionNode &sol);


private:
    std::vector<int> genes_;
    std::vector<RangeIndex> ranges_;
    double score_;

};

#endif