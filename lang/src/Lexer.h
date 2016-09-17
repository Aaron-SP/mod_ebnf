// Copyright 2016 <Aaron Springstroh>
#ifndef  __LEXER__

#include "Rules.h"
#include <string>

class Lexer
{
 protected:
    Rules _rules;
 public:
    explicit Lexer(const std::string& rules);
    bool isValid(const std::string& src) const;
};

#endif
