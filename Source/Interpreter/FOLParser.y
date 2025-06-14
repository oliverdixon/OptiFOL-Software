/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

%require "3.7.4"
%language "C++"

%define api.parser.class {BaseParser}
%define api.namespace {optifol::impl}
%define api.value.type variant
%define parse.error detailed
%defines
%skeleton "lalr1.cc"
%parse-param {FOLLexer* scanner}

%code requires
{
    #include "../IR/Mutable/Terms/MutableVariableNode.hpp"
    #include "../IR/Mutable/Terms/MutableFunctionNode.hpp"
    #include "../IR/Mutable/Terms/MutableConstantNode.hpp"

    #include "../IR/Mutable/Sentences/MutablePredicationNode.hpp"
    #include "../IR/Mutable/Sentences/MutableIdentitySentenceNode.hpp"
    #include "../IR/Mutable/Sentences/MutableConnectedSentenceNode.hpp"
    #include "../IR/Mutable/Sentences/MutableQuantifiedSentenceNode.hpp"
    #include "../IR/Mutable/Sentences/MutableSentenceRoot.hpp"

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

%type <IMutableSentenceNode *> sentence
%type <IMutableTermNode *> term
%type <std::vector<std::unique_ptr<IMutableTermNode>>> term_vector

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
             MutableSentenceRoot::build(std::unique_ptr<IMutableSentenceNode>($1))
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
             $$ = new MutableQuantifiedSentenceNode(
                 QuantifierTypes::Universal,
                 MutableVariableNode::build($2),
                 std::unique_ptr<IMutableSentenceNode>($4)
             );
         }
         |
         Existential Variable LeftParenthesis sentence RightParenthesis
         {
             $$ = new MutableQuantifiedSentenceNode(
                 QuantifierTypes::Existential,
                 MutableVariableNode::build($2),
                 std::unique_ptr<IMutableSentenceNode>($4)
             );
         }
         |
         Predicate LeftParenthesis term_vector RightParenthesis
         {
             $$ = new MutablePredicationNode($1, std::move($3));
         }
         |
         term Identity term
         {
             $$ = new MutableIdentitySentenceNode(std::unique_ptr<IMutableTermNode>($1), std::unique_ptr<IMutableTermNode>($3));
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
             $$ = new MutableConnectedSentenceNode(
                 BinaryOperatorTypes::Conjunction,
                 std::unique_ptr<IMutableSentenceNode>($1),
                 std::unique_ptr<IMutableSentenceNode>($3)
             );
         }
         |
         sentence Disjunction sentence
         {
             $$ = new MutableConnectedSentenceNode(
                 BinaryOperatorTypes::Disjunction,
                 std::unique_ptr<IMutableSentenceNode>($1),
                 std::unique_ptr<IMutableSentenceNode>($3)
             );
         }
         |
         sentence Implication sentence
         {
             $$ = new MutableConnectedSentenceNode(
                 BinaryOperatorTypes::Implication,
                 std::unique_ptr<IMutableSentenceNode>($1),
                 std::unique_ptr<IMutableSentenceNode>($3)
             );
         }
         |
         sentence Biconditional sentence
         {
             $$ = new MutableConnectedSentenceNode(
                 BinaryOperatorTypes::Biconditional,
                 std::unique_ptr<IMutableSentenceNode>($1),
                 std::unique_ptr<IMutableSentenceNode>($3)
             );
         }
         |
         LeftParenthesis sentence RightParenthesis
         {
             $$ = $2;
         }
         ;

term_vector :
            term
            {
                $$ = std::vector<std::unique_ptr<IMutableTermNode>>();
                $$.push_back(std::unique_ptr<IMutableTermNode>($1));
            }
            |
            term_vector Comma term
            {
                $$ = std::move($1);
                $$.push_back(std::unique_ptr<IMutableTermNode>($3));
            }
            ;

term :
     Function LeftParenthesis term_vector RightParenthesis
     {
         $$ = new MutableFunctionNode($1, std::move($3));
     }
     |
     Constant
     {
         $$ = new MutableConstantNode($1);
     }
     |
     Variable
     {
         $$ = new MutableVariableNode($1);
     }
     ;

%%

void optifol::impl::BaseParser::error(const std::string& msg)
{
    static_cast<FOLParser *>(this)->error(msg);
}
