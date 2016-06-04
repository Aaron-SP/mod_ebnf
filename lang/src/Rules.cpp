#include "Rules.h"
#include "Parse.h"
#include "SyntaxNode.h"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <set>

Rules::Rules(const std::string& filePath)
{
    _text = parse::read_file(filePath);
    parseRules();
    _root = parseSymbols();
}

Rules::Rules(const std::vector<char>& rules) : _text(rules)
{
    parseRules();
    _root = parseSymbols();
}

std::string Rules::getRoot() const
{
    return _root;
}

void Rules::parseRules()
{
    // white space list
    std::vector<char> ws{ 9, 10, 13, 32 };
    std::string rule = std::string();
    char quote_char = 0;
    bool quotes = false;

    const size_t size = _text.size();
    for (size_t i = 0; i < size; i++)
    {
        const char ch = _text[i];

        // If whitespace then ignore
        if (std::find(ws.begin(), ws.end(), ch) == ws.end())
        {
            parse::in_quotes(ch, quotes, quote_char);
            if (ch == ';' && !quotes)
            {
                // push_back string to list and reset the current rule
                _ruleList.push_back(rule);
                rule = std::string();
                continue;
            }
            rule += ch;
        }
    }
    // If we have quotes turned on then it is a parse error
    if (quotes)
    {
        throw std::runtime_error("Quotation '/\" mismatch. Check rules input file");
    }
    if (_ruleList.size() == 0)
    {
        throw std::runtime_error("Could not find any rules, did you miss a semi-colon (;)?");
    }
}

std::string Rules::parseSymbols()
{
    // Set will store all rhs symbols to determine the start symbol
    std::set<std::string> set;
    for (const auto& rule : _ruleList)
    {
        size_t found = rule.find('=');
        if (found != std::string::npos)
        {
            // lhs
            std::string token = rule.substr(0, found);
            // rhs
            std::string equality = rule.substr(found + 1, rule.size() - found - 1);
            // process rhs
            SyntaxNode node = SyntaxNode::tokenize(token, equality);
            std::vector<std::string> symbols = node.toVector();
            // Get the SyntaxNode's symbol and put them in the set
            std::copy(symbols.begin(), symbols.end(), std::inserter(set, set.end()));
            // map lhs to rhs for processing
            _tokenMap.insert({ token, symbols });
        }
        else
        {
            throw std::runtime_error("No equality found in rule");
        }
    }

    int count = 0;
    std::string root;
    // only one lhs should have no rhs definition
    for (const auto& x : _tokenMap)
    {
        if (set.count(x.first) == 0)
        {
            root = x.first;
            count++;
        }
    }

    if (count > 1)
    {
        throw std::runtime_error("Multiple root symbols found");
    }
    else if (count == 0)
    {
        throw std::runtime_error("No root symbol found");
    }
    return root;
}