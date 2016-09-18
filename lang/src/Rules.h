// Copyright 2016 <Aaron Springstroh>
#ifndef __RULES__

#include "SyntaxNode.h"
#include <map>
#include <string>
#include <vector>

class Rules
{
 protected:
    std::string _text;
    std::vector<std::string> _ruleList;
    std::map<std::string, SyntaxNode> _tokenMap;
    std::string _root;
 protected:
    void parse_rules();
    void parse_symbols();
    bool validate(const std::string& token, const std::string& input, size_t& position) const;
    bool matches(const SyntaxNode& node, const std::string& match, size_t& position) const;
 public:
    explicit Rules(const std::string& rules);
    const std::string& get_root() const { return _root; }
    bool validate(const std::string& token, const std::string& input) const;
};

#endif
