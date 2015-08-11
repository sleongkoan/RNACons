#include "../include/Tree.h"
#include <map>
#include <algorithm>


Node::Node(Node* parent, int label)
{
    // constructor
    if (parent)
    {
        parent->add_child(this);
    }
    this->label = label;
    this->children = std::vector<Node*>();
    this->parent = parent;
}

Node::~Node() { }

const std::vector<Node*> & Node::get_children()
{
    return this->children;
}

void Node::add_child(Node* child)
{
    // add the child to this children's list
    this->children.push_back(child);
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


void postorder_list_helper(Node* node, std::vector<Node*>& res)
{
    // helper for the postorder_list function
    if (node)
    {
        for(size_t i = 0; i != node->get_children().size(); ++i)
        {
            postorder_list_helper(node->get_children()[i], res);
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


Tree::Tree(std::string brackets)
{
    // all trees can be represented by a bracket (Vienna dot bracket minus the dots)
    // convert the bracket its corresponding tree structure
    if (! is_valid_dot_bracket(brackets))
    {
        std::cerr << "Badly formed dot bracket fed to tree constructor"
        << std::endl << brackets << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // create rooted tree
    Node* root = new Node();
    Node* position = root;
    int node_id = 0;
    for (int i = 0; brackets[i] != '\0'; i++)
    {
        char c = brackets[i];
        if (c == '(')
        {
            Node* child = new Node(position, node_id);
            node_id = node_id + 1;
            position = child;
        }
        else if (c == ')')
        {
            position = position->get_parent();
        }
        else
        {
            continue;
        }
    }

    // string representation
    this->brackets = only_paired(brackets);

    // postorder enumeration of the nodes and label them according to their order
    std::vector<Node*> nodes = get_postorder_enumeration(root);
    for (unsigned int post_order_index = 0; post_order_index < nodes.size(); ++post_order_index)
        nodes[post_order_index]->set_label(post_order_index);

    // calculate the leftmost descendents
    leftmost_descendents = std::vector<int>(nodes.size(), 0);
    for (unsigned int node_index=0; node_index <  nodes.size(); ++node_index)
    {
        leftmost_descendents[node_index] = nodes[node_index]->get_leftmost_descendent_label();
    }

    // calculate the keyroots based on the leftmost descendents
    this->keyroots = std::vector<int>();
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

std::vector<int> Tree::get_leftmost_descendents() const
{
    return this->leftmost_descendents;
}

std::vector<int> Tree::get_keyroots() const
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
