/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

%{
#include "GoogleTestParser.hpp"
#include "GoogleTestLexer.hpp"

#define YY_DECL int optifol::GoogleTestLexer::lex(optifol::GoogleTestParser::semantic_type *yylval)
#define yypanic(X) (void)(X)
%}

%option c++ interactive noyywrap noyylineno

event_prefix "event="
whitespace ([ \t]+)

%%

{event_prefix}"TestProgramStart"     return GoogleTestParser::token::ProgramStart;

<<EOF>>      return GoogleTestParser::token::End;
{whitespace} /* Ignore whitespace */

%%

int yyFlexLexer::yylex()
{
    throw std::runtime_error("Bad call to yyFlexLexer::yylex()");
}
