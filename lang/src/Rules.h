#ifndef __RULES__

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
    std::map<std::string, std::vector<std::string>> _tokenMap;
    std::string _root;
    void parseRules();
    std::string parseSymbols();
public:
    Rules(const std::string& filePath);
    Rules(const std::vector<char>& rules);
    std::string getRoot() const;
};

#endif