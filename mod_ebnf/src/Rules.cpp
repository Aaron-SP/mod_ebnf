// Copyright 2016 <Aaron Springstroh>
#include "Rules.h"
#include "Parse.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <set>
#include <utility>
#include <vector>

const int MAGIC_ALLOCATION_SIZE = 128;

Rules::Rules(const std::string& rules) : _text(rules)
{
    parse_rules();
    parse_symbols();
}

void Rules::parse_rules()
{
    // white space list
    std::vector<char> ws{ 9, 10, 13, 32 };
    std::string rule;
    rule.reserve(MAGIC_ALLOCATION_SIZE);
    char quote_char = 0;
    bool quotes = false;

    // Remove spaces by copying into new buffer
    for (const char ch : _text)
    {
        // If whitespace then ignore
        if (std::find(ws.begin(), ws.end(), ch) == ws.end())
        {
            parse::in_quotes(ch, quotes, quote_char);
            if (ch == ';' && !quotes)
            {
                // push_back string to list and reset the current rule
                _ruleList.push_back(rule);
                rule.clear();
                continue;
            }
            rule += ch;
        }
    }
    // If we have quotes turned on then it is a parse error
    if (quotes)
    {
        throw std::runtime_error("End of file found before closing quotation");
    }
    if (_ruleList.size() == 0)
    {
        throw std::runtime_error("Could not find any rules, did you miss a semi-colon (;)?");
    }
}

void Rules::parse_symbols()
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
            // Add symbols to set
            node.extract_symbols(set);
            // map lhs to rhs for processing
            _tokenMap.insert(std::pair<std::string, SyntaxNode>(token, std::move(node)));
        }
        else
        {
            throw std::runtime_error("No equality found in rule");
        }
    }

    int count = 0;
    // only one lhs should have no rhs definition
    for (const auto& x : _tokenMap)
    {
        if (set.count(x.first) == 0)
        {
            _root = x.first;
            count++;
        }
    }

    // Check for errors
    if (count > 1)
    {
        throw std::runtime_error("Multiple root symbols found");
    }
    else if (count == 0)
    {
        throw std::runtime_error("No root symbol found");
    }
}

bool Rules::validate(const std::string& token, const std::string& input, size_t& position) const
{
    auto iter = _tokenMap.find(token);
    if (iter != _tokenMap.end())
    {
        size_t reset = position;
        bool ret = matches(iter->second, input, position);
        // Reset position if matches fails
        if (!ret)
        {
            position = reset;
        }
        return ret;
    }
    else
    {
        throw std::runtime_error("Could not located token in token map");
    }
}

bool Rules::validate(const std::string& token, const std::string& input) const
{
    size_t position = 0;
    return validate(token, input, position) && input.size() == position;
}

bool Rules::matches(const SyntaxNode& node, const std::string& match, size_t& position) const
{
    SyntaxNode const* left = node.get_left();
    SyntaxNode const* right = node.get_right();
    const size_t size = match.size();
    const size_t start = position;
    const SyntaxNode::NodeType type = node.get_type();
    std::string const* symbol = &node.get_symbol();
    const bool repeat = node.get_repeat();
    const std::vector<std::string>& simplified = node.get_simplified();
    const bool is_simple = simplified.size() > 0;

    // If null pointer than it is a leaf
    if ((!left || !right) && type > SyntaxNode::LEAF)
    {
        throw std::runtime_error("Rules.matches(): null pointer");
    }

    // If we reached the end
    if (start == size)
    {
        return false;
    }
    // SIMPLE_LEAF or LEAF node
    else if (type < SyntaxNode::ALTER)
    {
        bool match_found = false;
        const size_t symbols = is_simple ? simplified.size() : 1;
        const size_t stop = repeat ? size : start + 1;
        for (size_t i = 0; i < symbols; i++)
        {
            if (position == stop)
            {
                break;
            }

            if (is_simple)
            {
                symbol = &simplified[i];
            }
            if (symbol->size() == 1)
            {
                for (size_t j = position; j < stop; j++)
                {
                    if ((*symbol)[0] != match[j])
                    {
                        break;
                    }
                    position++;
                    match_found = true;
                }
                // Start over
                if(is_simple && match_found && repeat)
                {
                    i = -1;
                    match_found = false;
                    continue;
                }
                else if (is_simple && match_found && !repeat)
                {
                    break;
                }
            }
            else if (symbol->size() > 1)
            {
                // Short circuit evaluation, repeat and non-repeat
                while (validate(*symbol, match, position) && repeat) {}
            }
            else
            {
                throw std::runtime_error("Symbol has zero size, invalid state occured");
            }
        }
    }
    else if (type == SyntaxNode::ALTER)
    {
        // Short circuit evaluation, repeat and non-repeat, left or right
        while ((matches(*left, match, position) || matches(*right, match, position)) && repeat) {}
    }
    else if (type == SyntaxNode::CONCAT)
    {
        size_t reset = position;

        // Short circuit evaluation, repeat and non-repeat, left and right
        while (matches(*left, match, position) && matches(*right, match, position))
        {
            reset = position;
            if (!repeat)
            {
                break;
            }
        }

        // Reset when only left matches but not right
        position = reset;
    }
    return position > start;
}
