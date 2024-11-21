//
// Created by owd on 18/11/24.
//

#include "FOLLexer.hpp"
#include "FOLParser.hpp"

int main()
{
    optifol::FOLLexer lexer{std::cin, std::cerr};
    optifol::FOLParser parser(&lexer);

    while (true)
        parser.parse();

    return 0;
}
