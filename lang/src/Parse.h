#ifndef __SyntaxNode_

#include <string>
#include <vector>

namespace parse
{
    std::vector<char> read_file(const std::string& file);
    std::string& strip_squotes(std::string& str);
    std::string& strip_dquotes(std::string& str);
    bool strip_quotes(std::string& str);
    bool in_quotes(char ch, bool& quotes, char& quote_char);
    bool in_brackets(const char ch, bool quotes, bool& in_brackets, char & bracket_char);
}

#endif
