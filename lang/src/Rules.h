#ifndef __RULES__

#include <string>
#include <vector>
#include <map>
#include "ParseTree.h"

class Token
{

};

class Rules
{
protected:
    std::vector<char> _text;
    std::vector<std::string> _ruleList;
    std::map<std::string, std::vector<SyntaxNode>> _tokenMap;
    ParseTree _tree;
    void parseRules();
    std::string parseSymbols();
public:
    Rules(const std::string& rules);
    static std::vector<char> read_file(const std::string& file);
    static std::vector<SyntaxNode> Rules::tokenize(const std::string& source);
    static std::string& strip_squotes(std::string& str);
    static std::string& strip_dquotes(std::string& str);
    static bool strip_quotes(std::string& str);
    static bool in_quotes(char ch, bool& quotes, char& quote_char);
    static bool in_brackets(const char ch, bool quotes, bool& in_brackets, char & bracket_char);
};

#endif