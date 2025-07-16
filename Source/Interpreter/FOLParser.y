/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

%require "3.7.4"
%language "C++"

%define api.parser.class {BaseFOLParser}
%define api.namespace {optifol::impl}
%define api.prefix {FOL}
%define api.value.type variant
%define parse.error detailed
%defines
%skeleton "lalr1.cc"
%parse-param {FOLLexer* scanner}

%code requires
{
    #include "../IR/MutableVariants/Terms/MutableVariable.hpp"
    #include "../IR/MutableVariants/Terms/MutableFunction.hpp"
    #include "../IR/MutableVariants/Terms/MutableConstant.hpp"

    #include "../IR/MutableVariants/Sentences/MutablePredicate.hpp"
    #include "../IR/MutableVariants/Sentences/MutableIdentity.hpp"
    #include "../IR/MutableVariants/Sentences/MutableBinaryConnected.hpp"
    #include "../IR/MutableVariants/Sentences/MutableQuantified.hpp"
    #include "../IR/MutableVariants/Sentences/MutableSentenceRoot.hpp"

    namespace optifol
    {
        class FOLLexer;
    }
}

%code
{
    #include "FOLLexer.hpp"
    #define yylex(x) scanner->lex(x)
}

%token <std::string> Constant
%token <std::string> Function
%token <std::string> Variable
%token <std::string> Predicate

%token Universal Existential
%token Conjunction Disjunction Negation Implication Biconditional Identity
%token LeftParenthesis RightParenthesis Comma
%token End

%left Biconditional
%left Implication
%left Disjunction
%left Conjunction
%right Negation

%type <IMutableSentence *> sentence
%type <IMutableTerm *> term
%type <std::vector<std::unique_ptr<IMutableTerm>>> term_vector

%start line

/*
 * Note the overall strategy for handling internal state during the parse. Raw C pointers are used to refer to nodes
 * during the build process, and are only transformed into unique pointers (via std::unique_ptr<T>(T*) or T::build(...))
 * at their relative termini. Thus the built model has full ownership semantics, and we don't have to worry about Bison
 * generating implicit calls to the default copy constructor, which is deleted for std::unique_ptr.
 */

%%

line :
    sentence End
    {
        static_cast<FOLParser *>(this)->register_sentence(
            MutableSentenceRoot::build(std::unique_ptr<IMutableSentence>($1))
        );

        return 0;
    }
    |
    error
    {
        return -1;
    }
    ;

sentence :
    Universal Variable LeftParenthesis sentence RightParenthesis
    {
        $$ = new MutableQuantified(
            QuantifierTypes::Universal,
            MutableVariable::build($2),
            std::unique_ptr<IMutableSentence>($4)
        );
    }
    |
    Existential Variable LeftParenthesis sentence RightParenthesis
    {
        $$ = new MutableQuantified(
            QuantifierTypes::Existential,
            MutableVariable::build($2),
            std::unique_ptr<IMutableSentence>($4)
        );
    }
    |
    Predicate LeftParenthesis term_vector RightParenthesis
    {
        $$ = new MutablePredicate($1, std::move($3));
    }
    |
    term Identity term
    {
        $$ = new MutableIdentity(std::unique_ptr<IMutableTerm>($1), std::unique_ptr<IMutableTerm>($3));
    }
    |
    Negation sentence
    {
        $2->flip_polarity();
        $$ = $2;
    }
    |
    sentence Conjunction sentence
    {
        $$ = new MutableBinaryConnected(
            BinaryOperatorTypes::Conjunction,
            std::unique_ptr<IMutableSentence>($1),
            std::unique_ptr<IMutableSentence>($3)
        );
    }
    |
    sentence Disjunction sentence
    {
        $$ = new MutableBinaryConnected(
            BinaryOperatorTypes::Disjunction,
            std::unique_ptr<IMutableSentence>($1),
            std::unique_ptr<IMutableSentence>($3)
        );
    }
    |
    sentence Implication sentence
    {
        $$ = new MutableBinaryConnected(
            BinaryOperatorTypes::Implication,
            std::unique_ptr<IMutableSentence>($1),
            std::unique_ptr<IMutableSentence>($3)
        );
    }
    |
    sentence Biconditional sentence
    {
        $$ = new MutableBinaryConnected(
            BinaryOperatorTypes::Biconditional,
            std::unique_ptr<IMutableSentence>($1),
            std::unique_ptr<IMutableSentence>($3)
            );
        }
    |
    LeftParenthesis sentence RightParenthesis
    {
        $$ = $2;
    }
    ;

term_vector :
    %empty
    {
        $$ = std::vector<std::unique_ptr<IMutableTerm>>();
    }
    |
    term
    {
        $$ = std::vector<std::unique_ptr<IMutableTerm>>();
        $$.push_back(std::unique_ptr<IMutableTerm>($1));
    }
    |
    term_vector Comma term
    {
        $$ = std::move($1);
        $$.push_back(std::unique_ptr<IMutableTerm>($3));
    }
    ;

term :
    Function LeftParenthesis term_vector RightParenthesis
    {
        $$ = new MutableFunction($1, std::move($3));
    }
    |
    Constant
    {
        $$ = new MutableConstant($1);
    }
    |
    Variable
    {
        $$ = new MutableVariable($1);
    }
    ;

%%

void optifol::impl::BaseFOLParser::error(const std::string& msg)
{
    static_cast<FOLParser *>(this)->error(msg);
}
