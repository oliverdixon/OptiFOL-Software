//
// Created by owd on 18/11/24.
//

#include "Interpreter/FOLLexer.hpp"
#include "Interpreter/FOLParser.hpp"

#include "Visitors/ImplicationEliminationVisitor.hpp"
#include "Visitors/UniversalEliminationVisitor.hpp"
#include "Visitors/DisjunctionDistributionVisitor.hpp"

int main()
{
    optifol::FOLLexer lexer{std::cin, std::cerr};
    optifol::FOLParser parser(&lexer);

    static const std::array<std::pair<std::unique_ptr<optifol::VisitorBase>, std::string>, 3> cnfNormalisers{ {
        { std::make_unique<optifol::ImplicationEliminationVisitor>(),  "ImplElim" },
        { std::make_unique<optifol::UniversalEliminationVisitor>(),    "UnivElim" },
        { std::make_unique<optifol::DisjunctionDistributionVisitor>(), "DisjDist" },
    } };

    while (parser.parse() == 0) {
        const auto root = parser.retrieve_sentence();
        std::cout << "[Parsed]\t" << root->to_string() << '\n';

        for (const auto& [visitor, name] : cnfNormalisers) {
            root->accept(*visitor);
            std::cout << '[' << name << "]\t" << root->to_string() << '\n';
        }
    }

    return 0;
}
