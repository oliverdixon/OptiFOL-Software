%{
#include "FOLParser.hpp"
#include "FOLLexer.hpp"

#define YY_DECL int optifol::FOLLexer::lex(optifol::FOLParser::semantic_type *yylval)
#define yypanic(X) (void)(X)
%}

%option c++ interactive noyywrap noyylineno nodefault

dseq            ([[:digit:]]+)
dseq_opt        ({dseq}?)
frac            (({dseq_opt}"."{dseq})|{dseq}".")
exp             ([eE][+-]?{dseq})
exp_opt         ({exp}?)

integer         ({dseq})
float           (({frac}{exp_opt})|({dseq}{exp}))
intvar          ([[:upper:]])
fltvar          ([[:lower:]])

%%

{integer}       yylval->emplace<long long>(strtoll(YYText(), nullptr, 10)); return FOLParser::token::INT;
{float}         yylval->emplace<double>(strtod(YYText(), nullptr)); return FOLParser::token::FLT;
{intvar}        yylval->emplace<char>(YYText()[0]); return FOLParser::token::INTVAR;
{fltvar}        yylval->emplace<char>(YYText()[0]); return FOLParser::token::FLTVAR;
"+"             return FOLParser::token::PLUS;
"-"             return FOLParser::token::MINUS;
"*"             return FOLParser::token::MULTIPLY;
"/"             return FOLParser::token::DIVIDE;
"%"             return FOLParser::token::MODULO;
"!"             return FOLParser::token::FACTORIAL;
"^"             return FOLParser::token::EXPONENT;
"("             return FOLParser::token::LPAREN;
")"             return FOLParser::token::RPAREN;
"="             return FOLParser::token::ASSIGN;
\n              return FOLParser::token::EOL;
<<EOF>>         return FOLParser::token::YYEOF;
.               /* no action on unmatched input */

%%

int yyFlexLexer::yylex()
{
    throw std::runtime_error("Bad call to yyFlexLexer::yylex()");
}
