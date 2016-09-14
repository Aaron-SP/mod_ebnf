#ifndef __SyntaxNode_

#include <string>
#include <vector>

namespace parse
{
    std::vector<char> read_file(const std::string& file);
    std::string& strip_single(std::string& str);
    std::string& strip_double(std::string& str);
    bool strip_quotes(std::string& str);
    bool in_quotes(const char ch, bool& quotes, char& quote_char);
    bool in_brackets(const char ch, const bool quotes, bool& in_brackets, char & bracket_char);
    bool bracket(const char ch);
    bool end_bracket(const char ch, const char bracket_char);
}

#endif
