//
// Created by owd on 18/11/24.
//

#include "Interpreter/FOLLexer.hpp"
#include "Interpreter/FOLParser.hpp"

#include "Visitors/UniversalEliminationVisitor.hpp"
#include "Visitors/DisjunctionDistributionVisitor.hpp"

int main()
{
    optifol::FOLLexer lexer{std::cin, std::cerr};
    optifol::FOLParser parser(&lexer);

    while (parser.parse() == 0) {
        const auto root = parser.retrieve_sentence();
        std::cout << "[Parsed]\t" << root->to_string() << '\n';

        optifol::UniversalEliminationVisitor universalEliminationVisitor;
        root->accept(universalEliminationVisitor);
        std::cout << "[Uni. Elim.]\t" << root->to_string() << '\n';

        optifol::DisjunctionDistributionVisitor disjunctionDistributionVisitor;
        root->accept(disjunctionDistributionVisitor);
        std::cout << "[Disj. Dist.]\t" << root->to_string() << '\n';
    }

    return 0;
}
