//
// Created by owd on 18/11/24.
//

#include "FOLLexer.hpp"

int main()
{
    optifol::FOLLexer scanner{std::cin, std::cerr};
    optifol::FOLParser parser{&scanner};
    std::cout.precision(10);
    parser.parse();
}
