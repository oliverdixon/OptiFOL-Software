/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

%{
#include "GoogleTestParser.hpp"
#include "GoogleTestLexer.hpp"

#undef YY_DECL
#define YY_DECL int optifol::GoogleTestLexer::lex(optifol::GoogleTestParser::semantic_type *yylval)
%}

%option c++ interactive noyywrap noyylineno
%option yyclass="GoogleTestLexer"
%option prefix="GoogleTest"

whitespace ([ \t\n]+)
numeric_sequence ([0-9]+)
decimal_sequence ([0-9.]+)

%%

"&" return GoogleTestParser::token::ParameterDelimeter;
"=" return GoogleTestParser::token::KeyValueDelimeter;

"gtest_streaming_protocol_version" return GoogleTestParser::token::ProtocolVersion;

"event=TestProgramStart" return GoogleTestParser::token::ProgramStart;
"event=TestProgramEnd" return GoogleTestParser::token::ProgramEnd;

"event=TestIterationStart" return GoogleTestParser::token::IterationStart;
"event=TestIterationEnd" return GoogleTestParser::token::IterationEnd;

{decimal_sequence} yylval->emplace<std::string>(YYText()); return GoogleTestParser::token::Decimal;

version_number

<<EOF>> return GoogleTestParser::token::End;
{whitespace} /* Ignore whitespace */

%%
