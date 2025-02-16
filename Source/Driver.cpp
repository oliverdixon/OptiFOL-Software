/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifdef OPTIFOL_DISABLE_GUI

#include <array>

#include "Exceptions/SemanticException.hpp"
#include "Interpreter/FOLLexer.hpp"

#include "Visitors/Sentences/CNFNormalisers/DisjunctionDistributionVisitor.hpp"
#include "Visitors/Sentences/CNFNormalisers/DMLVisitor.hpp"
#include "Visitors/Sentences/CNFNormalisers/ExistentialShiftingVisitor.hpp"
#include "Visitors/Sentences/CNFNormalisers/ImplicationEliminationVisitor.hpp"
#include "Visitors/Sentences/CNFNormalisers/SkolemIntroducingVisitor.hpp"
#include "Visitors/Sentences/CNFNormalisers/SymbolStandardisingVisitor.hpp"
#include "Visitors/Sentences/CNFNormalisers/UniversalEliminationVisitor.hpp"

#include "Visitors/Sentences/Serialisers/JSONSerialiserVisitor.hpp"
#include "Visitors/Sentences/Serialisers/TextSerialiserVisitor.hpp"

#include "Storage/PGDatabase.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"

namespace
{

[[maybe_unused]] std::string get_text(const optifol::ISentenceNode &node)
{
    // Just a quick helper. Not anything more!
    static optifol::TextSerialiserVisitor text_serialiser;
    node.accept(text_serialiser);
    const auto string = text_serialiser.extract();
    return string;
}

[[maybe_unused]] std::string get_json(const optifol::ISentenceNode &node)
{
    // Just a quick helper. Not anything more!
    static optifol::JSONSerialiserVisitor json_serialiser;
    node.accept(json_serialiser);
    const auto string = json_serialiser.extract().dump();
    return string;
}

}

int main()
{
    optifol::FOLLexer lexer{std::cin, std::cerr};
    optifol::FOLParser parser(&lexer);

    static const std::array<std::tuple<std::unique_ptr<optifol::MutatingSentenceVisitorBase>, std::string, bool>, 7>
            cnf_normalisers{
                {
                    {std::make_unique<optifol::ImplicationEliminationVisitor>(), "ImplElim", false},
                    {std::make_unique<optifol::DMLVisitor>(), "DeMorgan", true},
                    {std::make_unique<optifol::SymbolStandardisingVisitor>(), "SymbStnd", false},
                    {std::make_unique<optifol::ExistentialShiftingVisitor>(), "ExisShft", false},
                    {std::make_unique<optifol::SkolemIntroducingVisitor>(), "SklmIntr", false},
                    {std::make_unique<optifol::UniversalEliminationVisitor>(), "UnivElim", false},
                    {std::make_unique<optifol::DisjunctionDistributionVisitor>(), "DisjDist", false}
                }
            };

    while (parser.parse() == 0) {
        const auto root = parser.retrieve_sentence();
        std::cout << "[Parsed]\t" << get_text(*root) << '\n';

        for (const auto &[visitor, name, enabled]:
             cnf_normalisers) {
            if (enabled) {
                try {
                    root->accept(*visitor);
                    std::cout << '[' << name << "]\t" << get_json(*root) << '\n';
                } catch (const optifol::SemanticException &exception) {
                    std::cerr << exception.what() << std::endl;
                }

                visitor->reset();
            }
        }
    }

    return 0;
}

#pragma clang diagnostic pop

#endif
