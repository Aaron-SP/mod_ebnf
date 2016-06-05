#ifndef __RULES__

#include "SyntaxNode.h"

#include <string>
#include <vector>
#include <map>

class Token
{

};

class Rules
{
protected:
    std::vector<char> _text;
    std::vector<std::string> _ruleList;
    std::map<std::string, SyntaxNode> _tokenMap;
    std::string _root;
    void parseRules();
    std::string parseSymbols();
public:
    Rules(const std::string& filePath);
    Rules(const std::vector<char>& rules);
    std::string getRoot() const;
    bool isValid(std::string token, std::string input);
};

#endif