/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Google Test payload lexer base class
 * @author Oliver Dixon
 * @date 2025-08-02
 * @version Development
 */

#ifndef GOOGLETESTLEXER_HPP
#define GOOGLETESTLEXER_HPP

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#pragma ide diagnostic ignored "NotImplementedFunctions"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "GoogleTestParser.hpp"

#ifndef __FLEX_LEXER_H
#undef yyFlexLexer
#define yyFlexLexer GoogleTestFlexLexer
#include <FlexLexer.h>
#endif

namespace optifol
{

/**
 * @class GoogleTestLexer
 * @brief Base class for the Flex-generated C++ lexer to lex Google Test results payloads.
 * @see GoogleTestParser for the parsing dual.
 */
class GoogleTestLexer : public yyFlexLexer
{
public:
    /**
     * @brief Construct a new lexer object for lexing Google Test result payloads.
     * @param yy_in The input stream.
     * @param yy_out The output stream.
     */
    GoogleTestLexer(std::istream &yy_in, std::ostream &yy_out) :
        yyFlexLexer(yy_in, yy_out)
    {
    }

    /**
     * @brief Lex the next token on the input stream and
     * @param yylval The lexed token type output parameter
     * @return Flex error state
     */
    int lex(GoogleTestParser::semantic_type *yylval);
};

} // namespace optifol

#pragma clang diagnostic pop

#endif
