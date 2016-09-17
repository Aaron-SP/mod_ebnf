// Copyright 2016 <Aaron Springstroh>
#include "Lexer.h"
#include <chrono>
#include <iostream>
#include <string>

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
        throw std::runtime_error("Assert String Failed: '" +
            f(lhs) + "' is not equal to '" + f(rhs) + "'");
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
    Rules rules(rule);
}

void rule_validate(const std::string& rule)
{
    Rules rules(rule);
    rules.validate("pie", "test");
}

int main(int argc, char** argv)
{
    try
    {
        // Start time
        auto start = std::chrono::high_resolution_clock::now();

        // Test getRoot()
        {
            std::string rule = "identifier = letter , { letter | digit | \"_\" };";
            Rules rules(rule);
            assert<std::string>("identifier", rules.getRoot(), string_s);
        }
        // Test no closing quote
        {
            std::string rule = "identifier = letter , { letter | digit | \"_ };";
            std::string error = "End of file found before closing quotation";
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
            std::string rule = "identifier = letter , { letter | digit | \"_\" };"
                "character = letter | digit | symbol | \"_\";";
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
            std::string error = "Found end of file before end of quote/brace";
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
            Rules rules(rule);
            assert<bool>(true, rules.validate("letter", "A"), bool_s);
            assert<bool>(true, rules.validate("letter", "B"), bool_s);
            assert<bool>(false, rules.validate("letter", "Q"), bool_s);
            assert<bool>(false, rules.validate("letter", "Z"), bool_s);
            assert<bool>(false, rules.validate("letter", "AB"), bool_s);
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
            Rules rules(rule);
            assert<bool>(true, rules.validate("letter", "AC"), bool_s);
            assert<bool>(true, rules.validate("letter", "AD"), bool_s);
            assert<bool>(false, rules.validate("letter", "CB"), bool_s);
            assert<bool>(false, rules.validate("letter", "CD"), bool_s);
            assert<bool>(false, rules.validate("letter", "AAD"), bool_s);
        }
        // Test multi rule matches
        {
            std::string rule = "letter = \"A\" | \"B\" , \"C\" | \"D\"; "
                "digit = \"0\" | \"1\" | \"2\" | \"3\"; word = letter , digit;";
            Rules rules(rule);
            assert<std::string>("word", rules.getRoot(), string_s);
            assert<bool>(true, rules.validate("word", "AC1"), bool_s);
            assert<bool>(false, rules.validate("word", "ABCD"), bool_s);
            assert<bool>(false, rules.validate("word", "0123"), bool_s);
            assert<bool>(false, rules.validate("word", "ACA0"), bool_s);
        }
        // Test repeat alternation
        {
            std::string rule = "letter = {\"A\" | \"B\" | \"C\" | \"D\"};";
            Rules rules(rule);
            assert<bool>(true, rules.validate("letter", "AAAABBBBCCCCDDD"), bool_s);
            assert<bool>(true, rules.validate("letter", "ABCDDCBAADCBCABD"), bool_s);
        }
        // Test repeat concatenation
        {
            std::string rule = "letter = {\"A\" | \"B\"} , {\"C\" | \"D\"};";
            Rules rules(rule);
            assert<bool>(true, rules.validate("letter", "AAAABBBBCCCCDDD"), bool_s);
            assert<bool>(true, rules.validate("letter", "ABABABACDCDCDC"), bool_s);
            assert<bool>(false, rules.validate("letter", "AAAABBBBCCCCDDDA"), bool_s);
        }
        // Test advanced repeat concatenation
        {
            std::string rule = "letter = {(\"A\" | \"B\") , (\"C\" | \"D\")};";
            Rules rules(rule);
            assert<bool>(true, rules.validate("letter", "ACBDBDAC"), bool_s);
            assert<bool>(false, rules.validate("letter", "ACBDBDACA"), bool_s);
            assert<bool>(false, rules.validate("letter", "ABCD"), bool_s);
        }
        // Test multi rule repeat
        {
            std::string rule = "letter = \"A\" | \"B\" | \"C\" | \"D\"; "
                "digit = \"0\" | \"1\" | \"2\" | \"3\"; word = { letter } , { digit };";
            Rules rules(rule);
            assert<std::string>("word", rules.getRoot(), string_s);
            assert<bool>(true, rules.validate("word", "ABCD0123"), bool_s);
            assert<bool>(false, rules.validate("word", "A1A"), bool_s);
            assert<bool>(false, rules.validate("word", "ABCDE01234"), bool_s);
        }
        // Test repeat multi rule concatenation
        {
            std::string rule = "letter = \"A\" , \"B\"; word = { letter };";
            Rules rules(rule);
            assert<std::string>("word", rules.getRoot(), string_s);
            assert<bool>(true, rules.validate("word", "ABABAB"), bool_s);
            assert<bool>(false, rules.validate("word", "BABABA"), bool_s);
            assert<bool>(false, rules.validate("word", "AABAB"), bool_s);
            assert<bool>(false, rules.validate("word", "ABA"), bool_s);
        }
        // Priority brackets
        {
            std::string rule = "letter = { ( \"A\" | \")\" ) , ( \"C\" | \"D\" ) };";
            Rules rules(rule);
            assert<std::string>("letter", rules.getRoot(), string_s);
            assert<bool>(true, rules.validate("letter", "AC)D"), bool_s);
            assert<bool>(false, rules.validate("letter", "AC(D"), bool_s);
        }
        // Test recursive rule substitution
        {
            std::string rule = "letter = \"A\" | \"B\" | ( \"(\" , letter , \")\" ); "
                "word = { letter };";
            Rules rules(rule);
            assert<std::string>("word", rules.getRoot(), string_s);
            assert<bool>(true, rules.validate("word", "ABBAA"), bool_s);
            assert<bool>(true, rules.validate("word", "(A)BBAA"), bool_s);
            assert<bool>(false, rules.validate("word", "(A)C"), bool_s);
            assert<bool>(false, rules.validate("word", "A(C)"), bool_s);
        }
        // Test simple EBNF rules
        {
            std::string rule = "letter=\"A\"|\"B\"|\"C\"|\"D\"|\"E\"|\"F\"|\"G\"|\"H\"|\"I\"|\"J\"|\"K\"|\"L\"|\"M\"|\"N\"|\"O\"|\"P\"|\"Q\"|\"R\"|\"S\"|\"T\"|\"U\"|\"V\"|\"W\"|\"X\"|\"Y\"|\"Z\"|\"a\"|\"b\"|\"c\"|\"d\"|\"e\"|\"f\"|\"g\"|\"h\"|\"i\"|\"j\"|\"k\"|\"l\"|\"m\"|\"n\"|\"o\"|\"p\"|\"q\"|\"r\"|\"s\"|\"t\"|\"u\"|\"v\"|\"w\"|\"x\"|\"y\"|\"z\";digit=\"0\"|\"1\"|\"2\"|\"3\"|\"4\"|\"5\"|\"6\"|\"7\"|\"8\"|\"9\";symbol=\"[\"|\"]\"|\"{\"|\"}\"|\"(\"|\")\"|\"<\"|\">\"|\"'\"|'\"'|\"=\"|\"|\"|\".\"|\",\";character=letter|digit|symbol|\"_\";identifier=letter,{letter|digit|\"_\"};terminal=('\"',character,'\"');lhs=identifier;rhs=identifier|{terminal|\"|\"};rule=lhs,\"=\",rhs,\";\";grammar={rule};";
            Rules rules(rule);
            assert<std::string>("grammar", rules.getRoot(), string_s);
            assert<bool>(true, rules.validate("grammar", "letter=\"A\"|\"B\"|\"C\"|\"D\";word=letter;"), bool_s);
        }
        // Test recursive EBNF rules
        {
            std::string rule = "letter=\"A\"|\"B\"|\"C\"|\"D\"|\"E\"|\"F\"|\"G\"|\"H\"|\"I\"|\"J\"|\"K\"|\"L\"|\"M\"|\"N\"|\"O\"|\"P\"|\"Q\"|\"R\"|\"S\"|\"T\"|\"U\"|\"V\"|\"W\"|\"X\"|\"Y\"|\"Z\"|\"a\"|\"b\"|\"c\"|\"d\"|\"e\"|\"f\"|\"g\"|\"h\"|\"i\"|\"j\"|\"k\"|\"l\"|\"m\"|\"n\"|\"o\"|\"p\"|\"q\"|\"r\"|\"s\"|\"t\"|\"u\"|\"v\"|\"w\"|\"x\"|\"y\"|\"z\";digit=\"0\"|\"1\"|\"2\"|\"3\"|\"4\"|\"5\"|\"6\"|\"7\"|\"8\"|\"9\";symbol=\"[\"|\"]\"|\"{\"|\"}\"|\"(\"|\")\"|\"<\"|\">\"|\"'\"|'\"'|\"=\"|\"|\"|\".\"|\",\";character=letter|digit|symbol|\"_\";identifier=letter,{letter|digit|\"_\"};terminal=('\"',character,'\"');separator=\",\"|\"|\";expression=(identifier|terminal)|(\"{\",expression,\"}\")|(\"(\",expression,\")\");lhs=identifier;rhs=({expression,separator},expression)|expression;rule=lhs,\"=\",rhs,\";\";grammar={rule};";
            Rules rules(rule);
            assert<std::string>("grammar", rules.getRoot(), string_s);
            assert<bool>(true, rules.validate("grammar", "letter=\"A\"|\"B\",\"C\"|\"D\";word=letter;"), bool_s);
        }
        // Test nested recursive EBNF rules on itself
        {
            std::string rule = "letter=\"A\"|\"B\"|\"C\"|\"D\"|\"E\"|\"F\"|\"G\"|\"H\"|\"I\"|\"J\"|\"K\"|\"L\"|\"M\"|\"N\"|\"O\"|\"P\"|\"Q\"|\"R\"|\"S\"|\"T\"|\"U\"|\"V\"|\"W\"|\"X\"|\"Y\"|\"Z\"|\"a\"|\"b\"|\"c\"|\"d\"|\"e\"|\"f\"|\"g\"|\"h\"|\"i\"|\"j\"|\"k\"|\"l\"|\"m\"|\"n\"|\"o\"|\"p\"|\"q\"|\"r\"|\"s\"|\"t\"|\"u\"|\"v\"|\"w\"|\"x\"|\"y\"|\"z\";digit=\"0\"|\"1\"|\"2\"|\"3\"|\"4\"|\"5\"|\"6\"|\"7\"|\"8\"|\"9\";symbol=\"[\"|\"]\"|\"{\"|\"}\"|\"(\"|\")\"|\"<\"|\">\"|\"'\"|'\"'|\"=\"|\"|\"|\".\"|\",\"|\";\";character=letter|digit|symbol|\"_\";identifier=letter,{letter|digit|\"_\"};terminal=('\"',character,'\"')|(\"'\",character,\"'\");separator=\",\"|\"|\";term=identifier|terminal;expression=({term,separator},term)|(\"(\",expression,\")\")|(\"{\",expression,\"}\")|term;statement=({expression,separator},expression)|(\"(\",statement,\")\")|(\"{\",statement,\"}\")|expression;lhs=identifier;rhs=({statement,separator},statement)|statement;rule=lhs,\"=\",rhs,\";\";grammar={rule};";
            Rules rules(rule);
            assert<std::string>("grammar", rules.getRoot(), string_s);
            assert<bool>(true, rules.validate("grammar", rule), bool_s);
        }

        // End time
        auto end = std::chrono::high_resolution_clock::now();
        // Delta in seconds
        auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);

        // All tests passed
        std::cout << std::endl << "Tests passed in " << time_span.count() << " seconds!" << std::endl;
    }
    catch (const std::exception& e)
    {
        // Tests failed
        std::cout << e.what() << std::endl << std::endl;
        std::cout << "Tests failed!" << std::endl;
    }
    return 0;
}
