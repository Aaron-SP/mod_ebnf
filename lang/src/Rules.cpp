#include "Rules.h"
#include "Parse.h"

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

void Rules::parseRules()
{
    // white space list
    std::vector<char> ws{ 9, 10, 13, 32 };
    std::string rule;
    rule.reserve(_text.size());
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
            _tokenMap.insert(std::pair<std::string, SyntaxNode>(token, std::move(node)));
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

bool Rules::validate(const std::string& token, const std::string& input, size_t& position)
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

bool Rules::validate(const std::string& token, const std::string& input)
{
    size_t position = 0;
    return validate(token, input, position) && input.size() == position;
}

bool Rules::matches(const SyntaxNode& node, const std::string& match, size_t& position)
{
    SyntaxNode* left = node.getLeft();
    SyntaxNode* right = node.getRight();
    const size_t size = match.size();
    const size_t start = position;
    const SyntaxNode::NodeType type = node.getType();
    const std::string& symbol = node.getSymbol();
    const bool repeat = node.getRepeat();

    if ((!left || !right) && type != SyntaxNode::LEAF)
    {
        throw std::runtime_error("Rules.matches(): null pointer");
    }

    // If we reached the end
    if (start == size)
    {
        return false;
    }
    else if (type == SyntaxNode::LEAF)
    {
        if (symbol.size() == 1)
        {
            if (repeat)
            {
                for (size_t i = position; i < size; i++)
                {
                    if (symbol[0] != match[i])
                    {
                        break;
                    }
                    position++;
                }
            }
            else
            {
                if (symbol[0] == match[position])
                {
                    position++;
                }
            }
        }
        else if (symbol.size() > 1)
        {
            if (repeat)
            {
                while (validate(symbol, match, position)) {}
            }
            else
            {
                validate(symbol, match, position);
            }
        }
        else
        {
            throw std::runtime_error("Symbol has zero size, invalid state occured");
        }
        return position > start;
    }
    else if (type == SyntaxNode::ALTER)
    {
        bool state;
        if (repeat)
        {
            do
            {
                state = false;
                while (matches(*left, match, position))
                {
                    state = true;
                }
                while (matches(*right, match, position))
                {
                    state = true;
                }
            } while (state);
        }
        else
        {
            return matches(*left, match, position)
                || matches(*right, match, position);
        }
        return position > start;
    }
    else if (type == SyntaxNode::CONCAT)
    {
        bool lState = false;
        bool rState = false;
        size_t reset = position;
        if (repeat)
        {
            // Get run of concatenations
            do
            {
                lState = matches(*left, match, position);
                if (!lState)
                {
                    break;
                }
                rState = matches(*right, match, position);
                if (lState && rState)
                {
                    reset = position;
                }
            } while (lState && rState);

            // Reset if only odd number of matches
            position = reset;
            return position > start;
        }
        else
        {
            return matches(*left, match, position) &&
                matches(*right, match, position);
        }
    }
    return false;
}