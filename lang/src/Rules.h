#ifndef __RULES__

#include "SyntaxNode.h"

#include <string>
#include <vector>
#include <map>

class Rules
{
protected:
    std::vector<char> _text;
    std::vector<std::string> _ruleList;
    std::map<std::string, SyntaxNode> _tokenMap;
    std::string _root;
    void parseRules();
    std::string parseSymbols();
    bool validate(const std::string& token, const std::string& input, size_t& position) const;
    bool matches(const SyntaxNode& node, const std::string& match, size_t& position) const;
public:
    Rules(const std::string& filePath);
    Rules(const std::vector<char>& rules);
    std::string getRoot() const { return _root; }
    bool validate(const std::string& token, const std::string& input) const;
};

#endif