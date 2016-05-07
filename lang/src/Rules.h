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
    void parseRules();
    void parseSymbols();
public:
    Rules(const std::string& rules);
    static std::vector<char> readFile(const std::string& file);
    static std::vector<std::string> Rules::tokenize(std::string source);
    static std::string& strip_quotes(std::string& str);
    static void in_quotes(char ch, bool& quotes, char& quote_char);
};

#endif
