#include "../include/Distances.h"

#define MIN2(A,B)       ((A)<(B)?(A):(B))
#define MIN3(A,B,C)     (MIN2(MIN2((A),(B)),(C)))


void calculate_tree_distance(Tree A, Tree B, int i, int j,
                             std::vector< std::vector<int> >& treedists)
{
    std::vector<int> Al = A.get_leftmost_descendents();
    std::vector<int> Bl = B.get_leftmost_descendents();
    int p,q;

    int m = i - Al[i] + 2;
    int n = j - Bl[j] + 2;

    // create a m x n matrix of zeros
    std::vector< std::vector<int> > forest_distance = std::vector< std::vector<int> >(m);
    for(int x = 0; x != m; ++x)
        forest_distance[x].resize(n, 0);

    int ioff = Al[i] - 1;
    int joff = Bl[j] - 1;

    // fill deletions (first row)
    for (int x = 1; x != m; ++x)
        forest_distance[x][0] = forest_distance[x-1][0] + 1;

    // fill insertions (first column)
    for (int y = 1; y != n; ++y)
        forest_distance[0][y] = forest_distance[0][y-1] + 1;
    // fill the matrix
    for (int x = 1; x != m; ++x)
    {
        for (int y = 1; y != n; ++y)
        {
            // case 1
            // x is an ancestor of i and y is an ancestor of j
            if ( (Al[i] == Al[x+ioff]) && (Bl[j] == Bl[y+joff]) )
            {
                forest_distance[x][y] = MIN3( (forest_distance[x-1][y] + 1),  // deletion
                                              (forest_distance[x][y-1] + 1),  // insertion
                                              (forest_distance[x-1][y-1]) );  // substitution

                treedists[x+ioff][y+joff] = forest_distance[x][y];
            }
            // case 2
            else
            {
                p = Al[x+ioff]-1-ioff;
                q = Bl[y+joff]-1-joff;
                forest_distance[x][y] = MIN3((forest_distance[x-1][y] + 1),  // deletion
                                             (forest_distance[x][y-1] + 1),  // insertion
                                             (forest_distance[p][q] + treedists[x+ioff][y+joff]));  // substitution
            }
        }
    }
    return;
}


int unit_tree_indel_distance_trees(const Tree first,
                                   const Tree second)
{
    // unlabeled tree indel distance
    // create and fill the vector that will hold the tree distances computed
    size_t sizefirst = (first.get_brackets().size() / 2) + 1; // + 1 for artificial root
    size_t sizesecond = (second.get_brackets().size() / 2) + 1;

    // create the matrix holding tree distances, first x second
    std::vector< std::vector<int> > tree_distances = std::vector< std::vector<int> >(sizefirst);
    for(unsigned int x = 0; x !=sizefirst; ++x)
    {
        tree_distances[x].resize(sizesecond, 0);
    }

    std::vector<int>::iterator index1;
    std::vector<int>::iterator index2;
    std::vector<int> keyrootsfirst = first.get_keyroots();
    std::vector<int> keyrootssecond = second.get_keyroots();
    for (index1 = keyrootsfirst.begin(); index1 != keyrootsfirst.end(); ++index1)
    {
        for(index2 = keyrootssecond.begin(); index2 != keyrootssecond.end(); ++index2)
        {
            calculate_tree_distance(first, second, *index1, *index2, tree_distances);
        }
    }

    return tree_distances[tree_distances.size()-1][tree_distances[0].size()-1];
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
