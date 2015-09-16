#ifndef SOLUTION_H
#define SOLUTION_H

#include <iostream>  // <<
#include <vector>
#include <limits>    // infinity for default score

class Solution
{
public:
    // constructors and destructors
    Solution(std::vector<int> genes=std::vector<int>(), double score=std::numeric_limits<double>::infinity());
    Solution(const Solution& other);
    ~Solution();

    // getters and setters
    const std::vector<int> & get_genes() const;
    void set_gene(int position, int new_gene);

    double get_score() const;
    void set_score(double new_score);

    // some comparison operators
    bool operator <(const Solution &other) const;
    bool operator ==(const Solution &other) const;

    // pretty printer
    friend std::ostream& operator<< (std::ostream &out, Solution &sol);

private:
    std::vector<int> genes_;
    double score_;
};


#endif // SOLUTION_H
