#ifndef SRC_CPP_TREE_H_
#define SRC_CPP_TREE_H_

#include <vector>
#include <string>
#include <cstddef>
#include <iostream>

#include <assert.h>

#include "Node.h"
#include "RNA.h"


class Tree
{
public:
    // constructor
    Tree(std::string brackets);
    Tree(const Tree &other);

    // destructor
    ~Tree();

    // getters
    std::string get_brackets() const;
    std::vector<int> get_leftmost_descendents() const;
    std::vector<int> get_keyroots() const;

    // comparison operators
    friend bool operator==(const Tree &tree1, const Tree &tree2);
    friend bool operator!=(const Tree &tree1, const Tree &tree2);
    friend bool operator<(const Tree &tree1, const Tree &tree2);

    // stream operator
    friend std::ostream& operator<< (std::ostream& out, Tree &tree);
    friend std::ostream& operator<< (std::ostream& out, Tree* tree);

private:
    std::string brackets;
    std::vector<int> leftmost_descendents;
    std::vector<int> keyroots;
};

/*
this distance is exactly the same as Zhang-Shasha
tree edit distance, but based on the special case of
unit cost for deletion and insertion and zero cost for
substitution

for RNA, this is equivalent to the number of base pairs
to create or break to convert tree1 into tree2
*/

int unit_distance(const Tree A, const Tree B);

#endif // SRC_CPP_TREE_H_
