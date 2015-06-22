
#include "../include/SubSolution.h"


// constructor and destructor
SubSolution::SubSolution(std::vector<int> genes,
                         std::vector<RangeIndex> ranges,
                         double score)
{
    genes_ = genes;
    ranges_ = ranges;
    score_ = score;
    return;
}

SubSolution::SubSolution(const SubSolution& other)
{
    // copy constructor
    genes_ = other.get_genes();
    ranges_ = other.get_ranges();
    score_ = other.get_score();
}


SubSolution::~SubSolution(){}



// getters and setters
const std::vector<int> & SubSolution::get_genes() const
{
    return genes_;
}

void SubSolution::set_gene(int position, int new_gene)
{
    genes_[position] = new_gene;
    return;
}


std::vector<RangeIndex> SubSolution::get_ranges() const
{
    return ranges_;
}

double SubSolution::get_score() const
{
    return score_;
}

void SubSolution::set_score(std::vector< std::vector<double> > elem_range_dm,
                             std::vector< std::vector<double> > distance_matrix)
{
    // set the score to the best possible score for this solution
    double part1 = 0.;
    double part2 = 0.;

    // calculate the chosen gene contribution
    int gene1, gene2, range_index;
    for (size_t i = 0; i != genes_.size(); ++i)
    {
        gene1 = genes_[i];
        for(size_t j = 0; j != genes_.size(); ++j)
        {
            gene2= genes_[j];
            part1 += distance_matrix[gene1][gene2];
        }
    }

    // part 2: sum of distance from genes to nearest in remaining sets
    for (size_t i = 0; i != genes_.size(); ++i)
    {
        gene1 = genes_[i];
        for (size_t j = 0; j != ranges_.size(); ++j)
        {
            range_index = ranges_[j].index;
            part2 += elem_range_dm[gene1][range_index];
        }
    }

    score_ = part1 + part2;
    return;
}


Solution SubSolution::return_solution()
{
    // convert a subsolution to a solution and return it
    std::sort(genes_.begin(), genes_.end());
    return Solution(genes_, score_);
}


bool SubSolution::operator<(const SubSolution &other) const
{
    return (score_ < other.score_);
}


bool SubSolution::operator==(const SubSolution &other)
{
    return (genes_ == other.genes_);
}


std::ostream& operator<< (std::ostream &out, const SubSolution &sol)
{
    // genes
    out << "genes ";
    for (size_t i = 0; i < sol.genes_.size(); ++i)
    {
        out << sol.genes_[i] << " ";
    }
    out << std::endl;

    // score
    out << "score " << sol.score_ << std::endl;

    // remaining ranges
    out << "ranges" << std::endl;
    for(size_t range_index = 0; range_index < sol.ranges_.size(); ++range_index)
    {
        out << "[ " << sol.ranges_[range_index].index << " " << sol.ranges_[range_index].low <<
        " .. " << sol.ranges_[range_index].high << " ]" << std::endl;
    }
    return out;
}

