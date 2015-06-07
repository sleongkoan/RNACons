#include "../include/Solution.h"


Solution::Solution()
{
    genes = std::vector<int>();
    score = std::numeric_limits<double>::infinity();
}


Solution::Solution(std::vector<int> chosen_genes)
{
    genes = chosen_genes;
    score = std::numeric_limits<double>::infinity();
}


Solution::Solution(std::vector<int> chosen_genes, double calculated_score)
{
    this->genes = chosen_genes;
    this->score = calculated_score;

}

Solution::Solution(const Solution& other)
{
    // copy constructor
    genes = other.get_genes();
    score = other.get_score();
}


Solution::~Solution() { }


const std::vector<int> & Solution::get_genes() const
{
    return this->genes;
}

void Solution::set_gene(int position, int new_gene)
{
    this->genes[position] = new_gene;
}


void Solution::add_gene(int new_gene)
{
    this->genes.push_back(new_gene);
}


double Solution::get_score() const
{
    return this->score;
}

void Solution::set_score(double new_score)
{
    this->score = new_score;
}

bool Solution::operator<(const Solution &other) const
{
    return this->score < other.score;
}


bool Solution::operator==(const Solution &other)
{
    return this->genes == other.genes;
}


std::ostream& operator<< (std::ostream &out, Solution &sol)
{
    // outputs the genes and the score to the stream
    out << "[";
    for (std::vector<int>::iterator it = sol.genes.begin();
            it != sol.genes.end(); ++it)
    {
        out << *it << " ";
    }
    out << " : " << sol.score << "]" << std::endl;
    return out;
}



