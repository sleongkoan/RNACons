#include "../include/RNA2D.h"


std::string only_paired(std::string dot_bracket)
{
    // removes the "." characters from the dot_bracket
    assert(is_valid_dot_bracket(dot_bracket));  // only apply on legal dot_bracket
    std::string ret = std::string(dot_bracket);
    ret.erase(std::remove(ret.begin(), ret.end(), '.'), ret.end());
    return ret;
}


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


std::string shape_level_5(std::string dot_bracket){
    char db[dot_bracket.size()];
    int dblen=0;
    for( int i=0; i<dot_bracket.size(); ++i ){
        if( dot_bracket[i]!='.' )
        {
            char c = dot_bracket[i];
            c = c==')'?']':(c=='('?'[':c);
            db[dblen++]=c;
        }
    }

    //compute the idb (integer dot bracket) of the db
    int buddies[dblen];
    int stack[dblen];
    int sp=-1;
    int maxsp = -1; //needed to know if a db is only filled with '.'
    for( int i=0;i<dblen;++i ){
        if(db[i]=='['){
            stack[++sp]=i;
        } else if( db[i]==']' ){
            buddies[i]=stack[sp];
            buddies[stack[sp]]=i;
            --sp;
        }
        maxsp = maxsp>sp?maxsp:sp;
    }

    int slen = 1; //1 and not 0 because if db[0] != '.' then there is no room for the first (
    //1find length of shape
    for( int i=1; i<dblen; ++i ){
        if( ( buddies[i-1]-buddies[i]!=1 ) | ( db[i]!=db[i-1] ) ) ++slen;
    }
    // fill the shape
    std::vector<char> shape = std::vector<char>(slen+1);

    int next=0;
    if(maxsp>-1)shape[next++]='[';
    for( int i=1; i<dblen; ++i ){
        if( ( ( buddies[i-1]-buddies[i]!=1 ) | ( db[i]!=db[i-1] ) ) )
            shape[next++]=db[i];
    }
    std::string result = std::string(shape.begin(), shape.begin() + next); //shape[next]=0;
    return result;
}

