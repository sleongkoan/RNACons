#include "../include/RNAConsensus.h"

using std::string;
using std::vector;


int get_subopt_length(string line)
{
    for (unsigned int i = 1; i < line.size(); ++i)
    {
        if (line[i] != '(' && line[i] != ')' && line[i] != '.')
        {
            return i;
        }
    }
    return 0; // will provoke failure later
}


bool is_nucleotide(char c)
{
    char lower_case = tolower(c);
    string nucleotides = "actgu";
    if (nucleotides.find(lower_case) == string::npos)
    {
        return false;
    }
    else
    {
        return true;
    }
}


vector<vector<string> > read_data(std::string file_name)
{
    // read marna file format with suboptimal structures
    // we only take suboptimal structures as data, rest is ignored
    vector< vector<string> > data = vector< vector<string> >();

    // temporary structures
    string line;
    int subopt_length;
    vector<string> subopts = vector<string>();

    // work on the file
    std::ifstream dataFile(file_name.c_str());

    //
    if (!dataFile.is_open())
    {
        std::cerr << "Error opening data file" << std::endl;
        exit (EXIT_FAILURE);
      }

    //assert (dataFile.is_open());
    if (dataFile.is_open())
    {
        while (std::getline(dataFile, line))
        {

            // figure out where we are
            if ((line[0] == '>') && (subopts.size() != 0))
            {
                // new subopts are coming, add old ones to the data
                data.push_back(subopts);
                subopts = vector<string>();
            }
            else if (line[0] == '(' || line[0] == ')' || line[0] == '.')
            {
                // read the new suboptimal
                // must keep only the structure (first part)
                subopt_length = get_subopt_length(line);
                string subopt = line.substr(0, subopt_length);
                subopts.push_back(subopt);
            }
            else
            {
                continue;
            }
        }
        if (subopts.size() != 0)
        {
            data.push_back(subopts);
        }
    }
    return data;
}


std::vector< std::vector<Tree> > get_tree_lists(std::vector< std::vector<std::string> > dot_brackets_vectors)
{
    // for efficiency reasons, might want to use a Set, but a vector will do for now
    vector< vector<Tree> > result = vector< vector<Tree> >();

    for (size_t i = 0; i < dot_brackets_vectors.size(); ++i)
    {
        // initialize a vector of trees to add later
        vector<string> current_unique_brackets = vector<string>();
        vector<Tree> current_unique_trees = vector<Tree>();

        for(size_t j = 0; j < dot_brackets_vectors[i].size(); ++j)
        {
            // calculate the bracket
            string bracket = only_paired(dot_brackets_vectors[i][j]);

            // check if it has already been added to the current inner vector
            bool found = false;
            for (size_t k = 0; k < current_unique_brackets.size(); ++k)
            {
                if (current_unique_brackets[k] == bracket)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                // create the new bracket and tree and add it to the current vector
                current_unique_brackets.push_back(bracket);
                current_unique_trees.push_back(Tree(bracket));
            }
        }
        // add to the result
        result.push_back(current_unique_trees);
    }

    return result;
}


std::vector< std::vector<std::string> > filter_dot_brackets(std::vector< std::vector<std::string> > dot_brackets_vectors, std::vector<std::string> brackets)
{
    // remove dot_brackets with the wrong base tree topology
    vector< vector<string> > result = vector< vector<string> >();
    for(unsigned int i = 0; i < dot_brackets_vectors.size(); ++i)
    {
        string bracket = brackets[i];
        vector<string> compliant_dot_brackets = vector<string>();

        for(unsigned int j = 0; j < dot_brackets_vectors[i].size(); ++j)
        {
            if(only_paired(dot_brackets_vectors[i][j]) == bracket)
            {
                compliant_dot_brackets.push_back(dot_brackets_vectors[i][j]);
            }
        }
        result.push_back(compliant_dot_brackets);
    }
    return result;
}

