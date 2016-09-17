#ifndef  __LEXER__

#include <memory>
#include <string>
#include "Rules.h"

class Lexer
{
protected:
    Rules _rules;
public:
    Lexer(const std::string& rules);
    bool isValid(const std::string& src) const;
};

#endif