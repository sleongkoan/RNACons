#include "../include/ShapeExplorer.h"


using optparse::OptionParser;


std::string join(const std::vector<std::string> stringsToJoin, char separator)
{
    std::ostringstream buffer;
    for (size_t i = 0; i != stringsToJoin.size(); ++i)
    {
        buffer << separator << stringsToJoin[i];
    }
    buffer << separator;
    return buffer.str();
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

    Consensus(std::vector<std::string> structure_list, int level)
    {
        // keep the structures for pretty printing
        structures_ = std::vector<std::string>();
        for(size_t i = 0; i != structure_list.size(); ++i)
        {
            structures_.push_back(std::string(structure_list[i]));
        }


        // calculate the shape signature (unique arrangement of shapes in the consensus)
        shapes_ = std::vector<std::string>();
        for (size_t i = 0; i != structure_list.size(); ++i)
        {
            shapes_.push_back(std::string(RNAshapes(structure_list[i], level)));
        }
        shapes_str_ = join(shapes_, '\n');

        // calculate a normalizing constant, n * n -1 unique comparisons
        double normalizing_constant = structure_list.size();
        normalizing_constant *= (normalizing_constant - 1);

        // calculate both the unit tree indel distance and the string edit distance
        tree_dist_ = 0;
        string_dist_ = 0;

        for (size_t i = 0; i != structure_list.size(); ++i)
        {
            for(size_t j = 0; j != structure_list.size(); ++j)
            {
                tree_dist_ += unit_tree_indel_distance_strings(structures_[i], structures_[j]);
                string_dist_ += string_edit_distance(structures_[i], structures_[j]);

            }
        }
        tree_dist_ = tree_dist_ / normalizing_constant;
        string_dist_ = string_dist_ / normalizing_constant;
        index_ = -1;
        return;
    }

    Consensus(const Consensus& other):
    shapes_(other.shapes_), structures_(other.structures_),
    shapes_str_(other.shapes_str_), tree_dist_(other.tree_dist_),
    string_dist_(other.string_dist_), index_(other.index_)
    { }


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


    std::string to_str_with_shapes()
    {
        std::ostringstream buffer;
        buffer << "> " << index_ << " " << tree_dist_ << " " << string_dist_ << '\n';
        for (size_t i = 0; i != structures_.size(); ++i)
        {
            buffer << structures_[i] << "\t" << shapes_[i] << '\n';
        }
        buffer << '\n';
        return buffer.str();
    }

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
        shapes_to_consensus[current.shapes_str_].push_back(Consensus(current));
    }

    // sort the consensus by their score and keep only the best
    std::vector<Consensus> best_consensus = std::vector<Consensus>();
    std::map<std::string, std::vector<Consensus> >::iterator consensus_iterator = 
    std::map<std::string, std::vector<Consensus> >::iterator();

    for(consensus_iterator = shapes_to_consensus.begin();
        consensus_iterator != shapes_to_consensus.end();
        consensus_iterator++)
    {
        // sort the all the consensus having the current shape
        std::sort(consensus_iterator->second.begin(), consensus_iterator->second.end());

        // keep only the best consensus
        Consensus best_current(consensus_iterator->second[0]);
        for(size_t i = 0; i != consensus_iterator->second.size(); ++i)
        {
            Consensus current = Consensus(consensus_iterator->second[i]);
            if (current == best_current)
            {
                best_consensus.push_back(Consensus(current));
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
    parser.add_option("-l", "--level").dest("shape_level").help("level of the shape used (1, 3 or 5)").type("int");
    parser.add_option("-s", "--shape").dest("shape").action("store_true").help("display shapes alongside structures");
    optparse::Values options = parser.parse_args(argc, argv);

    if (options.is_set("data_file"))
    {
      // parse the shape level desired
      int level = atoi(options["shape_level"].c_str());
      assert (level == 1 || level == 3 || level == 5);


      // read the consensus file
      std::string path = options["data_file"];
      std::vector< std::vector< std::string> > consensus_list = read_consensus_file(path);


      // convert to a list of consensus
      std::vector<Consensus> input_consensus = std::vector<Consensus>();
      for (size_t i = 0; i != consensus_list.size(); ++i)
      {
          input_consensus.push_back(Consensus(consensus_list[i], level));
          input_consensus[i].index_ = i;
      }


      // filter the consensus
      std::vector<Consensus> best_consensus = find_shape_representatives(input_consensus);

      // write to stdout
      if (options.is_set("shape"))
      {
          for(size_t i = 0; i != best_consensus.size(); ++i)
          {
            best_consensus[i].index_ = i;
            std::cout << best_consensus[i].to_str_with_shapes();
          }
      }
      else
      {
          for(size_t i = 0; i != best_consensus.size(); ++i)
          {
              best_consensus[i].index_ = i;
              std::cout << best_consensus[i];
          }
      }
      return 1;

    } else
    {
      // something went wrong with the arguemnts, print error message and exit
      std::cerr << "Error: something went wrong, please check usage -h or --help" << std::endl;
      std::exit(0);
    }
}
