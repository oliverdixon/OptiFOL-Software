/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the static logical Analysis Manager
 * @author Oliver Dixon
 * @date 2025-05-04
 * @version Development
 */

#ifndef ANALYSISMANAGER_HPP
#define ANALYSISMANAGER_HPP

#include <sstream>

#include "../Interpreter/FOLLexer.hpp"
#include "../Interpreter/FOLParser.hpp"

namespace optifol
{

class AnalysisManager
{
public:
    enum class NormalisationExtent
    {
        ImplicationElimination,
        DML,
        SymbolStandardising,
        // TODO: move quantifiers outwards for Skolemisation (introduction of Skolem functions)
        SkolemIntroducing,
        UniversalElimination,
        DisjunctionDistribution,

        AllCNF
    };

    static std::unique_ptr<ISentenceNode> parse_sentence(std::string&& sentence);

    static std::unique_ptr<ISentenceNode> normalise_sentence(std::unique_ptr<ISentenceNode>&& sentence,
        NormalisationExtent extent);

    static std::string get_json(const ISentenceNode * node);

    static std::string get_text(const ISentenceNode * node);

private:
    static std::unique_ptr<ISentenceNode> implication_elimination(std::unique_ptr<ISentenceNode>&& sentence);

    static std::istringstream lexer_input_stream;
    static FOLLexer lexer;
    static FOLParser parser;
};

}

#endif
