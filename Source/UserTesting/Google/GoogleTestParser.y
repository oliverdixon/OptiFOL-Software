/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

%require "3.7.4"
%language "C++"

%define api.parser.class {BaseGoogleTestParser}
%define api.namespace {optifol::impl}
%define api.value.type variant
%define parse.error detailed
%defines
%skeleton "lalr1.cc"
%parse-param {GoogleTestLexer* scanner}

%code requires
{
    namespace optifol
    {
        class GoogleTestLexer;
    }
}

%code
{
    #include "GoogleTestLexer.hpp"
    #define yylex(x) scanner->lex(x)
}

%token ProgramStart
%token End

%start line

%%

line :
     ProgramStart End
     {
         std::cout << "Program starting" << std::endl;
     }
     |
     error
     {
         return -1;
     }
     ;

%%

void optifol::impl::BaseGoogleTestParser::error(const std::string& msg)
{
    static_cast<GoogleTestParser *>(this)->error(msg);
}
