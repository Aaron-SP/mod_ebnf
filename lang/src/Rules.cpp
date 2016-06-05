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
        matches(iter->second, input, position);
        return input.size() == position;
    }
    else
    {
        throw std::runtime_error("Could not located token in token map");
    }
}

bool Rules::validate(const std::string& token, const std::string& input)
{
    size_t position = 0;
    return validate(token, input, position);
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
                for (int i = position; i < size; i++)
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
            return validate(symbol, match, position);
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
        // Loop on characters
        if (repeat)
        {
            do
            {
                state = false;
                if (left)
                {
                    while (matches(*left, match, position))
                    {
                        state = true;
                    };
                }
                if (right)
                {
                    while (matches(*right, match, position))
                    {
                        state = true;
                    };
                }
            } while (state);
        }
        else
        {
            if (matches(*left, match, position))
            {
                return position > start;
            }
            if (matches(*right, match, position))
            {
                return position > start;
            }
        }
        // Did we make it to the end?
        return position > start;
    }
    else if (type == SyntaxNode::CONCAT)
    {
        bool lState;
        bool rState;
        if (repeat)
        {
            do
            {
                lState = rState = false;
                if (left)
                {
                    while (matches(*left, match, position)) 
                    {
                        lState = true;
                    }
                }
                // If found no matches or found all matches
                if (position == start || position == size)
                {
                    return false;
                }
                if (right)
                {
                    while (matches(*right, match, position)) 
                    {
                        rState = true;
                    }
                }
                // Did we make it to the end?
            } while (lState && rState);
        }
        else
        {
            matches(*left, match, position);
            // If found no matches or found all matches
            if (position == start || position == size)
            {
                return false;
            }
            matches(*right, match, position);
        }
        return position > start;
    }
    return false;
}