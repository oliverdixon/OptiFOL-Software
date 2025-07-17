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
    #include <memory>

    #include "../Exceptions/SemanticException.hpp"
    #include "../Exceptions/ParseError.hpp"
    #include "../Storage/TestResult.hpp"
    #include "../Logging.hpp"

    namespace optifol
    {
       class GoogleTestLexer;
    }
}

%code
{
    #include "GoogleTestLexer.hpp"
    #define yylex(x) scanner->lex(x)

    const log4cxx::LoggerPtr logger = optifol::Logging::get_logger({"UserTesting", "GoogleTestResultsParser"});
}

%token ProtocolVersion
%token End
%token Passed
%token ElapsedTime
%token Name

%token <std::string> Literal

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

%type <TestResult::Partial> partial_test
%type <std::vector<TestResult::Partial>> partial_test_list

%start program_entry

%%

program_entry :
    protocol_line ProgramStart iteration_list ProgramEnd Passed Literal End
    {
        const auto& results = dynamic_cast<GoogleTestParser&>(*this).observe_test_results();
        std::cout << "Ran " << std::to_string(results.size()) << " tests with result " << $6 << std::endl;
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
            if (fabsf(version - supported_version) >= std::numeric_limits<float>::epsilon())
                throw SemanticException("Unsupported Google Test protocol version " + $2);
        } catch (const std::logic_error& parsing_exception) {
            logger->error(parsing_exception.what());
            throw ParseError("Could not parse Google Test protocol version number \"" + $2 + '"', 0); // TODO column number
        } catch (const SemanticException& semantic_exception) {
            logger->error(semantic_exception.what());
            throw;
        }
    }
    ;

iteration :
    IterationStart IterationCount Literal case_list IterationEnd Passed Literal ElapsedTime Literal
    {
        // Collapse iteration lists
        if (logger->isDebugEnabled())
            logger->debug("Parsed iteration #" + $3 + ", taking " + $9 + " with an overall result of " + $7 + '.');
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

case :
    TestCaseStart Name Literal test_list TestCaseEnd Passed Literal ElapsedTime Literal
    {
        // Collapse test case sets
        if (logger->isDebugEnabled())
            logger->debug("Parsed test case \"" + $3 + "\", taking " + $9 + " with an overall result of " + $7 + '.');
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
        try {
            const auto passed_result = std::stoi($7);
            if (passed_result != 0 && passed_result != 1)
                throw SemanticException("Invalid test result; should be '0' or '1', but received " + $7);

            auto result = std::make_unique<TestResult>(passed_result, std::stoi($9), std::move($4));
            dynamic_cast<GoogleTestParser&>(*this).add_result(std::move(result));

            if (logger->isDebugEnabled())
                logger->debug("Parsed test \"" + $3 + "\", taking " + $9 + " with an overall result of " +
                    ((passed_result) ? "PASS" : "FAIL") + '.');
        } catch (const std::logic_error& parsing_exception) {
            logger->error(parsing_exception.what());
            throw ParseError("Could not parse Google Test test result \"" + $3 + '"', 0); // TODO column number
        } catch (const SemanticException& semantic_exception) {
            logger->error(semantic_exception.what());
            throw;
        }
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
        try {
            $$ = TestResult::Partial($3, std::stoi($5), $7);

            if (logger->isDebugEnabled())
                logger->debug("Parsed partial test result in file \"" + $3 +"\" at line " + $5 + '.');
        } catch (const std::logic_error& parsing_exception) {
            logger->error(parsing_exception.what());
            throw ParseError("Could not parse Google Test partial result in file \"" + $3 + "\" at line " + $5 +
                "\": " + parsing_exception.what(), 0); // TODO column number
        }
    }

partial_test_list :
    %empty
    {
    }
    |
    partial_test_list partial_test
    {
        $$ = std::move($1);
        $$.push_back(std::move($2));
    }
    ;

%%

void optifol::impl::BaseGoogleTestParser::error(const std::string& msg)
{
    dynamic_cast<GoogleTestParser&>(*this).error(msg);
}
