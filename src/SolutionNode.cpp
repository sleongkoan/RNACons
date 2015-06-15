
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

const std::vector<RangeIndex> & SolutionNode::get_ranges()
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
    for (size_t i = 0; i != genes_.size(); ++i)
    {
        for(size_t j =0; j != genes_.size(); ++j)
        {
            part1 += distance_matrix[i][j];
        }
    }

    // part 2: sum of distance from genes to nearest in remaining sets
    std::vector<int> range_indices = std::vector<int>();
    for (size_t i = 0; i != ranges_.size(); ++i)
    {
        range_indices.push_back(ranges_[i].index);
    }

    for (size_t i = 0; i != genes_.size(); ++i)
    {
        for (size_t j = 0; j != range_indices.size(); ++j)
        {
            part2 += elem_range_dm[i][j];
        }
    }

    score_ = part1 + part2;
    return;
}


bool SolutionNode::operator<(const SolutionNode &other) const
{
    return (score_ < other.score_);
}


bool SolutionNode::operator==(const SolutionNode &other)
{
    return (genes_ == other.genes_);
}


std::ostream& operator<< (std::ostream &out, SolutionNode &sol)
{
    // genes
    out << "genes ";
    for (size_t i = 0; i != sol.genes_.size()-1; ++i)
    {
        out << sol.genes_[i] << " - ";
    }
    // output the last one
    if (sol.genes_.size() > 1)
    {
        out << sol.genes_[sol.genes_.size()-1] << std::endl;
    }

    // score
    out << "score " << sol.score_ << std::endl;

    // remaining ranges
    out << "ranges" << std::endl;
    for(size_t range_index = 0; range_index != sol.ranges_.size(); ++range_index)
    {
        out << "[ " << sol.ranges_[range_index].low <<
        " .. " << sol.ranges_[range_index].high << " ]" << std::endl;
    }
    return out;
}

