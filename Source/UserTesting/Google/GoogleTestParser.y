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
    #include <cmath>

    #include "../Exceptions/SemanticException.hpp"
    #include "../Exceptions/ParseError.hpp"

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

%token ProtocolVersion
%token End
%token Passed
%token ElapsedTime
%token Name

%token ProgramStart
%token ProgramEnd

%token IterationStart
%token IterationCount
%token IterationEnd

%token TestCaseStart
%token TestCaseEnd

%token TestStart
%token TestPartial
%token File
%token Line
%token Message
%token TestEnd

%token <std::string> Literal

%start program_entry

%%

program_entry :
    protocol_line ProgramStart iteration_list ProgramEnd Passed Literal End
    {
        std::cout << "Valid. Resulted in " << $6 << std::endl;
        return 0;
    }
    |
    error
    {
        return -1;
    }
    ;

protocol_line :
    ProtocolVersion Literal
    {
        constexpr float supported_version = 1.0f;

        try {
            const float version = std::stof($2);
            if (fabsf(version - supported_version) >= std::numeric_limits<double>::epsilon())
                throw SemanticException("Unsupported Google Test protocol version " + $2);
        } catch (const std::logic_error& parsing_exception) {
            throw ParseError("Could not parse Google Test protocol version number \"" + $2 + '"', 0); // TODO column number
        }
    }
    ;
    
iteration_list :
    %empty
    {
    }
    |
    iteration_list iteration
    {
    }
    ;

iteration :
    IterationStart IterationCount Literal case_list IterationEnd Passed Literal ElapsedTime Literal
    {
        std::cout << "Iteration #" << $3 << " resulted in " << $7 << " executed in " << $9 << std::endl;
    }
    ;

case :
    TestCaseStart Name Literal test_list TestCaseEnd Passed Literal ElapsedTime Literal
    {
        std::cout << "Test case " << $3 << " resulted in " << $7 << " executed in " << $9 << std::endl;
    }
    ;

case_list :
    %empty
    {
    }
    |
    case_list case
    {
    }
    ;

test :
    TestStart Name Literal partial_test_list TestEnd Passed Literal ElapsedTime Literal
    {
        std::cout << "Test " << $3 << " resulted in " << $7 << " executed in " << $9 << std::endl;
    }

test_list:
    %empty
    {
    }
    |
    test_list test
    {
    }
    ;

partial_test :
    TestPartial File Literal Line Literal Message Literal
    {
    }

partial_test_list :
    %empty
    {
    }
    |
    partial_test_list partial_test
    {
    }
    ;

%%

void optifol::impl::BaseGoogleTestParser::error(const std::string& msg)
{
    static_cast<GoogleTestParser *>(this)->error(msg);
}
