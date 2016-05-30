#include "Parse.h"
#include <fstream>
#include <algorithm>

std::vector<char> parse::read_file(const std::string& filePath)
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

std::string& parse::strip_squotes(std::string& str)
{
    str.erase(remove_if(str.begin(), str.end(), [](char chr) { return chr == '\''; }), str.end());
    return str;
}

std::string& parse::strip_dquotes(std::string& str)
{
    str.erase(remove_if(str.begin(), str.end(), [](char chr) { return chr == '\"'; }), str.end());
    return str;
}

bool parse::strip_quotes(std::string& str)
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
            parse::strip_squotes(str);
        }
        else if (d_quote > 1)
        {
            parse::strip_dquotes(str);
        }
    }
    else if (s_quote)
    {
        parse::strip_squotes(str);
    }
    else if (d_quote)
    {
        parse::strip_dquotes(str);
    }
    return s_quote || d_quote;
}

bool parse::in_quotes(const char ch, bool& quotes, char& quote_char)
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

bool parse::in_brackets(const char ch, bool quotes, bool& in_brackets, char & bracket_char)
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