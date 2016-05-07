#ifndef  __LEXER__

#include <memory>
#include <string>
#include "Rules.h"

class Lexer
{
protected:
    std::vector<char> _data;
    Rules _rules;
public:
    Lexer(const std::string& src, const std::string& rules);
};

#endif