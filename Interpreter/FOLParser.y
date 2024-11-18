%{
#include <iostream>
#include <string>
#include <cmath>
%}

%require "3.7.4"
%language "C++"

%define api.parser.class {FOLParser}
%define api.namespace {optifol}
%define api.value.type variant
%parse-param {FOLLexer* scanner}

%code requires
{
    namespace optifol {
        class FOLLexer;
    }
}

%code
{
    #include "FOLLexer.hpp"
    #define yylex(x) scanner->lex(x)
}

%token              EOL LPAREN RPAREN
%token <long long>  INT
%token <double>     FLT
%token <char>       INTVAR FLTVAR

%nterm <long long>  iexp
%nterm <double>     fexp

%nonassoc           ASSIGN
%left               PLUS MINUS
%left               MULTIPLY DIVIDE MODULO
%precedence         UMINUS
%precedence         FACTORIAL
%right              EXPONENT

%%

lines   : %empty
        | lines line
        ;

line    : EOL                       { std::cerr << "Read an empty line.\n"; }
        | iexp EOL                  { std::cout << $1 << '\n'; }
        | fexp EOL                  { std::cout << $1 << '\n'; }
        | error EOL                 { yyerrok; }
        ;

iexp    : INT                       { $$ = $1; }
        | iexp PLUS iexp            { $$ = $1 + $3; }
        | iexp MINUS iexp           { $$ = $1 - $3; }
        | iexp MULTIPLY iexp        { $$ = $1 * $3; }
        | iexp DIVIDE iexp          { $$ = $1 / $3; }
        | iexp MODULO iexp          { $$ = $1 % $3; }
        | MINUS iexp %prec UMINUS   { $$ = -$2; }
        | LPAREN iexp RPAREN        { $$ = $2; }
        ;

fexp    : FLT                       { $$ = $1; }
        | fexp PLUS fexp            { $$ = $1 + $3; }
        | fexp MINUS fexp           { $$ = $1 - $3; }
        | fexp MULTIPLY fexp        { $$ = $1 * $3; }
        | fexp DIVIDE fexp          { $$ = $1 / $3; }
        | fexp EXPONENT fexp        { $$ = pow($1, $3); }
        | MINUS fexp %prec UMINUS   { $$ = -$2; }
        | LPAREN fexp RPAREN        { $$ = $2; }
        ;

%%

void optifol::FOLParser::error(const std::string& msg) {
    std::cerr << msg << '\n';
}
