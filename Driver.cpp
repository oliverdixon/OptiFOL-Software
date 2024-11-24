//
// Created by owd on 18/11/24.
//

#include "Interpreter/FOLLexer.hpp"
#include "Interpreter/FOLParser.hpp"

#include "Visitors/ImplicationPurgingVisitor.hpp"

int main()
{
    optifol::FOLLexer lexer{std::cin, std::cerr};
    optifol::FOLParser parser(&lexer);

    while (parser.parse() == 0) {
        const auto root = parser.retrieve_sentence();
        std::cout << "[As-Parsed]\t" << root->to_string() << '\n';

        optifol::ImplicationPurgingVisitor normaliser;
        root->accept(normaliser);
        std::cout << "[As-Normalised]\t" << root->to_string() << '\n';
    }

    return 0;
}
