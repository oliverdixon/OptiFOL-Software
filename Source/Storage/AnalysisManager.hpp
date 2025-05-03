/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
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
    static std::unique_ptr<ISentenceNode> parse_sentence(std::string&& sentence);

private:
    static std::istringstream lexer_input_stream;
    static FOLLexer lexer;
    static FOLParser parser;
};

}

#endif
