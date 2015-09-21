#include "../include/Tree.h"



// ==================================NODE======================================


int Node::get_leftmost_descendent_index()
{ // TODO: there is a better way to find them, but this works ok for now
    Node* position = this;
    while (position->get_children().size() > 0)
    {
        position = position->get_children()[0];
    }
    return position->get_index();
}



// =========================ORDERED LABELED TREE===============================


void postorder_helper(Node* node, std::vector<Node*>& res)
{  // recursive helper to get the postorder enumeration
    if (node)
    {
        for(size_t i = 0; i != node->get_children().size(); ++i)
        {
            postorder_helper(node->get_children()[i], res);
        }
        res.push_back(node);
    }
    return;
}


std::vector<Node*> get_postorder_enumeration(Node* root)
{  // get list of node pointers to the postorder enumeration
    std::vector<Node*> postorder_enum = std::vector<Node*>();
    postorder_helper(root, postorder_enum);
    return postorder_enum;
}


OrderedLabeledTree::OrderedLabeledTree(std::string string_repr, // representation
                                       char add_branch_symbol,  // + and down last added
                                       char unbranch_symbol,    // up
                                       char add_leaf_symbol)    // +
{   // main constructor
    // 1- assign the string representation and remember the symbols
    string_repr_ = string_repr;
    add_branch_symbol_ = add_branch_symbol;
    unbranch_symbol_  = unbranch_symbol;
    add_leaf_symbol_ = add_leaf_symbol;

    // 2- create rooted tree
    Node root = Node(NULL, add_branch_symbol);
    Node* position = &root;
    char c;
    for (size_t i = 0; i != string_repr.size(); i++)
    {
        c = string_repr[i];
        if (c == add_branch_symbol)       // create new node and position goes down
        {
            Node child = Node(position, add_branch_symbol);
            position = &(child);
        }
        else if (c == unbranch_symbol)  // position goes up
        {
            position = position->get_parent();
        }
        else if (c == add_leaf_symbol) // add unpaired node, position stays same
        {
            Node child = Node(position, add_leaf_symbol);
        }
        else
        {
            exit(EXIT_FAILURE); // read unknown symbol
        }
    }

    // 3- get postorder pointers
    std::vector<Node*> postorder_nodes = get_postorder_enumeration(&root);

    // 4- set the index and fetch labels
    node_labels_ = std::vector<char>();
    for (size_t index = 0; index != postorder_nodes.size(); ++index)
    {
        postorder_nodes[index]->set_index(index);
        node_labels_.push_back(postorder_nodes[index]->get_label());
    }

    // 5- fetch the leftmost descendents
    leftmost_descendents_ = std::vector<int>(postorder_nodes.size(), 0);
    for (size_t index = 0; index != postorder_nodes.size(); ++index)
    {
        leftmost_descendents_[index] = postorder_nodes[index]->get_leftmost_descendent_index();
    }

    // 6- fetch the keyroots
    keyroots_ = std::vector<int>();
    std::map<int, int> keyroots_map;
    int lmd;
    for(int node_index = postorder_nodes.size()-1; node_index != -1; --node_index)
    {
        lmd = leftmost_descendents_[node_index];

        if (!(keyroots_map.count(lmd)))
        {
            keyroots_map[lmd] = node_index;
        }
    }

    for (std::map<int, int>::iterator it = keyroots_map.begin(); it != keyroots_map.end(); ++it)
    {
        keyroots_.push_back(it->second);
    }
    std::sort(keyroots_.begin(), keyroots_.end());

    return;
}



bool operator==(const OrderedLabeledTree &tree1, const OrderedLabeledTree &tree2)
{   // only equality on representation and instance...
    if ((tree1.add_branch_symbol_ == tree2.add_branch_symbol_) &&
        (tree1.unbranch_symbol_ == tree2.unbranch_symbol_) &&
        (tree1.add_leaf_symbol_ == tree2.add_leaf_symbol_) )
    {
        return (tree1.string_repr_ == tree2.string_repr_);
    }
    else
    {
        return false;
    }
}


bool operator!=(const OrderedLabeledTree &tree1, const OrderedLabeledTree &tree2)
{
    return !(tree1 == tree2);
}


bool operator<(const OrderedLabeledTree &tree1, const OrderedLabeledTree &tree2)
{
    assert((tree1.add_branch_symbol_ == tree2.add_branch_symbol_) &&
           (tree1.unbranch_symbol_ == tree2.unbranch_symbol_) &&
           (tree1.add_leaf_symbol_ == tree2.add_leaf_symbol_) );
    return (tree1.string_repr_ < tree2.string_repr_);
}


std::ostream& operator<< (std::ostream &out, OrderedLabeledTree tree)
{
    out << tree.string_repr_ << std::endl;
    return out;
}






OrderedLabeledTree::OrderedLabeledTree(const OrderedLabeledTree &other)
{   // copy constructor

    // ordered tree fields
    nodes_labels_ = other.get_all_labels();
    leftmost_descendents_ = other.get_leftmost_descendents();
    keyroots_ = other.get_keyroots();

    // RNA tree fields
    string_repr_ = other.to_string();
    add_branch_symbol_ = other.get_add_branch_symbol();
    unbranch_symbol_  = other.get_unbranch_symbol() ;
    add_leaf_symbol_ = other.get_add_leaf_symbol();
    return;
}


std::string OrderedLabeledTree::to_string() const
{
    return string_repr_;
}


bool OrderedLabeledTree::equality_predicate(OrderedLabeledTree other) const
{
    return to_string() == other.to_string();
}


bool OrderedLabeledTree::less_than_predicate(OrderedLabeledTree other) const
{
    return to_string() < other.to_string();
}

