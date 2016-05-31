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

int main(int argc, char** argv)
{
    try
    {
        std::string rule ="identifier = letter , { letter | digit | \"_\" } ;";
        std::vector<char> v(rule.begin(), rule.end());
        Rules rules(v);
        // Test getRoot()
        assert("identifier", rules.getRoot());

    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}