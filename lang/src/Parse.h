// Copyright 2016 <Aaron Springstroh>
#ifndef __SyntaxNode_

#include <string>

namespace parse
{
    std::string read_file(const std::string& file);
    void strip_single(std::string& str);
    void strip_double(std::string& str);
    bool strip_quotes(std::string& str);
    bool in_quotes(const char ch, bool& quotes, char& quote_char);
    bool in_brackets(const char ch, const bool quotes, bool& in_brackets, char & bracket_char);
    bool bracket(const char ch);
    bool end_bracket(const char ch, const char bracket_char);
}  // namespace parse

#endif
