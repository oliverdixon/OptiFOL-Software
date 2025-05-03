/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#include "AnalysisManager.hpp"

namespace optifol
{

std::unique_ptr<optifol::ISentenceNode> AnalysisManager::parse_sentence(std::string&& sentence)
{
    lexer_input_stream.str(std::move(sentence));
    parser.parse();

    return parser.retrieve_sentence();
}

}
