#include "../include/RNA.h"


bool is_valid_dot_bracket(std::string dot_bracket)
{
    // tests Vienna dot-bracket for illegal structure (or symbol)
    int counter = 0;

    for (unsigned int i = 0; i < dot_bracket.size(); ++i)
    {
        char c = dot_bracket[i];
        if (c == '(')               // stack
        {
            counter += 1;
        }
        else if (c == ')')          // unstack
        {
            counter -= 1;
        }
        else if (c != '.')          // illegal character
        {
            return false;
        }
        if (counter < 0)            // left unbalanced
        {
            return false;
        }
    }
    if (counter != 0)               // right unbalanced
    {
        return false;
    }
    else                            // correct dotbracket
    {
        return true;
    }
}


std::string only_paired(std::string dot_bracket)
{
    // removes the "." characters from the dot_bracket
    assert(is_valid_dot_bracket(dot_bracket));  // only apply on legal dot_bracket
    std::string ret = std::string(dot_bracket);
    ret.erase(std::remove(ret.begin(), ret.end(), '.'), ret.end());
    return ret;
}


int levenshtein(std::string a, std::string b)
{
    // string edit distance
    int aLength = a.size();
    int bLength = b.size();

    std::vector< std::vector<int> > levDistances = std::vector< std::vector<int> >();
    for(int i = 0; i < aLength+1; ++i)
    {
        levDistances.push_back(std::vector<int>(bLength+1, 0));
    }


    // fill first column
    for (int i = 0; i < aLength+1; ++i)
    {
        levDistances[i][0] = i;
    }

    // fill second column
    for (int j = 0; j < bLength+1; ++j)
    {
        levDistances[0][j] = j;
    }

    // fill the table now
    for (int i = 0; i < aLength; ++i)
    {
        for (int j = 0; j < bLength; ++j)
        {
            if (a[i] == b[j])
            {
                levDistances[i + 1][j + 1] = levDistances[i][j];
            }
            else
            {
                int minDist = levDistances[i][j + 1];
                int dist = levDistances[i + 1][j];
                if (dist < minDist)
                    minDist = dist;
                dist = levDistances[i][j];
                if (dist < minDist)
                    minDist = dist;

                levDistances[i + 1][j + 1] = minDist + 1;
            }
        }
    }
    return levDistances[aLength][bLength];
}


Node* dot_bracket_to_node(std::string dot_bracket)
{
    //creates a base pair tree from Vienna dot bracket
    assert(is_valid_dot_bracket(dot_bracket));
    Node* root = new Node();
    Node* position = root;
    int node_id = 0;
    for (int i = 0; dot_bracket[i] != '\0'; i++)
    {
        char c = dot_bracket[i];
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
    return root;
}
