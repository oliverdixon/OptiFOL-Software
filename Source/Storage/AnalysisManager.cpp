/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the static logical Analysis Manager
 * @author Oliver Dixon
 * @date 2025-05-04
 * @version Development
 */

#include "AnalysisManager.hpp"

#include "../Visitors/Sentences/CNFNormalisers/DisjunctionDistributionVisitor.hpp"
#include "../Visitors/Sentences/CNFNormalisers/DMLVisitor.hpp"
#include "../Visitors/Sentences/CNFNormalisers/ExistentialShiftingVisitor.hpp"
#include "../Visitors/Sentences/CNFNormalisers/ImplicationEliminationVisitor.hpp"
#include "../Visitors/Sentences/CNFNormalisers/SkolemIntroducingVisitor.hpp"
#include "../Visitors/Sentences/CNFNormalisers/SymbolStandardisingVisitor.hpp"
#include "../Visitors/Sentences/CNFNormalisers/UniversalEliminationVisitor.hpp"
#include "../Visitors/Sentences/Serialisers/JSONSerialiserVisitor.hpp"
#include "../Visitors/Sentences/Serialisers/TextSerialiserVisitor.hpp"

namespace optifol
{

std::istringstream AnalysisManager::lexer_input_stream;

/*
 * TODO: this construction is possibly undefined due to std::cerr. But for the real case, we'll use a custom error
 *  handler that can be statically initialised in the Analysis Manager, so this is OK for development.
 */
FOLLexer AnalysisManager::lexer{AnalysisManager::lexer_input_stream, std::cerr};
FOLParser AnalysisManager::parser{&AnalysisManager::lexer};

std::unique_ptr<optifol::ISentenceNode> AnalysisManager::parse_sentence(std::string&& sentence)
{
    lexer_input_stream.str(std::move(sentence));
    parser.parse();

    return parser.retrieve_sentence();
}

std::unique_ptr<ISentenceNode> AnalysisManager::normalise_sentence(std::unique_ptr<ISentenceNode> &&sentence,
    const NormalisationExtent extent)
{
    switch (extent) {
    case NormalisationExtent::ImplicationElimination:
        return implication_elimination(std::move(sentence));
    }

    return {}; // TODO: remove
}

std::string AnalysisManager::get_json(const ISentenceNode * node)
{
    // Just a quick helper. Not anything more!
    static JSONSerialiserVisitor json_serialiser;
    node->accept(json_serialiser);
    return json_serialiser.extract().dump();
}

std::string AnalysisManager::get_text(const optifol::ISentenceNode * node)
{
    // Just a quick helper. Not anything more!
    static TextSerialiserVisitor text_serialiser;
    node->accept(text_serialiser);
    return text_serialiser.extract();
}

std::unique_ptr<ISentenceNode> AnalysisManager::implication_elimination(std::unique_ptr<ISentenceNode> &&sentence)
{
    static auto normaliser = ImplicationEliminationVisitor();
    sentence->accept(normaliser);
    return sentence;
}

}
