// Copyright 2016 <Aaron Springstroh>
#include "Lexer.h"
#include "Parse.h"
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    try
    {
        if (argc == 3)
        {
            std::string src = parse::read_file(argv[1]);
            std::string rule = parse::read_file(argv[2]);
            Lexer lexer(rule);
            if (lexer.is_valid(src))
            {
                std::cout << "Source is valid!" << std::endl;
            }
            else
            {
                std::cout << "Source is NOT valid!" << std::endl;
            }
        }
        else
        {
            std::cout << "Usage: lang src rules" << std::endl;
            std::cout << "Error: Not enough inputs provided" << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " + std::string(e.what()) << std::endl;
    }
    return 0;
}