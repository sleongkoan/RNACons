#include "../include/RNAshapes.h"


// RNAshapes related code (level 1, 3 and 5)


class Node{
public:
    Node(char label, Node* parent)
    {
        parent_ = parent;
        label_ = label;
        children_ = std::vector<Node>();
    }
        ~Node(){};

        Node* parent_;
        char label_;
        std::vector<Node> children_;
};




std::vector<Node> dot_bracket_to_tree(std::string dot_bracket)
{ // transform a dotbracket into a tree structure of P and U nodes
    Node root = Node('P', NULL);
    Node* position = &root;
    char c;
    for (size_t i = 0; i != dot_bracket.size(); ++i)
    {
        c = dot_bracket[i];
        if (c == '.')       // unpaired
        {
            position->children_.push_back(Node('U', position));
        }
        else if (c == '(')  // paired
        {
            position->children_.push_back(Node('P', position));
            position = &(position->children_.back());
        }
        else
        {
            position = position->parent_;
        }
    }
    return root.children_;
}


void print_helper(Node* position, std::vector<char>& symbol_list,
                  char open_symbol, char close_symbol, char unpaired_symbol)
{ //
    if (position->label_ == 'P')
    {
        symbol_list.push_back(open_symbol);
        for (size_t i = 0; i != position->children_.size(); ++i)
        {
            print_helper(&(position->children_[i]), symbol_list, open_symbol, close_symbol, unpaired_symbol);
        }
        symbol_list.push_back(close_symbol);
    }
    else if (position->label_ =='U')
    {
        symbol_list.push_back(unpaired_symbol);
    }
    return;
}


std::string print_tree(std::vector<Node> trees,
                       char open_symbol='(',
                       char close_symbol=')',
                       char unpaired_symbol='.')
{ //
    std::vector<char> chars = std::vector<char>();
    for (size_t i = 0; i != trees.size(); ++i)
    {
        print_helper(&trees[i], chars, open_symbol, close_symbol, unpaired_symbol);
    }
    return std::string(chars.begin(), chars.end());
}



bool remove_stem(Node* node)
{   // to be used in a BFS traversal only
    if ( (node->label_ == 'P') && (node->children_.size() == 1) )
    {
        node->children_ = node->children_[0].children_;
        return true;
    }
    else
    {
        return false;
    }
}


void remove_stems(Node* tree)
{
    std::deque<Node*> Q = std::deque<Node*>();
    Q.push_back(tree);
    Node* current_node;

    bool modified;
    while(!Q.empty())
    {
        // dequeue
        current_node = Q.front();
        Q.pop_front();

        // apply stem removing operation and check status
        modified = remove_stem(current_node);
        if (modified)
        {
            // requeue immediately
            Q.push_front(current_node);
        }
        else if (current_node->label_ == 'P')
        {
            for (size_t i = 0; i != current_node->children_.size(); ++i)
            {
                Q.push_back(&(current_node->children_[i]));
            }
        }
    }
    return;
}


std::string preprocess(std::string dot_bracket)
{
    // ... -> .
    std::vector<char> step1 = std::vector<char>();
    char currentChar;
    char lastChar = 'a';
    for (size_t i = 0; i != dot_bracket.size(); ++i)
    {
        currentChar = dot_bracket[i];
        if ( (currentChar =='.') && (lastChar == currentChar))
        {
            // don't add
            continue;
        }
        else
        {
            step1.push_back(currentChar);
        }
        lastChar = currentChar;
    }

    // (.) -> ()
    std::vector<char> step2 = std::vector<char>();
    step2.push_back(step1[0]);

    for(size_t i = 1; i != step1.size()-1; ++i)
    {
        if ( (step2.back() == '(') && (step1[i] == '.') && (step1[i+1] == ')') )
        {
            continue;
        }
        else
        {
            step2.push_back(step1[i]);
        }
    }
    step2.push_back(step1.back());

    return std::string(step2.begin(), step2.end());
}


std::string RNAshapes(std::string dot_bracket, int level)
{
    assert(level == 1 || level == 3 || level==5);

    // preprocess the dotbracket and convert it to tree representation
    std::vector<Node> trees = dot_bracket_to_tree(preprocess(dot_bracket));



    //-------------------------------------------level 1
    // must remove nodes with single children (stems without branching)
    for (size_t i = 0; i != trees.size(); ++i)
    {
        remove_stems(&(trees[i]));
    }
    std::string level1_str = print_tree(trees, '[', ']', '_');
    if (level == 1)
    {
        return level1_str;
    }


    //-------------------------------------------level 3
    // we use string replacement here, its easier
    std::vector<char> level3 = std::vector<char>();
    for (size_t i = 0; i != level1_str.size(); ++i)
    {
        if (level1_str[i] != '_')
        {
            level3.push_back(level1_str[i]);
        }
    }
    std::string level3_str = std::string(level3.begin(), level3.end());
    if (level == 3)
    {
        return level3_str;
    }

    //-------------------------------------------level 5
    // same as for level 1 except it is applied on the level 3 string
    std::vector<char> level5 = std::vector<char>();
    for(size_t i = 0; i != level3_str.size(); ++i)
    {
        if (level3_str[i] == '[')
        {
            level5.push_back('(');
        }
        else if (level3_str[i] == ']')
        {
            level5.push_back(')');
        }
    }
    std::vector<Node> trees2 = dot_bracket_to_tree(std::string(level5.begin(), level5.end()));
    for (size_t i = 0; i != trees2.size(); ++i)
    {
        remove_stems(&(trees2[i]));
    }
    return print_tree(trees2, '[', ']', '.');
}