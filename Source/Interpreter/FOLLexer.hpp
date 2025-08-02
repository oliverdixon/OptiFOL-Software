/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef FOLLEXER_HPP
#define FOLLEXER_HPP

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#pragma ide diagnostic ignored "NotImplementedFunctions"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "FOLParser.hpp"

#ifndef __FLEX_LEXER_H
#undef yyFlexLexer
#define yyFlexLexer FOLFlexLexer
#include <FlexLexer.h>
#endif

namespace optifol
{

class FOLLexer :
        public yyFlexLexer
{
public:
    FOLLexer(std::istream &yy_in, std::ostream &yy_out) :
            yyFlexLexer(yy_in, yy_out)
    {}

    int lex(FOLParser::semantic_type *yylval);
};

}

#pragma clang diagnostic pop

#endif
