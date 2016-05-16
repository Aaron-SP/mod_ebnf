#include "Lexer.h"
#include <iostream>

int main(int argc, char** argv)
{
    try
    {
        if (argc > 2)
        {
            Lexer lexer(argv[1], argv[2]);
        }
        else
        {
            std::cout << "Usage: lang src rules" << std::endl;
            std::cout << "Error: Not enough inputs provided" << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}