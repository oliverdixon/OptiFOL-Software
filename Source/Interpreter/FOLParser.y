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
    #include "../IR/Terms/VariableNode.hpp"
    #include "../IR/Terms/FunctionNode.hpp"
    #include "../IR/Terms/ConstantNode.hpp"

    #include "../IR/Sentences/PredicationNode.hpp"
    #include "../IR/Sentences/IdentitySentenceNode.hpp"
    #include "../IR/Sentences/ConnectedSentenceNode.hpp"
    #include "../IR/Sentences/QuantifiedSentenceNode.hpp"
    #include "../IR/Sentences/SentenceRoot.hpp"

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

%type <ISentenceNode *> sentence
%type <ITermNode *> term
%type <std::vector<std::unique_ptr<ITermNode>>> term_vector

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
             SentenceRoot::build(std::unique_ptr<ISentenceNode>($1))
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
             $$ = new QuantifiedSentenceNode(
                 QuantifierTypes::Universal,
                 VariableNode::build($2),
                 std::unique_ptr<ISentenceNode>($4)
             );
         }
         |
         Existential Variable LeftParenthesis sentence RightParenthesis
         {
             $$ = new QuantifiedSentenceNode(
                 QuantifierTypes::Existential,
                 VariableNode::build($2),
                 std::unique_ptr<ISentenceNode>($4)
             );
         }
         |
         Predicate LeftParenthesis term_vector RightParenthesis
         {
             $$ = new PredicationNode($1, std::move($3));
         }
         |
         term Identity term
         {
             $$ = new IdentitySentenceNode(std::unique_ptr<ITermNode>($1), std::unique_ptr<ITermNode>($3));
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
             $$ = new ConnectedSentenceNode(
                 BinaryOperatorTypes::Conjunction,
                 std::unique_ptr<ISentenceNode>($1),
                 std::unique_ptr<ISentenceNode>($3)
             );
         }
         |
         sentence Disjunction sentence
         {
             $$ = new ConnectedSentenceNode(
                 BinaryOperatorTypes::Disjunction,
                 std::unique_ptr<ISentenceNode>($1),
                 std::unique_ptr<ISentenceNode>($3)
             );
         }
         |
         sentence Implication sentence
         {
             $$ = new ConnectedSentenceNode(
                 BinaryOperatorTypes::Implication,
                 std::unique_ptr<ISentenceNode>($1),
                 std::unique_ptr<ISentenceNode>($3)
             );
         }
         |
         sentence Biconditional sentence
         {
             $$ = new ConnectedSentenceNode(
                 BinaryOperatorTypes::Biconditional,
                 std::unique_ptr<ISentenceNode>($1),
                 std::unique_ptr<ISentenceNode>($3)
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
                $$ = std::vector<std::unique_ptr<ITermNode>>();
                $$.push_back(std::unique_ptr<ITermNode>($1));
            }
            |
            term_vector Comma term
            {
                $$ = std::move($1);
                $$.push_back(std::unique_ptr<ITermNode>($3));
            }
            ;

term :
     Function LeftParenthesis term_vector RightParenthesis
     {
         $$ = new FunctionNode($1, std::move($3));
     }
     |
     Constant
     {
         $$ = new ConstantNode($1);
     }
     |
     Variable
     {
         $$ = new VariableNode($1);
     }
     ;

%%

void optifol::impl::BaseParser::error(const std::string& msg)
{
    static_cast<FOLParser *>(this)->error(msg);
}
