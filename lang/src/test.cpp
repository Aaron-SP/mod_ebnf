#include "Lexer.h"
#include <iostream>

std::string bool_s(const bool& b)
{
    if (b == true)
    {
        return "true";
    }
    return "false";
}

std::string string_s(const std::string& s)
{
    return s;
}

template <class T> void assert(const T& lhs, const T& rhs, std::string(*f)(const T&))
{
    if (lhs == rhs)
    {
        std::cout << "Test Passed!" << std::endl;
    }
    else
    {
        throw std::runtime_error("Assert String Failed: '" + f(lhs) + "' is not equal to '" + f(rhs) + "'");
    }
}

void assert_throw(void(*func)(const std::string&), std::string arg, std::string error)
{
    try
    {
        func(arg);
        std::cout << "Warning::Assert Throw did not throw!" << std::endl;
    }
    catch (const std::exception& ex)
    {
        if (ex.what() == error)
        {
            std::cout << "Test Passed!" << std::endl;
        }
        else
        {
            throw std::runtime_error(ex.what());
        }
    }
}

void make_rule(const std::string& rule)
{
    std::vector<char> v(rule.begin(), rule.end());
    Rules rules(v);
}

void rule_validate(const std::string& rule)
{
    std::vector<char> v(rule.begin(), rule.end());
    Rules rules(v);
    rules.validate("pie", "test");
}

int main(int argc, char** argv)
{
    try
    {
        // Test getRoot()
        {
            std::string rule = "identifier = letter , { letter | digit | \"_\" };";
            std::vector<char> v(rule.begin(), rule.end());
            Rules rules(v);
            assert<std::string>("identifier", rules.getRoot(), string_s);
        }
        // Test no closing quote
        {
            std::string rule = "identifier = letter , { letter | digit | \"_ };";
            std::string error = "Quotation '/\" mismatch. Check rules input file";
            assert_throw(make_rule, rule, error);
        }
        // Test no semi colon
        {
            std::string rule = "identifier = letter , { letter | digit | \"_\" }";
            std::string error = "Could not find any rules, did you miss a semi-colon (;)?";
            assert_throw(make_rule, rule, error);
        }
        // Test multiple root node exception
        {
            std::string rule = "identifier = letter , { letter | digit | \"_\" };character = letter | digit | symbol | \"_\";";
            std::string error = "Multiple root symbols found";
            assert_throw(make_rule, rule, error);
        }
        // Test no root node exception
        {
            std::string rule = "identifier = identifier | letter , { letter | digit | \"_\" };";
            std::string error = "No root symbol found";
            assert_throw(make_rule, rule, error);
        }
        //  Test no 2nd brace
        {
            std::string rule = "identifier = identifier | letter , { letter | digit | \"_\";";
            std::string error = "Found EOL before end of quote/brace";
            assert_throw(make_rule, rule, error);
        }
        // Test empty symbol
        {
            std::string rule = "identifier = identifier | letter , , { letter | digit | \"_\";";
            std::string error = "Empty symbol found while constructing rule";
            assert_throw(make_rule, rule, error);
        }
        // Test text before brace
        {
            std::string rule = "identifier = identifier | letter , test { letter | digit | \"_\";";
            std::string error = "Braces must be fully delimited, found text before start brace";
            assert_throw(make_rule, rule, error);
        }
        // Test alternation matches
        {
            std::string rule = "letter = \"A\" | \"B\" | \"C\" | \"D\";";
            std::vector<char> v(rule.begin(), rule.end());
            Rules rules(v);
            assert<bool>(true, rules.validate("letter", "A"), bool_s);
            assert<bool>(true, rules.validate("letter", "ABCDABDCADCBA"), bool_s);
            assert<bool>(false, rules.validate("letter", "Q"), bool_s);
            assert<bool>(false, rules.validate("letter", "ABCDABDCADCBAQ"), bool_s);
        }
        // Test invalid token exception
        {
            std::string rule = "letter = \"A\" | \"B\" | \"C\" | \"D\";";
            std::string error = "Could not located token in token map";
            assert_throw(rule_validate, rule, error);
        }
        // Test concatenation matches
        {
            std::string rule = "letter = \"A\" | \"B\" , \"C\" | \"D\";";
            std::vector<char> v(rule.begin(), rule.end());
            Rules rules(v);
            assert<bool>(true, rules.validate("letter", "AC"), bool_s);
            assert<bool>(true, rules.validate("letter", "AAAABACDC"), bool_s);
            assert<bool>(false, rules.validate("letter", "CB"), bool_s);
            assert<bool>(false, rules.validate("letter", "CD"), bool_s);
        }
        // Test multi rule matches
        {
            std::string rule = "letter = \"A\" | \"B\" , \"C\" | \"D\"; digit = \"0\" | \"1\" | \"2\" | \"3\"; word = letter , digit;";
            std::vector<char> v(rule.begin(), rule.end());
            Rules rules(v);
            assert<std::string>("word", rules.getRoot(), string_s);
            assert<bool>(true, rules.validate("word", "ABCD0123"), bool_s);
            assert<bool>(false, rules.validate("word", "ABCD"), bool_s);
            assert<bool>(false, rules.validate("word", "0123"), bool_s);
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}