#include "../include/ShapeExplorer.h"


using optparse::OptionParser;


void join(const std::vector<std::string>& v, char c, std::string& s)
{
   // simple string joining method
   s.clear();

   for (std::vector<std::string>::const_iterator p = v.begin();p != v.end(); ++p)
   {
      s += *p;
      if (p != v.end() - 1)
        s += c;
   }
}


class Consensus
{
public:
    std::vector<std::string> shapes_;
    std::string shapes_str_;
    std::vector<std::string> structures_;
    double tree_dist_;
    double string_dist_;
    int index_;

    Consensus(std::vector<std::string> structure_list)
    {
        // keep the structures for pretty printing
        structures_ = std::vector<std::string>();
        for(size_t i = 0; i != structure_list.size(); ++i)
        {
            structures_.push_back(std::string(structure_list[i]));
        }

        // calculate a normalizing constant, n * n -1 unique comparisons
        double normalizing_constant = structure_list.size();
        normalizing_constant *= (normalizing_constant - 1);

        // calculate the shape signature (unique arrangement of shapes in the consensus)
        std::vector<std::string> shapes_ = std::vector<std::string>();
        for (size_t j = 0; j != structure_list.size(); ++j)
        {
            shapes_.push_back(shape_level_5(structure_list[j]));
        }
        std::string shapes_str_;
        join(shapes_, '\n', shapes_str_);

        // calculate both the unit tree indel distance and the string edit distance
        double tree_dist_ = 0;
        double string_dist_ = 0;

        for (size_t j = 0; j != structure_list.size(); ++j)
        {
            for(size_t k = 0; k != structure_list.size(); ++k)
            {
                tree_dist_ += unit_tree_indel_distance_strings(structure_list[j], structure_list[k]);
                string_dist_ += string_edit_distance(structure_list[j], structure_list[k]);
            }
        }
        tree_dist_ /= normalizing_constant;
        string_dist_ /= normalizing_constant;
        index_ = -1;
        return;
    }


    bool operator<(const Consensus &other) const
    {
        bool smaller = true;
        if (tree_dist_ == other.tree_dist_)
        {
            smaller = (string_dist_ < other.string_dist_);
        }
        else
        {
            smaller = (tree_dist_ < other.tree_dist_);
        }
        return smaller;
    }


    bool operator==(const Consensus &other) const
    {
        return ((tree_dist_ == other.tree_dist_) &&
                (string_dist_ == other.string_dist_));
    }

    friend std::ostream& operator<< (std::ostream &out, Consensus &cons);

};


std::ostream& operator<< (std::ostream &out, Consensus &cons)
{
    // outputs the genes and the score to the stream
    out << "> " << cons.index_ << " " << cons.tree_dist_ << " " << cons.string_dist_ << std::endl;
    for (size_t i = 0; i != cons.structures_.size(); ++i)
    {
        out << cons.structures_[i] << std::endl;;
    }
    out << std::endl;
    return out;
}



std::vector<Consensus> find_shape_representatives(std::vector<Consensus> consensus_list)
{
    // separate the consensus by arrangement of the shapes
    std::map<std::string, std::vector<Consensus> > shapes_to_consensus = std::map<std::string, std::vector<Consensus> >();
    for (size_t i  = 0; i != consensus_list.size(); ++i)
    {
        Consensus current = consensus_list[i];
        if ( shapes_to_consensus.find(current.shapes_str_) == shapes_to_consensus.end() )
        {
            shapes_to_consensus[current.shapes_str_] = std::vector<Consensus>();
        }
        shapes_to_consensus[current.shapes_str_].push_back(current);
    }

    // sort the consensus by their score and keep only the best
    std::vector<Consensus> best_consensus = std::vector<Consensus>();
    double best_tree_dist, best_string_dist;
    std::map<std::string, std::vector<Consensus> >::iterator consensus_iterator = std::map<std::string, std::vector<Consensus> >::iterator();
    for(consensus_iterator = shapes_to_consensus.begin();
        consensus_iterator != shapes_to_consensus.end();
        consensus_iterator++)
    {
        // sort the all the consensus having the current shape
        std::sort(consensus_iterator->second.begin(), consensus_iterator->second.end());

        // keep only the best consensus
        best_tree_dist = consensus_iterator->second[0].tree_dist_;
        best_string_dist = consensus_iterator->second[0].string_dist_;
        for(size_t i = 0; i != consensus_iterator->second.size(); ++i)
        {
            if ((consensus_iterator->second[i].tree_dist_ == best_tree_dist) &&
                (consensus_iterator->second[i].string_dist_ == best_string_dist))
            {
                best_consensus.push_back(Consensus(consensus_iterator->second[i]));
            }
            else
            {
              break;
            }
        }
    }
    return best_consensus;
}




int main(int argc, char *argv[])
{

    // create the command line parser
    OptionParser parser = OptionParser().description("Shape Explorer: finding best consensus for each unique arrangement of level 5 RNA abstract shapes");
    parser.add_option("-i", "--input").dest("data_file").help("path to a MC-Cons output file");
    optparse::Values options = parser.parse_args(argc, argv);


    if (options.is_set("data_file"))
    {
      // read the consensus file
      std::string path = options["data_file"];
      std::vector< std::vector< std::string> > consensus_list = read_consensus_file(path);

      // convert to a list of consensus
      std::vector<Consensus> input_consensus = std::vector<Consensus>();
      for (size_t i = 0; i != consensus_list.size(); ++i)
      {
          input_consensus.push_back(Consensus(consensus_list[i]));
      }

      // filter the consensus
      std::vector<Consensus> best_consensus = find_shape_representatives(input_consensus);

      // write to stdout
      for(size_t i = 0; i != best_consensus.size(); ++i)
      {
          best_consensus[i].index_ = i;
          std::cout << best_consensus[i];
      }
      return 1;

    } else
    {
      // something went wrong with the arguemnts, print error message and exit
      std::cerr << "Error: something went wrong, please check usage -h or --help" << std::endl;
      std::exit(0);
    }
}
