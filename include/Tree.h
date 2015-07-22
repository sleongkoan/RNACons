#ifndef SRC_CPP_TREE_H_
#define SRC_CPP_TREE_H_

#include <vector>
#include <string>
#include <cstddef>
#include <iostream>

#include <assert.h>

#include "Node.h"


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


// used to assert that dot bracket is balanced
// and contains no illegal symbols
bool is_valid_dot_bracket(std::string dot_bracket);


// (((..).))) -> ((()))
std::string only_paired(std::string dot_bracket);

// (())..() -> [][]
std::string db2shape(std::string dotbracket);

// converts dot bracket to Nodes, used by Tree constructor
// don't forget to free it
Node* dot_bracket_to_node(std::string  dot_bracket);

int unit_distance(const Tree A, const Tree B);

#endif // SRC_CPP_TREE_H_
