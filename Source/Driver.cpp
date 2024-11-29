//
// Created by owd on 18/11/24.
//

#include "Interpreter/FOLLexer.hpp"

#include "Visitors/CNFNormalisers/ImplicationEliminationVisitor.hpp"
#include "Visitors/CNFNormalisers/DMLVisitor.hpp"
#include "Visitors/CNFNormalisers/UniversalEliminationVisitor.hpp"
#include "Visitors/CNFNormalisers/DisjunctionDistributionVisitor.hpp"

#include "Visitors/Serialisers/TextSerialiserVisitor.hpp"
#include "Visitors/Serialisers/JSONSerialiserVisitor.hpp"

namespace
{

std::string get_text(const optifol::ISentenceNode& node)
{
    // Just a quick helper. Not anything more!
    static optifol::TextSerialiserVisitor text_serialiser;
    node.accept(text_serialiser);
    const auto string = std::any_cast<optifol::TextSerialiserVisitor::OutT>(text_serialiser.extract());
    text_serialiser.reset();
    return string;
}

std::string get_json(const optifol::ISentenceNode& node)
{
    // Just a quick helper. Not anything more!
    static optifol::JSONSerialiserVisitor json_serialiser;
    node.accept(json_serialiser);
    const auto string = std::any_cast<optifol::TextSerialiserVisitor::OutT>(json_serialiser.extract());
    json_serialiser.reset();
    return string;
}

}

int main()
{
    optifol::FOLLexer lexer{std::cin, std::cerr};
    optifol::FOLParser parser(&lexer);

    static const std::array<std::tuple<std::unique_ptr<optifol::MutatingVisitorBase>, std::string, bool>, 4>
            cnf_normalisers{{
                                    {std::make_unique<optifol::ImplicationEliminationVisitor>(), "ImplElim", true},
                                    {std::make_unique<optifol::DMLVisitor>(), "DeMorgan", true},
                                    {std::make_unique<optifol::UniversalEliminationVisitor>(), "UnivElim", true},
                                    {std::make_unique<optifol::DisjunctionDistributionVisitor>(), "DisjDist", true}
                            }};

    while (parser.parse() == 0) {
        const auto root = parser.retrieve_sentence();
        std::cout << "[Parsed]\t" << get_json(*root) << '\n';

        for (const auto &[visitor, name, enabled]: cnf_normalisers) {
            if (enabled) {
                root->accept(*visitor);
                std::cout << '[' << name << "]\t" << get_json(*root) << '\n';
                visitor->reset();
            }
        }
    }

    return 0;
}
