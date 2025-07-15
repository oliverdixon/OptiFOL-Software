/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

%require "3.7.4"
%language "C++"

%define api.parser.class {BaseGoogleTestParser}
%define api.namespace {optifol::impl}
%define api.value.type variant
%define api.prefix {GoogleTest}
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

%token EventMarker
%token ProtocolVersion
%token ProgramStart
%token ProgramEnd
%token IterationStart
%token IterationEnd
%token TestCaseStart
%token TestCaseEnd
%token TestStart
%token TestEnd
%token TestPartial
%token ParameterDelimeter
%token KeyValueDelimeter
%token End

%token <std::string> Decimal

%start program_entry

%%

program_entry :
     protocol_line ProgramStart ProgramEnd End
     {
         std::cout << "Valid!" << std::endl;
         return 0;
     }
     |
     error
     {
         return -1;
     }
     ;

/* TODO: we need to determine a good AST for storing test results. They'll be immediately assigned to existing GLib
 *  test objects, so how heavy of a middle layer do we need? Could we just have a TestResult class composed by Test? */

protocol_line :
     ProtocolVersion KeyValueDelimeter Decimal
     {
         std::cout << "Google Test, TCP protocol " << $3 << std::endl;
     }
     ;

%%

void optifol::impl::BaseGoogleTestParser::error(const std::string& msg)
{
    static_cast<GoogleTestParser *>(this)->error(msg);
}
