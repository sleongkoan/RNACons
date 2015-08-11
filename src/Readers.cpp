#include "../include/Readers.h"


int get_subopt_length(std::string line)
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


std::vector<std::vector<std::string> > read_marna_file(std::string file_name)
{
    // read marna file format with suboptimal structures
    // we only take suboptimal structures as data, rest is ignored
    std::vector< std::vector<std::string> > data = std::vector< std::vector<std::string> >();

    // temporary structures
    std::string line;
    int subopt_length;
    std::vector<std::string> subopts = std::vector<std::string>();

    // work on the file
    std::ifstream dataFile(file_name.c_str());

    // check wether or not the file is open
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
                subopts = std::vector<std::string>();
            }
            else if (line[0] == '(' || line[0] == ')' || line[0] == '.')
            {
                // read the new suboptimal
                // must keep only the structure (first part)
                subopt_length = get_subopt_length(line);
                std::string subopt = line.substr(0, subopt_length);
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

/*
std::vector<std::string> read_consensus_file(std::string file_name)
{
    // read marna file format with suboptimal structures
    // we only take suboptimal structures as data, rest is ignored
    std::vector< std::vector<std::string> > data = std::vector< std::vector<std::string> >();

    // temporary structures
    std::string line;
    int subopt_length;
    std::vector<std::string> subopts = std::vector<std::string>();

    // work on the file
    std::ifstream dataFile(file_name.c_str());

    // check wether or not the file is open
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
                subopts = std::vector<std::string>();
            }
            else if (line[0] == '(' || line[0] == ')' || line[0] == '.')
            {
                // read the new suboptimal
                // must keep only the structure (first part)
                subopt_length = get_subopt_length(line);
                std::string subopt = line.substr(0, subopt_length);
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
}*/
