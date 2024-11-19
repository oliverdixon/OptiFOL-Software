%{
#include "FOLParser.hpp"
#include "FOLLexer.hpp"

#define YY_DECL int optifol::FOLLexer::lex(optifol::FOLParser::semantic_type *yylval)
#define yypanic(X) (void)(X)
%}

%option c++ interactive noyywrap noyylineno

lowercase_sequence ([a-z]+)
uppercase_sequence ([A-Z]+)
whitespace         ([ \t]+)
symbol_prefix      "%"
constant_prefix    "_"
function_prefix    "$"
character_sequence ([a-zA-Z]+)

%%

{constant_prefix}{character_sequence}   yylval->emplace<std::string>(YYText()); return FOLParser::token::Constant;
{function_prefix}{character_sequence}   yylval->emplace<std::string>(YYText()); return FOLParser::token::Function;
{lowercase_sequence}                    yylval->emplace<std::string>(YYText()); return FOLParser::token::Variable;
{uppercase_sequence}                    yylval->emplace<std::string>(YYText()); return FOLParser::token::Predicate;

{symbol_prefix}"Univ"   return FOLParser::token::Universal;
{symbol_prefix}"Exis"   return FOLParser::token::Existential;
{symbol_prefix}"Conj"   return FOLParser::token::Conjunction;
{symbol_prefix}"Disj"   return FOLParser::token::Disjunction;
{symbol_prefix}"Negn"   return FOLParser::token::Negation;
{symbol_prefix}"Impl"   return FOLParser::token::Implication;
{symbol_prefix}"Bcnd"   return FOLParser::token::Biconditional;
{symbol_prefix}"Idty"   return FOLParser::token::Identity;

"(" return FOLParser::token::LeftParenthesis;
")" return FOLParser::token::RightParenthesis;
"," return FOLParser::token::Comma;

\n           return FOLParser::token::End;
<<EOF>>      return FOLParser::token::End;
{whitespace} /* Ignore whitespace */

%%

int yyFlexLexer::yylex()
{
    throw std::runtime_error("Bad call to yyFlexLexer::yylex()");
}
