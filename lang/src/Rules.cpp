#include "Rules.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <stdexcept>

std::vector<char> Rules::readFile(const std::string& filePath)
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

std::string& Rules::strip_quotes(std::string& str)
{
    str.erase(remove_if(str.begin(), str.end(), [](char chr) { return chr == '\'' || chr == '\"'; }), str.end());
    return str;
}

void Rules::in_quotes(char ch, bool& quotes, char& quote_char)
{
    if ((ch == '\'' || ch == '\"') && !quotes)
    {
        // turn on quotes
        quotes = true;
        quote_char = ch;
    }
    else if ((ch == '\'' || ch == '\"') && quotes && quote_char == ch)
    {
        // turn off quotes
        quotes = false;
        quote_char = 0;
    }
}

std::vector<std::string> Rules::tokenize(std::string source)
{
    std::vector<std::string> out;
    size_t start = 0; size_t end = 0;
    const size_t size = source.size();

    bool quotes = false;
    bool in_brackets = false;
    char quote_char = 0;
    char bracket_char = 0;

    for (size_t end=0; end<size; end++)
    {
        char ch = source[end];
        in_quotes(ch, quotes, quote_char);

        if ((ch == '|' || ch == ',') && !quotes && !in_brackets)
        {
            if (end - start > 0)
            {
                std::string s = source.substr(start, end - start);
                strip_quotes(s);
                out.push_back(s);
                start = end + 1;
            }
        }
        else if ((ch == '{' || ch == '[') && !quotes && !in_brackets)
        {
            // turn on brackets
            in_brackets = true;
            bracket_char = ch;
            start = end + 1;
        }
        else if ((ch == '}' || ch == ']') && !quotes && in_brackets && bracket_char == '{')
        {
            // turn off brackets
            std::vector<std::string> again = tokenize(source.substr(start, end - start));
            out.insert(out.end(), again.begin(), again.end());
            in_brackets = false;
            bracket_char = 0;
            start = end + 1;
        }
    }

    // if we hit the end of the line with characters in buffer
    if (end - start > 0)
    {
        std::string s = source.substr(start, end - start);
        strip_quotes(s);
        out.push_back(s);
        start = end + 1;
    }
    return out;
}

Rules::Rules(const std::string& rules)
{
    _text = Rules::readFile(rules);
    parseRules();
    parseSymbols();
}

void Rules::parseRules()
{
    const size_t size = _text.size();
    // white space list
    std::vector<char> ws{9, 10, 13, 32};
    std::string rule = std::string();
    char quote_char = 0;
    bool quotes = false;

    for (size_t i = 0; i < size; i++)
    {
        const char ch = _text[i];

        // If not whitespace then skip
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

    if (quotes)
    {
        throw std::runtime_error("Quotation '/\" mismatch. Check rules input file");
    }
 }

void printVector(const std::vector<std::string>& list)
{
    const size_t size = list.size();
    if (size > 0)
    {
        std::cout << "{" << list[0];
        for (size_t i = 1; i<size; i++)
        {
            std::cout << '.' << list[i];
        }
        std::cout << "}";
    }
}

void Rules::parseSymbols()
{
    for (const auto& rule : _ruleList)
    {
        size_t found = rule.find('=');
        if (found != std::string::npos)
        {
            // lhs
            std::string token = rule.substr(0, found);
            // rhs
            std::string equality = rule.substr(found + 1, rule.size() - found - 1);

            std::vector<std::string> symbols = tokenize(equality);
            _tokenMap.insert({ token, symbols });
        }
        else
        {
            throw std::runtime_error("No equality found in rule.");
        }
    }

    for (const auto& x : _tokenMap)
    {
        std::cout << x.first << "=";
        printVector(x.second);
        std::cout << std::endl;
    }
}