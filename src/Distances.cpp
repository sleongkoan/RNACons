#include "../include/Distances.h"

#define MIN2(A,B)       ((A)<(B)?(A):(B))
#define MIN3(A,B,C)     (MIN2(MIN2((A),(B)),(C)))



// ===============================TREE DISTANCES===============================


void calculate_tree_distance(Tree first, Tree second,                    // trees
                             int i, int j,                               // keyroots indices
                             std::vector< std::vector<int> >& treedists, // tree distance table (to update)
                             int (*insertion_fun) (char),                // labeled node insertion function
                             int (*deletion_fun) (char),                 // labeled node deletion function
                             int (*substitution_fun)(char, char))        // labeled nodes substitution function
{  // calculate the tree edit distances for two subtrees
    // =============================INITIALIZATION=============================
    std::vector<int> FirstLMDS =  first.get_leftmost_descendents();
    std::vector<int> SecondLMDS = second.get_leftmost_descendents();
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
    // fill deletions (first row)
    for (int x = 1; x != m; ++x)
    {
        forest_distance[x][0] = forest_distance[x-1][0] + 1;
    }


    // fill insertions (first column)
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
            label1 = first.get_label(x+ioff);
            label2 = second.get_label(y+joff);

            deletion_cost =  deletion_cost(label1);
            insertion_cost = insertion_fun(label2);

            // case 1
            // x is an ancestor of i and y is an ancestor of j
            if ( (FirstLMDS[i]  == FirstLMDS[x+ioff]) &&
                 (SecondLMDS[j] == SecondLMDS[y+joff]) )
            {
                substitution_cost = label_distance(label1, label2);

                forest_distance[x][y] = MIN3( // deletion
                                              (forest_distance[x-1][y] + deletion_cost),
                                              // insertion
                                              (forest_distance[x][y-1] + insertion_cost),
                                              // substitution
                                              (forest_distance[x-1][y-1]+ substitution_cost)
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


std::vector< std::vector<int> > tree_edit_distance(const Tree first,
                                                   const Tree second,
                                                   int (*insertion_fun) (char),          // labeled node insertion function
                                                   int (*deletion_fun) (char),           // labeled node deletion function
                                                   int (*substitution_fun)(char, char))  // labeled nodes substitution function
{  // return the dynamic programming matrix for the tree edit distance between two whole trees

    // =============================INITIALIZATION=============================

    size_t sizefirst  =  first.get_all_labels().size();
    size_t sizesecond = second.get_all_labels().size();

    // create the matrix holding tree distances
    std::vector< std::vector<int> > tree_distances = std::vector< std::vector<int> >(sizefirst);
    for(unsigned int x = 0; x !=sizefirst; ++x)
    {
        tree_distances[x].resize(sizesecond, 0);
    }


    // =======================TREE DISTANCES CALCULATIONS======================
    std::vector<int> keyroots1 = first.get_keyroots();
    std::vector<int> keyroots2 = second.get_keyroots();
    int keyroot1, keyroot2;
    for (size_t i = 0; i != keyroots1.size(); ++i)
    {
        keyroot1 = keyroots1[i];
        for (size_t j = 0; j != keyroots2.size(); ++j)
        {
            keyroot2 = keyroots2[j];
            calculate_tree_distance(first, second,
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


int unit_tree_indel_distance_trees(const Tree first,
                                   const Tree second)
{
    return tree_edit_distance(first, second,
                              unit_cost, unit_cost, equality_cost).back().back();
}


int unit_tree_indel_distance_strings(const std::string first,
                                     const std::string second)
{
    // unit tree indel distance for dot brackets

    // convert both dot brackets to trees
    Tree first_tree = Tree(first);
    Tree second_tree = Tree(second);
    return unit_tree_indel_distance_trees(first_tree,
                                          second_tree);
}



// ============================STRING EDIT DISTANCE============================


int string_edit_distance(std::string first,
                         std::string second)
{
    // string edit distance
    int length1 = first.size();
    int length2 = second.size();
    std::vector< std::vector<int> > string_dist = std::vector< std::vector<int> >();

    for(int i = 0; i < length1 + 1; ++i)
    {
        string_dist.push_back(std::vector<int>(length2 + 1, 0));
    }


    // fill first column
    for (int i = 0; i < length1 + 1; ++i)
    {
        string_dist[i][0] = i;
    }

    // fill second row
    for (int j = 0; j < length2+1; ++j)
    {
        string_dist[0][j] = j;
    }

    // fill the whole table
    for (int i = 0; i < length1; ++i)
    {
        for (int j = 0; j < length2; ++j)
        {
            if (first[i] == second[j])
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

