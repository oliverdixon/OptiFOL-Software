//
// Created by owd on 18/11/24.
//

#include "Interpreter/FOLLexer.hpp"

#include "Visitors/ImplicationEliminationVisitor.hpp"
#include "Visitors/DMLVisitor.hpp"
#include "Visitors/UniversalEliminationVisitor.hpp"
#include "Visitors/DisjunctionDistributionVisitor.hpp"

int main()
{
    optifol::FOLLexer lexer{std::cin, std::cerr};
    optifol::FOLParser parser(&lexer);

    static const std::array<std::tuple<std::unique_ptr<optifol::VisitorBase>, std::string, bool>, 4> cnfNormalisers{ {
        { std::make_unique<optifol::ImplicationEliminationVisitor>(),  "ImplElim", false },
        { std::make_unique<optifol::DMLVisitor>(),                     "DeMorgan", true },
        { std::make_unique<optifol::UniversalEliminationVisitor>(),    "UnivElim", false },
        { std::make_unique<optifol::DisjunctionDistributionVisitor>(), "DisjDist", false }
    } };

    while (parser.parse() == 0) {
        const auto root = parser.retrieve_sentence();
        std::cout << "[Parsed]\t" << root->to_string() << '\n';

        for (const auto& [visitor, name, enabled] : cnfNormalisers) {
            if (enabled) {
                root->accept(*visitor);
                visitor->reset();
                std::cout << '[' << name << "]\t" << root->to_string() << '\n';
            }
        }
    }

    return 0;
}
