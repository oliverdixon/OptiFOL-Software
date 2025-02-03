/*
 * Copyright (c) All Rights Reserved
 * 2024-2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef OPTIFOL_FOLLEXER_HPP
#define OPTIFOL_FOLLEXER_HPP

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#pragma ide diagnostic ignored "NotImplementedFunctions"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#ifndef __FLEX_LEXER_H
// Note that the yyFlexLexer definition is not suitably guarded by some versions of Flex.
#include <FlexLexer.h>
#endif

#include "FOLParser.hpp"

namespace optifol
{

class FOLLexer :
        public yyFlexLexer
{
public:
    FOLLexer(std::istream &yy_in, std::ostream &yy_out) :
            yyFlexLexer(yy_in, yy_out)
    {}

    explicit FOLLexer(std::istream *yy_in = nullptr, std::ostream *yy_out = nullptr) :
            yyFlexLexer(yy_in, yy_out)
    {}

    int lex(FOLParser::semantic_type *yylval);
};

}

#pragma clang diagnostic pop

#endif //OPTIFOL_FOLLEXER_HPP
