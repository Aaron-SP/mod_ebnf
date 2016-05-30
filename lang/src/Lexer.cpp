#include "Lexer.h"
#include "Parse.h"

Lexer::Lexer(const std::string& src, const std::string& rules) : _rules(rules)
{
    _data = parse::read_file(src);
}

