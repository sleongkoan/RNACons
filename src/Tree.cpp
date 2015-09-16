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


std::vector<int> OrderedLabeledTree::get_leftmost_descendents() const
{
    return leftmost_descendents_;
}


std::vector<int> OrderedLabeledTree::get_keyroots() const
{
    return keyroots_;
}


char OrderedLabeledTree::get_label(int index) const
{
    return node_labels_[index];
}


std::vector<char> get_all_labels() const
{  // returns the vector of all labels (postorder)
    return node_labels_;
}


bool operator==(const OrderedLabeledTree &first, const OrderedLabeledTree &second)
{
    return first.equality_predicate(second);
}


bool operator!=(const OrderedLabeledTree &first, const OrderedLabeledTree &second)
{
    return !(first.equality_predicate(second);
}


bool operator<(const OrderedLabeledTree &first, const OrderedLabeledTree &second)
{
    return first.less_than_predicate(second);

}


std::ostream& operator<< (std::ostream &out, OrderedLabeledTree tree)
{
    out << tree.to_string() << std::endl;
    return out;
}


// =================================RNA TREE===================================
RNATree::RNATree(std::string string_repr,
                 char opening_symbol,
                 char closing_symbol,
                 char unpaired_symbol)
{
    // 1- assign the string representation
    string_repr_ = string_repr;

    // 2- create rooted tree
    Node root = Node(NULL, opening_symbol);
    Node* position = root;
    char c;
    for (size_t i = 0; i != string_repr.size(); i++)
    {
        c = string_repr[i];
        if (c == opening_symbol)       // create new node and position goes down
        {
            Node child = Node(position, opening_symbol);
            position = &(child);
        }
        else if (c == closing_symbol)  // position goes up
        {
            position = position->get_parent();
        }
        else if (c == unpaired_symbol) // add unpaired node, position stays same
        {
            Node child = Node(position, unpaired_symbol);
        }
        else
        {
            continue;
        }
    }

    // 3- get postorder pointers
    std::vector<Node*> postorder_nodes = get_postorder_enumeration(root);

    // 4- set the index and fetch labels
    labels_ = std::vector<char>();
    for (siz_t index = 0; index != postorder_nodes.size(); ++index)
    {
        postorder_nodes[index]->set_index(index);
        labels_.push_back(postorder_nodes[index]->get_label());
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
    for(size_t index = postorder_nodes.size()-1; node_index != -1; --node_index)
    {
        lmd = leftmost_descendents_[node_index];

        if (!(keyroots_map.count(lmd)))
        {
            keyroots_map[lmd] = index;
        }
    }
    for (std::map<int, int>::iterator it = keyroots_map.begin(); it != keyroots_map.end(); ++it)
    {
        keyroots_.push_back(it->second);
    }
    std::sort(keyroots_.begin(), keyroots_.end());

    return;
}


std::string RNATree::to_string() const
{
    return string_repr_;
}


bool RNATree::equality_predicate(RNATree other)
{
    return to_string() == other.to_string();
}


bool RNATree::less_than_predicate(RNATree other)
{
    return to_string() < other.to_string();
}

