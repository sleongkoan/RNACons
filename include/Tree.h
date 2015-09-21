#ifndef TREE_H
#define TREE_H

#include <assert.h>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <map>
#include <string>
#include <vector>


// ====================================NODE====================================


class Node
{   // used for trees
public:
    // constructor
    Node(Node* parent, char label)
    {
        // constructor
        if (parent)
        {
            parent->add_child(this);
        }
        parent_ = parent;
        label_ = label;
        index_ = -1;
        children_ = std::vector<Node*>();
    };

    // destructor
    ~Node(){};

    // getters
    char get_label() const { return label_; }
    int get_index()  const { return index_; }
    Node* get_parent() { return parent_; }
    const std::vector<Node*> & get_children() { return children_; }

    // setters
    void set_parent(Node* parent) { parent_ = parent; }
    void set_label(char label) { label_ = label; }
    void set_index(int index)  { index_ = index; }

    // modifier
    void add_child(Node* child) { children_.push_back(child); }

    // helper
    int get_leftmost_descendent_index();

private:
    Node* parent_;
    char label_;
    int index_;
    std::vector<Node*> children_;
};


// ============================ORDERED LABELED TREE============================


class OrderedLabeledTree
{   // abstract base class for RNA trees

public:
    // ctor & dtor
    OrderedLabeledTree(std::string string_repr, // representation
                       char add_branch_symbol,  // + and down last added
                       char unbranch_symbol,    // up
                       char add_leaf_symbol);   // +
    ~OrderedLabeledTree();

    // accessors
    std::vector<int> &  get_leftmost_descendents() { return leftmost_descendents_; 
    std::vector<int> &  get_keyroots() { return keyroots_; }
    std::vector<char> & get_labels() { return postorder_labels_; }
    std::string get_string() { return string_repr_; }
    char get_add_branch_symbol() { return add_branch_symbol_; }
    char get_unbranch_symbol()  { return unbranch_symbol_; }
    char get_add_leaf_symbol() { return add_leaf_symbol_; }

        // friend functions
    friend bool operator==(const OrderedLabeledTree &tree1, const OrderedLabeledTree &tree2);
    friend bool operator!=(const OrderedLabeledTree &tree1, const OrderedLabeledTree &tree2);
    friend bool operator< (const OrderedLabeledTree &tree1, const OrderedLabeledTree &tree2);
    friend std::ostream& operator<< (std::ostream& out, const OrderedLabeledTree& tree);

protected:
    // tree distance members
    std::vector<char> postorder_labels_;
    std::vector<int> leftmost_descendents_;
    std::vector<int> keyroots_;
    std::string string_repr_;
    char add_branch_symbol_;
    char unbranch_symbol_;
    char add_leaf_symbol_;
};




#endif // TREE_H

