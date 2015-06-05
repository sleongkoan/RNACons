#ifndef SOLUTION_H
#define SOLUTION_H

#include <iostream>
#include <vector>

#include <limits>

class Solution
{
public:
    // constructors and destructors
    Solution();
    Solution(std::vector<int> chosen_genes);
    Solution(std::vector<int> chosen_genes, double score);
    Solution(const Solution& other); // copy constructor
    ~Solution();

    // getters and setters
    std::vector<int> get_genes() const;
    void set_gene(int position, int new_gene);
    void add_gene(int new_gene);

    double get_score() const;
    void set_score(double new_score);

    // some comparison operators
    bool operator <(const Solution &other) const;
    bool operator ==(const Solution &other);

    // pretty printer
    friend std::ostream& operator<< (std::ostream &out, Solution &sol);

private:
    std::vector<int> genes;
    double score;
};


#endif // SOLUTION_H
