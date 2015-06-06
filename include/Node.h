#ifndef NODE_H
#define NODE_H
#include <vector>
#include <cstddef>

class Node
{
public:
    // constructors
    Node(Node* parent=NULL, int label=-1);

    // destructor
    ~Node();

    // getters and setters
    void add_child(Node* other);
    std::vector<Node*>& get_children();
    Node* get_parent();
    void set_parent(Node* parent);
    int get_label();
    void set_label(int label);

    // helper
    int get_leftmost_descendent_label();

private:
    Node* parent;
    int label;
    std::vector<Node*> children;
};


// some useful functions

// node <-> bracket interconversion
void node_to_bracket(Node* node, std::vector<char>& brackets);

// helpers
std::vector<Node*> get_postorder_enumeration(Node* root);


#endif // NODE_H
