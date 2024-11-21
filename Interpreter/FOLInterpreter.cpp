//
// Created by owd on 18/11/24.
//

#include <memory>

#include "FOLLexer.hpp"
#include "AST/ISentenceNode.hpp"

namespace optifol
{

[[maybe_unused]] std::shared_ptr<optifol::ISentenceNode> yyroot;

}

int main()
{
    optifol::FOLLexer scanner{std::cin, std::cerr};
    optifol::FOLParser parser{&scanner};

    while (true)
        if (parser.parse() == 0)
            std::cout << optifol::yyroot->to_string() << std::endl;
}
