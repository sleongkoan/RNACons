#include "../include/Node.h"
#include <iostream>

Node::Node()
{
    this->label = -1;
    this->children = std::vector<Node*>();
    this->parent = NULL;
}

Node::Node(Node* parent=NULL, int label=-1)
{
    // constructor
    if (parent)
    {
        parent->get_children().push_back(this);
    }
    this->label = label;
    this->children = std::vector<Node*>();
    this->parent = parent;
}

Node::~Node() { }

std::vector<Node*> & Node::get_children()
{
    return (this->children);
}

void Node::add_child(Node* child)
{
    // add the child to this children's list
    this->get_children().push_back(child);
    return;
}



Node* Node::get_parent()
{
    return this->parent;
}

void Node::set_parent(Node* new_parent)
{
    this->parent = new_parent;
    return;
}

int Node::get_label()
{
    return this->label;
}

void Node::set_label(int new_label)
{
    this->label = new_label;
    return;
}

int Node::get_leftmost_descendent_label()
{
    //std::cout << "got here" << std::endl;
    Node* position = this;
    //std::cout << "got here" << std::endl;
    while (position->get_children().size() > 0)
    {
        position = position->get_children()[0];
    }
    return position->get_label();
}


// ----------------------------------------------------------------------------


void node_to_bracket(Node* node, std::vector<char>& brackets)
{
    // tree structure -> (()()) representation
    brackets.push_back('(');
    std::vector<Node*>::iterator it;
    if (node)
    {
        for (it = node->get_children().begin(); it != node->get_children().end(); ++it)
            node_to_bracket(*it, brackets);
    }
    brackets.push_back(')');
    return;
}

void postorder_list_helper(Node* node, std::vector<Node*>& res)
{
    // helper for the postorder_list function
    std::vector<Node*>::iterator children;
    if (node)
    {
        for(children = node->get_children().begin(); children != node->get_children().end(); ++children)
        {
            postorder_list_helper(*children, res);
        }
        res.push_back(node);
    }
    return;
}
std::vector<Node*> get_postorder_enumeration(Node* root)
{
    std::vector<Node*> result = std::vector<Node*>();
    postorder_list_helper(root, result);
    return result;
}


