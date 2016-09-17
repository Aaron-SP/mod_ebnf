// Copyright 2016 <Aaron Springstroh>
#include "Lexer.h"
#include "Parse.h"
#include <string>

Lexer::Lexer(const std::string& rules) : _rules(rules){}

bool Lexer::isValid(const std::string& src) const
{
    return _rules.validate(_rules.getRoot(), src);
}
