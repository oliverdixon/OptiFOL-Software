/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

%{
#include "FOLParser.hpp"
#include "FOLLexer.hpp"

#undef YY_DECL
#define YY_DECL int optifol::FOLLexer::lex(optifol::FOLParser::semantic_type *yylval)
%}

%option c++ interactive noyywrap noyylineno
%option yyclass="FOLLexer"
%option prefix="FOL"

lowercase_sequence ([a-z]+)
uppercase_sequence ([A-Z]+)
whitespace         ([ \t]+)
symbol_prefix      "%"
function_prefix    "#"
character_sequence ([a-zA-Z]+)

%%

{function_prefix}{character_sequence}   yylval->emplace<std::string>(YYText()); return FOLParser::token::Function;
{lowercase_sequence}                    yylval->emplace<std::string>(YYText()); return FOLParser::token::Variable;
{uppercase_sequence}                    yylval->emplace<std::string>(YYText()); return FOLParser::token::Predicate;

{symbol_prefix}"U"   return FOLParser::token::Universal;
{symbol_prefix}"E"   return FOLParser::token::Existential;

"&"     return FOLParser::token::Conjunction;
"|"     return FOLParser::token::Disjunction;
"~"     return FOLParser::token::Negation;
"=>"    return FOLParser::token::Implication;
"<=>"   return FOLParser::token::Biconditional;
"="     return FOLParser::token::Identity;

"(" return FOLParser::token::LeftParenthesis;
")" return FOLParser::token::RightParenthesis;
"," return FOLParser::token::Comma;

\n           return FOLParser::token::End;
<<EOF>>      return FOLParser::token::End;
{whitespace} /* Ignore whitespace */

%%
