/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef GOOGLETESTLEXER_HPP
#define GOOGLETESTLEXER_HPP

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#pragma ide diagnostic ignored "NotImplementedFunctions"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#ifndef __FLEX_LEXER_H
// Note that the yyFlexLexer definition is not suitably guarded by some versions of Flex.
#include <FlexLexer.h>
#endif

#include "GoogleTestParser.hpp"

namespace optifol
{

class GoogleTestLexer :
        public yyFlexLexer
{
public:
    GoogleTestLexer(std::istream &yy_in, std::ostream &yy_out) :
            yyFlexLexer(yy_in, yy_out)
    {}

    explicit GoogleTestLexer(std::istream *yy_in = nullptr, std::ostream *yy_out = nullptr) :
            yyFlexLexer(yy_in, yy_out)
    {}

    int lex(GoogleTestParser::semantic_type *yylval);
};

}

#pragma clang diagnostic pop

#endif
