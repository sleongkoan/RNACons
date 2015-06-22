#include "../include/Solution.h"


Solution::Solution(std::vector<int> genes, double score)
{
    genes_ = genes;
    score_ = score;

}


Solution::Solution(const Solution& other)
{
    // copy constructor
    genes_ = other.get_genes();
    score_ = other.get_score();
}


Solution::~Solution() { }


const std::vector<int> & Solution::get_genes() const
{
    return genes_;
}


void Solution::set_gene(int position, int new_gene)
{
    genes_[position] = new_gene;
}


double Solution::get_score() const
{
    return score_;
}

void Solution::set_score(double new_score)
{
    score_ = new_score;
}

bool Solution::operator<(const Solution &other) const
{
    return score_ < other.score_;
}


bool Solution::operator==(const Solution &other)
{
    return genes_ == other.genes_;
}


std::ostream& operator<< (std::ostream &out, Solution &sol)
{
    // outputs the genes and the score to the stream
    out << "[";
    for (size_t i = 0; i != sol.genes_.size(); ++i)
    {
        out << sol.genes_[i] << " ";
    }
    out << " : " << sol.score_ << "]" << std::endl;
    return out;
}



