#include "Rules.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <set>

std::vector<char> Rules::read_file(const std::string& filePath)
{
    std::vector<char> data;
    std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        std::streampos size = file.tellg();
        if (size > 0)
        {
            data.resize(size);
            file.seekg(0, std::ios::beg);
            file.read(&data[0], size);
        }
        file.close();
    }
    else
    {
        throw std::runtime_error("Can't open file: " + filePath);
    }
    return data;
}

std::string& Rules::strip_squotes(std::string& str)
{
    str.erase(remove_if(str.begin(), str.end(), [](char chr) { return chr == '\''; }), str.end());
    return str;
}

std::string& Rules::strip_dquotes(std::string& str)
{
    str.erase(remove_if(str.begin(), str.end(), [](char chr) { return chr == '\"'; }), str.end());
    return str;
}

bool Rules::strip_quotes(std::string& str)
{
    int s_quote = 0; int d_quote = 0;
    for (auto& ch : str)
    {
        if (ch == '\"')
        {
            d_quote++;
        }
        else if (ch == '\'')
        {
            s_quote++;
        }
    }
    if (s_quote && d_quote)
    {
        if (s_quote > 1 && d_quote > 1)
        {
            throw std::runtime_error("Invalid quote formation detected");
        }
        else if (s_quote > 1)
        {
            Rules::strip_squotes(str);
        }
        else if (d_quote > 1)
        {
            Rules::strip_dquotes(str);
        }
    }
    else if (s_quote)
    {
        Rules::strip_squotes(str);
    }
    else if (d_quote)
    {
        Rules::strip_dquotes(str);
    }
    return s_quote || d_quote;
}

bool Rules::in_quotes(const char ch, bool& quotes, char& quote_char)
{
    bool action = false;
    if ((ch == '\'' || ch == '\"'))
    {
        if (!quotes)
        {
            // turn on quotes
            quotes = true;
            quote_char = ch;
            action = true;
        }
        else if (quotes && quote_char == ch)
        {
            // turn off quotes
            quotes = false;
            quote_char = 0;
            action = true;
        }
    }
    return action;
}

bool Rules::in_brackets(const char ch, bool quotes, bool& in_brackets, char & bracket_char)
{
    bool action = false;
    if (!quotes)
    {
        if ((ch == '{' || ch == '[') && !in_brackets)
        {
            // turn on brackets
            in_brackets = true;
            bracket_char = ch;
            action = true;
        }
        else if ((ch == '}' || ch == ']') && in_brackets && bracket_char == '{')
        {
            // turn off brackets
            in_brackets = false;
            bracket_char = 0;
            action = true;
        }
    }
    return action;
}

std::vector<SyntaxNode> Rules::tokenize(const std::string& source)
{
    std::vector<SyntaxNode> out;
    size_t start = 0; size_t end = 0;
    const size_t size = source.size();
    bool quotes = false;
    bool brackets = false;
    bool bracket_action = false;
    char quote_char = 0;
    char bracket_char = 0;

    // We use root to initialize only
    SyntaxNode::NodeType last_type = SyntaxNode::ROOT;

    // end points to line after current char
    for (size_t end = 0; end < size; end++)
    {
        char ch = source[end];
        in_quotes(ch, quotes, quote_char);
        bracket_action = in_brackets(ch, quotes, brackets, bracket_char);

        // Enter bracket
        if (bracket_action && brackets)
        {
            // Skip everything to after bracket
            start = end + 1;
        }
        // Escape bracket
        else if (bracket_action && !brackets)
        {
            if (end - start > 0)
            {
                std::vector<SyntaxNode> again = tokenize(source.substr(start, end - start));
                out.insert(out.end(), again.begin(), again.end());
                // Move to after bracket
                start = end + 1;
            }
        }

        if (((ch == '|' || ch == ',') && !quotes && !brackets) || end == size - 1)
        {
            // Get the correct length if reach the end
            if (end == size - 1)
            {
                end++;
            }
            if (end - start > 0)
            {
                std::string s = source.substr(start, end - start);

                // if it is a string then it is a leaf in the parse tree
                bool isString = strip_quotes(s);
                if (ch == '|')
                {
                    last_type = SyntaxNode::NodeType::ALTER;
                }
                else if (ch == ',')
                {
                    last_type = SyntaxNode::NodeType::CONCATE;
                }
                else if (last_type == SyntaxNode::ROOT)
                {
                    last_type = SyntaxNode::NodeType::EQUALS;
                }
                SyntaxNode node(last_type, s, !isString);
                out.push_back(node);
                start = end + 1;
            }
        }
    }
    return out;
}

Rules::Rules(const std::string& rules) : _tree("root")
{
    _text = Rules::read_file(rules);
    parseRules();
    _tree = ParseTree(parseSymbols());
    std::cout << _tree.getRoot().getSymbol() << std::endl;
}

void Rules::parseRules()
{
    const size_t size = _text.size();
    // white space list
    std::vector<char> ws{ 9, 10, 13, 32 };
    std::string rule = std::string();
    char quote_char = 0;
    bool quotes = false;

    for (size_t i = 0; i < size; i++)
    {
        const char ch = _text[i];

        // If whitespace then ignore
        if (std::find(ws.begin(), ws.end(), ch) == ws.end())
        {
            in_quotes(ch, quotes, quote_char);
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
            std::vector<SyntaxNode> symbols = tokenize(equality);
            // Get the SyntaxNode's symbol and put them in the set
            std::transform(symbols.begin(), symbols.end(), std::inserter(set, set.end()), SyntaxNode::toString);
            // map lhs to rhs for processing
            _tokenMap.insert({ token, symbols });
        }
        else
        {
            throw std::runtime_error("No equality found in rule.");
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
        throw std::runtime_error("Multiple root symbols found.");
    }
    return root;
}