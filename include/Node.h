#ifndef NODE_H
#define NODE_H
#include <vector>
#include <cstddef>

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


// node -> bracket reduction
void node_to_bracket(Node* node, std::vector<char>& brackets);

// computes the postorder enumeration of the nodes
std::vector<Node*> get_postorder_enumeration(Node* root);


#endif // NODE_H
