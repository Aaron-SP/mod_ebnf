#include "Lexer.h"
#include <iostream>

void assert(const std::string lhs, const std::string rhs)
{
    if (lhs == rhs)
    {
        std::cout << "Test Passed!" << std::endl;
    }
    else
    {
        throw std::runtime_error("Assert String Failed: '" + lhs + "' is not equal to '" + rhs + "'");
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

void make_rule(const std::string & rule)
{
    std::vector<char> v(rule.begin(), rule.end());
    Rules rules(v);
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
            assert("identifier", rules.getRoot());
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
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}