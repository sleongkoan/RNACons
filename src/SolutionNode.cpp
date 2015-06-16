
#include "../include/SolutionNode.h"


// constructor and destructor
SolutionNode::SolutionNode(std::vector<int> genes,
                           std::vector<RangeIndex> ranges,
                           double score)
{
    genes_ = genes;
    ranges_ = ranges;
    score_ = score;
    return;
}

SolutionNode::SolutionNode(const SolutionNode& other)
{
    // copy constructor
    genes_ = other.get_genes();
    ranges_ = other.get_ranges();
    score_ = other.get_score();
}


SolutionNode::~SolutionNode(){}



// getters and setters
const std::vector<int> & SolutionNode::get_genes() const
{
    return genes_;
}

void SolutionNode::set_gene(int position, int new_gene)
{
    genes_[position] = new_gene;
    return;
}

void SolutionNode::add_gene(int new_gene)
{
    genes_.push_back(new_gene);
    return;
}

std::vector<RangeIndex> SolutionNode::get_ranges() const
{
    return ranges_;
}

double SolutionNode::get_score() const
{
    return score_;
}

void SolutionNode::set_score(std::vector< std::vector<double> > elem_range_dm,
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


Solution SolutionNode::return_solution()
{
    // convert to a solution and return it
    std::sort(genes_.begin(), genes_.end());
    return Solution(genes_, score_);
}


bool SolutionNode::operator<(const SolutionNode &other) const
{
    return (score_ < other.score_);
}


bool SolutionNode::operator==(const SolutionNode &other)
{
    return (genes_ == other.genes_);
}


std::ostream& operator<< (std::ostream &out, const SolutionNode &sol)
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

