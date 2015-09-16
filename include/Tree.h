#ifndef TREE_H
#define TREE_H

#include <assert.h>

#include <algorithm>
#include <cstddef>
#include <map>
#include <string>
#include <vector>


// ====================================NODE====================================


class Node
{  // used for trees
public:
    // constructor
    Node(Node* parent=NULL, char label)
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
{  // abstract base class for RNA trees
public:
    // ctor & dtor
    OrderedLabeledTree();
    OrderedLabeledTree(const OrderedLabeledTree &other);
    ~OrderedLabeledTree();

    // getters
    std::vector<int> get_leftmost_descendents() const;
    std::vector<int> get_keyroots() const;
    char get_label(int index) const;
    std::vector<char> get_all_labels() const;

    // friend functions
    friend bool operator==(const OrderedLabeledTree &first, const OrderedLabeledTree &second);
    friend bool operator!=(const OrderedLabeledTree &first, const OrderedLabeledTree &second);
    friend bool operator< (const OrderedLabeledTree &first, const OrderedLabeledTree &second);
    friend std::ostream& operator<< (std::ostream& out, OrderedLabeledTree tree);

    // virtual member methods related to friend functions
    virtual std::string to_string()=0;
    virtual bool equality_predicate(Tree other)=0;
    virtual bool less_than_predicate(Tree other)=0;


private:
    // constructor fields
    std::string string_repr_;
    char opening_symbol_;
    char closing_symbol_;
    char unpaired_symbol_;

    // tree distance members
    std::vector <char> node_labels_;  // label comparison
    std::vector<int> leftmost_descendents_;
    std::vector<int> keyroots_;
};



// ==================================RNA TREE==================================


class RNATree: public OrderedLabeledTree
{ // abstract shape and full 2D structure
public:
    // constructors
    RNATree(std::string string_repr,
            char opening_symbol,
            char closing_symbol,
            char unpaired_symbol);
    RNATree(RNATree& other);
    ~RNATree(){};

    // friend methods
    bool equality_predicate(RNATree other);
    bool less_than_predicate(RNATree other);

    std::string to_string() const;

private:
    std::string string_repr_;
};


#endif // TREE_H

