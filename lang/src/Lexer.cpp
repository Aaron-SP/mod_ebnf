// Copyright 2016 <Aaron Springstroh>
#include "Lexer.h"
#include "Parse.h"
#include <string>

Lexer::Lexer(const std::string& rules) : _rules(rules){}

bool Lexer::is_valid(const std::string& src) const
{
    return _rules.validate(_rules.get_root(), src);
}
