#include "../include/Tree.h"
#include <map>
#include <algorithm>

#define MIN2(A,B)       ((A)<(B)?(A):(B))
#define MIN3(A,B,C)     (MIN2(MIN2((A),(B)),(C)))


using std::vector;


Tree::Tree(std::string brackets)
{
    // all trees can be represented by a bracket (Vienna dotbracket minus the dots)
    // convert the bracket its corresponding tree structure
    assert(is_valid_dot_bracket(brackets));
    Node* root = dot_bracket_to_node(brackets);

    // string representation
    this->brackets = only_paired(brackets);

    // postorder enumeration of the nodes and label them according to their order
    vector<Node*> nodes = get_postorder_enumeration(root);
    for (unsigned int post_order_index = 0; post_order_index < nodes.size(); ++post_order_index)
        nodes[post_order_index]->set_label(post_order_index);

    // calculate the leftmost descendents
    leftmost_descendents = vector<int>(nodes.size(), 0);
    for (unsigned int node_index=0; node_index <  nodes.size(); ++node_index)
    {
        leftmost_descendents[node_index] = nodes[node_index]->get_leftmost_descendent_label();
    }

    // calculate the keyroots based on the leftmost descendents
    this->keyroots = vector<int>();
    std::map<int, int> keyroots_map;
    int leftmost_descendent_label;
    for(int node_index = nodes.size()-1; node_index > -1; --node_index)
    {
        leftmost_descendent_label= leftmost_descendents[node_index];

        if (!(keyroots_map.count(leftmost_descendent_label)))
        {
            keyroots_map[leftmost_descendent_label] = node_index;
        }
    }
    for (std::map<int, int>::iterator it = keyroots_map.begin(); it != keyroots_map.end(); ++it)
    {
        this->keyroots.push_back((*it).second);
    }

    std::sort(this->keyroots.begin(), this->keyroots.end());

    // free the nodes
    for (std::vector<Node*>::iterator it = nodes.begin() ; it != nodes.end(); ++it)
    {
        delete (*it);
    }
    nodes.clear();

    return;
}


Tree::Tree(const Tree &other)
{
    // copy constructor
    brackets = std::string(other.brackets);
    leftmost_descendents = other.leftmost_descendents;
    keyroots = other.keyroots;
}


Tree::~Tree() { }


std::string Tree::get_brackets() const
{
    return this->brackets;
}


// getters

vector<int> Tree::get_leftmost_descendents() const
{
    return this->leftmost_descendents;
}

vector<int> Tree::get_keyroots() const
{
    return this->keyroots;
}



// comparison operators

bool operator==(const Tree &tree1, const Tree &tree2)
{
    std::string a = tree1.get_brackets();
    std::string b = tree2.get_brackets();
    return (a == b);
}


bool operator!=(const Tree &tree1, const Tree &tree2)
{
    return !(tree1 == tree2);
}

bool operator<(const Tree &tree1, const Tree &tree2)
{
    return tree1.get_brackets() < tree2.get_brackets();

}



// stream operators

std::ostream& operator<< (std::ostream &out, Tree &tree)
{
    out << tree.brackets << std::endl;
    return out;
}


std::ostream& operator<< (std::ostream &out, Tree* tree)
{
    out << *tree;
    return out;
}


void calculate_tree_distance(Tree A, Tree B, int i, int j, vector< vector<int> >& treedists)
{
    vector<int> Al = A.get_leftmost_descendents();
    vector<int> Bl = B.get_leftmost_descendents();
    int p,q;

    int m = i - Al[i] + 2;
    int n = j - Bl[j] + 2;

    // create a m x n matrix of zeros
    vector< vector<int> > forest_distance = vector< vector<int> >(m);
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
                                              (forest_distance[x-1][y-1]) );    // substitution

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

}


int unit_distance(const Tree A, const Tree B)
{
    // unlabeled tree distance
    // create and fill the vector that will hold the tree distances computed
    size_t sizeA = (A.get_brackets().size() / 2) + 1; // + 1 for artificial root
    size_t sizeB = (B.get_brackets().size() / 2) + 1;

    // create the matrix holding tree distances, A x B
    vector< vector<int> > tree_distances = vector< vector<int> >(sizeA);
    for(unsigned int x = 0; x !=sizeA; ++x)
    {
        tree_distances[x].resize(sizeB, 0);
    }

    vector<int>::iterator index1;
    vector<int>::iterator index2;
    vector<int> keyrootsA = A.get_keyroots();
    vector<int> keyrootsB = B.get_keyroots();
    for (index1 = keyrootsA.begin(); index1 != keyrootsA.end(); ++index1)
    {
        for(index2 = keyrootsB.begin(); index2 != keyrootsB.end(); ++index2)
        {
            calculate_tree_distance(A, B, *index1, *index2, tree_distances);
        }
    }
    // TODO check that this arithmetic works properly
    return tree_distances[tree_distances.size()-1][tree_distances[0].size()-1];
}


