#include "lexer.h"

Lexer::Lexer(const std::string& src, const std::string& rules) : _rules(rules)
{
    _data = Rules::read_file(src);
}

