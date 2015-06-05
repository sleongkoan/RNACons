#include "Test.h"

Test::Test()
{
    //ctor
}

Test::~Test()
{
    //dtor
}


bool test_RNAConsensus() {
    bool working = true;
    // step 1: reading from file
    vector<vector<string> > data = read_data("../../../data/others/DM1-33.txt");
    for(int i =0 ; i != data.size(); ++i) {
        std::cout << std::endl;
        for(int j= 0; j < data[i].size(); ++j) {
            std::cout << data[i][j] << std::endl;
        }
    }
    if (data.size() == 0)
    {
        working = false;
    }
    // step 2: converting dotbrackets to trees in efficient fashion
    vector< vector<Tree*> > trees = get_tree_lists(data);
    if (trees.size() == 0)
    {
        working = false;
    }

    // step 3: filtering
    vector<string> consensus = vector<string>();
    for(unsigned int i = 0; i < trees.size(); ++i)
    {
        consensus.push_back(trees[i][0]->get_brackets());
    }
    vector< vector<string> > filtered = filter_dot_brackets(data, consensus);


    for(unsigned int i = 0; i < consensus.size(); ++i)
    {
        for(unsigned int j = 0; j < filtered[i].size(); ++j)
        {
            if ( consensus[i] != only_paired(filtered[i][j]) )
            {
                working = false;
                break;
            }
        }
    }

    // free structures
    for(size_t i = 0; i < trees.size(); ++i)
    {
        for (size_t j = 0; j < trees[i].size(); ++j)
        {
            delete (trees[i][j]);
        }
    }
    trees.clear();
    return working;
}


bool test_ConsensusProblem() {

    // flag
    bool working = true;

    // fetch the data
    vector<vector<string> > data = read_data("../../../data/others/DM1-33.txt");
    vector<vector<Tree*> > trees = get_tree_lists(data);

    // create the problem instance
    ConsensusProblem<Tree*>* prob1 = new ConsensusProblem<Tree*>(trees, *unit_distance);
    ConsensusProblem<string>* prob2 = new ConsensusProblem<string>(data, *levenshtein);

    vector<vector<double> > mat = prob2->get_distance_matrix();
    vector<Range> ranges = prob2->get_ranges();

    // check for symmetry and assignation (not infinity)
    for(int i = 0; i < ranges[0].high; ++i)
    {
        for(int j = ranges[1].low; j != ranges[1].high; ++j)
        {
            if ( (mat[i][j] != mat[j][i]) || (mat[i][j] == std::numeric_limits<double>::infinity()) )
            {
                working = false;
                break;
            }
        }
    }

    // free the structures
    for(size_t i = 0; i < trees.size(); ++i)
    {
        for(size_t j = 0; j < trees[i].size(); ++j)
        {
            delete trees[i][j];
        }
    }
    delete prob1;
    delete prob2;

    return working;
}



void all_tests()
{
    std::cout << "Testing RNA Consensus" << std::endl;
    bool t1 = test_RNAConsensus();

    std::cout << "Testing Consensus Problem" << std::endl;
    bool t2 = test_ConsensusProblem();

    if ( t1 && t2 ) {
        cout << "Passed Tests" << endl;
    } else {
        cout << "Failed Tests" << endl;
    }
    return;
}

