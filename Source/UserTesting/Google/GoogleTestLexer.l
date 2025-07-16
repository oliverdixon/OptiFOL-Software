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

%%

[^\t\n=&]+ {
    yylval->emplace<std::string>(YYText());
    return GoogleTestParser::token::Literal;
}

"gtest_streaming_protocol_version=" return GoogleTestParser::token::ProtocolVersion;
"&elapsed_time=" return GoogleTestParser::token::ElapsedTime;
"&passed=" return GoogleTestParser::token::Passed;
"&name=" return GoogleTestParser::token::Name;

"event=TestProgramStart" return GoogleTestParser::token::ProgramStart;
"event=TestProgramEnd" return GoogleTestParser::token::ProgramEnd;

"event=TestIterationStart" return GoogleTestParser::token::IterationStart;
"&iteration=" return GoogleTestParser::token::IterationCount;
"event=TestIterationEnd" return GoogleTestParser::token::IterationEnd;

"event=TestCaseStart" return GoogleTestParser::token::TestCaseStart;
"event=TestCaseEnd" return GoogleTestParser::token::TestCaseEnd;

"event=TestStart" return GoogleTestParser::token::TestStart;
"event=TestPartResult" return GoogleTestParser::token::TestPartial;
"&file=" return GoogleTestParser::token::File;
"&line=" return GoogleTestParser::token::Line;
"&message=" return GoogleTestParser::token::Message;
"event=TestEnd" return GoogleTestParser::token::TestEnd;

<<EOF>> return GoogleTestParser::token::End;
{whitespace} /* Ignore whitespace */

%%
