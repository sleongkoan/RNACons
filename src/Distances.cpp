#include "../include/Distances.h"

#define MIN2(A,B)       ((A)<(B)?(A):(B))
#define MIN3(A,B,C)     (MIN2(MIN2((A),(B)),(C)))



// ===============================TREE DISTANCES===============================


void tree_distance_(OrderedLabeledTree tree1, OrderedLabeledTree tree2, // trees
                    int i, int j,                               // keyroots indices
                    std::vector< std::vector<int> >& treedists, // table to modify
                    int (*insertion_fun) (char),                // insertion
                    int (*deletion_fun) (char),                 // deletion
                    int (*substitution_fun)(char, char))        // substitution
{  // calculate the tree edit distances for two subtrees (helper)

    // =============================INITIALIZATION=============================
    std::vector<int> FirstLMDS =  tree1.get_leftmost_descendents();
    std::vector<int> SecondLMDS = tree2.get_leftmost_descendents();
    int p,q;

    int m = i - FirstLMDS[i]  + 2;
    int n = j - SecondLMDS[j] + 2;

    // create a m x n matrix of zeros
    std::vector< std::vector<int> > forest_distance = std::vector< std::vector<int> >(m);
    for(int x = 0; x != m; ++x)
    {
        forest_distance[x].resize(n, 0);
    }

    // figure out the offset
    int ioff = FirstLMDS[i] - 1;
    int joff = SecondLMDS[j] - 1;

    // some data we'll use along the way
    char label1, label2;
    int insertion_cost, deletion_cost, substitution_cost;


    // ====================DYNAMIC PROGRAMMING TABLE FILLING===================
    // fill deletions (tree1 row)
    for (int x = 1; x != m; ++x)
    {
        forest_distance[x][0] = forest_distance[x-1][0] + 1;
    }


    // fill insertions (tree1 column)
    for (int y = 1; y != n; ++y)
    {
        forest_distance[0][y] = forest_distance[0][y-1] + 1;
    }



    // fill the rest of the matrix
    for (int x = 1; x != m; ++x)
    {
        for (int y = 1; y != n; ++y)
        {
            // some situation independent data
            label1 = tree1.get_label(x+ioff);
            label2 = tree2.get_label(y+joff);

            deletion_cost =  deletion_fun(label1);
            insertion_cost = insertion_fun(label2);

            // case 1
            // x is an ancestor of i and y is an ancestor of j
            if ( (FirstLMDS[i]  == FirstLMDS[x+ioff]) &&
                 (SecondLMDS[j] == SecondLMDS[y+joff]) )
            {
                substitution_cost = substitution_fun(label1, label2);

                forest_distance[x][y] = MIN3(  // deletion
                                               (forest_distance[x-1][y] + deletion_cost),
                                               // insertion
                                               (forest_distance[x][y-1] + insertion_cost),
                                               // substitution
                                               (forest_distance[x-1][y-1] + substitution_cost)
                                            );

                treedists[x+ioff][y+joff] = forest_distance[x][y];
            }
            // case 2
            else
            {
                p = FirstLMDS[x+ioff] -1-ioff;
                q = SecondLMDS[y+joff]-1-joff;
                forest_distance[x][y] = MIN3(  // deletion
                                               (forest_distance[x-1][y] + deletion_cost),
                                               // insertion
                                               (forest_distance[x][y-1] + insertion_cost),
                                               // substitution
                                               (forest_distance[p][q] + treedists[x+ioff][y+joff])
                                            );
            }
        }
    }
    return;
}


std::vector< std::vector<int> > tree_edit_distance(OrderedLabeledTree tree1,
                                                   OrderedLabeledTree tree2,
                                                   int (*insertion_fun) (char),          // labeled node insertion function
                                                   int (*deletion_fun) (char),           // labeled node deletion function
                                                   int (*substitution_fun)(char, char))  // labeled nodes substitution function
{  // return the dynamic programming matrix for the tree edit distance between two whole trees

    // =============================INITIALIZATION=============================

    size_t sizetree1  =  tree1.get_all_labels().size();
    size_t sizetree2 = tree2.get_all_labels().size();

    // create the matrix holding tree distances
    std::vector< std::vector<int> > tree_distances = std::vector< std::vector<int> >(sizetree1);
    for(unsigned int x = 0; x !=sizetree1; ++x)
    {
        tree_distances[x].resize(sizetree2, 0);
    }


    // =======================TREE DISTANCES CALCULATIONS======================
    std::vector<int> keyroots1 = tree1.get_keyroots();
    std::vector<int> keyroots2 = tree2.get_keyroots();
    int keyroot1, keyroot2;
    for (size_t i = 0; i != keyroots1.size(); ++i)
    {
        keyroot1 = keyroots1[i];
        for (size_t j = 0; j != keyroots2.size(); ++j)
        {
            keyroot2 = keyroots2[j];
            tree_distance_(tree1, tree2,
                           keyroot1, keyroot2,
                           tree_distances,
                           insertion_fun,
                           deletion_fun,
                           substitution_fun);
        }
    }

    return tree_distances;
}



int unit_cost(char _)
{  // unit cost for deletion and insertion
    return 1;
}

int equality_cost(char a, char b)
{  // if both labels are equal, return 0, otherwise 2
    int val = 0;
    if (a != b)
    {
        val = 2;
    }
    return val;
}


int unit_tree_indel_distance_trees(const OrderedLabeledTree tree1,
                                   const OrderedLabeledTree tree2)
{
    return tree_edit_distance(tree1, tree2,
                              unit_cost, unit_cost, equality_cost).back().back();
}



// ============================STRING EDIT DISTANCE============================


int string_edit_distance(std::string tree1,
                         std::string tree2)
{
    // string edit distance
    int length1 = tree1.size();
    int length2 = tree2.size();
    std::vector< std::vector<int> > string_dist = std::vector< std::vector<int> >();

    for(int i = 0; i < length1 + 1; ++i)
    {
        string_dist.push_back(std::vector<int>(length2 + 1, 0));
    }


    // fill tree1 column
    for (int i = 0; i < length1 + 1; ++i)
    {
        string_dist[i][0] = i;
    }

    // fill tree2 row
    for (int j = 0; j < length2+1; ++j)
    {
        string_dist[0][j] = j;
    }

    // fill the whole table
    for (int i = 0; i < length1; ++i)
    {
        for (int j = 0; j < length2; ++j)
        {
            if (tree1[i] == tree2[j])
            {
                string_dist[i + 1][j + 1] = string_dist[i][j];
            }
            else
            {
                int minDist = string_dist[i][j + 1];
                int dist = string_dist[i + 1][j];
                if (dist < minDist)
                    minDist = dist;
                dist = string_dist[i][j];
                if (dist < minDist)
                    minDist = dist;

                string_dist[i + 1][j + 1] = minDist + 1;
            }
        }
    }
    return string_dist[length1][length2];
}

