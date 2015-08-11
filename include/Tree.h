#ifndef TREE_H
#define TREE_H

#include <vector>
#include <string>
#include <cstddef>
#include <iostream>

#include <assert.h>
#include "RNA2D.h"

class Node
{
public:
    // constructor
    Node(Node* parent=NULL, int label=-1);

    // destructor
    ~Node();

    // getters and setters
    Node* get_parent();
    int get_label();
    const std::vector<Node*> & get_children();

    void set_parent(Node* parent);
    void set_label(int label);
    void add_child(Node* other);

    // helper
    int get_leftmost_descendent_label();

private:
    Node* parent;
    int label;
    std::vector<Node*> children;
};


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


// computes the postorder enumeration of the nodes
std::vector<Node*> get_postorder_enumeration(Node* root);


// used to assert that dot bracket is balanced
// and contains no illegal symbols
bool is_valid_dot_bracket(std::string dot_bracket);


// (((..).))) -> ((()))
std::string only_paired(std::string dot_bracket);


// (())..() -> [][]
std::string db2shape(std::string dotbracket);


// converts dot bracket to Nodes, used by Tree constructor
// don't forget to free it
// Node* dot_bracket_to_node(std::string  dot_bracket);



#endif // TREE_H
