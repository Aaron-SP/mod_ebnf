#include "Lexer.h"
#include "Parse.h"

Lexer::Lexer(const std::string& rules) : _rules(rules){}

bool Lexer::isValid(const std::string& src) const
{
    return _rules.validate(_rules.getRoot(), src);
}