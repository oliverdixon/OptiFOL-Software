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

    #include "../IR/Sentences/NegatedSentenceNode.hpp"
    #include "../IR/Sentences/PredicationNode.hpp"
    #include "../IR/Sentences/IdentitySentenceNode.hpp"
    #include "../IR/Sentences/ConnectedSentenceNode.hpp"
    #include "../IR/Sentences/QuantifiedSentenceNode.hpp"

    #include "../IR/Sentences/NodeProxy.hpp"

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

%type <std::unique_ptr<ISentenceNode>> sentence
%type <std::unique_ptr<ITermNode>> term
%type <std::vector<std::unique_ptr<ITermNode>>> term_vector

%start line

%%

line :
     sentence End
     {
         static_cast<FOLParser *>(this)->register_sentence($1);
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
             $$ = std::make_unique<NodeProxy>(
                 std::make_unique<QuantifiedSentenceNode>(
                     QuantifierTypes::Universal,
                     std::make_unique<VariableNode>($2),
                     $4
                 )
             );
         }
         |
         Existential Variable LeftParenthesis sentence RightParenthesis
         {
             $$ = std::make_unique<NodeProxy>(
                 std::make_unique<QuantifiedSentenceNode>(
                     QuantifierTypes::Existential,
                     std::make_unique<VariableNode>($2),
                     $4
                 )
             );
         }
         |
         Predicate LeftParenthesis term_vector RightParenthesis
         {
             $$ = std::make_unique<PredicationNode>($1, std::move($3));
         }
         |
         term Identity term
         {
             $$ = std::make_unique<IdentitySentenceNode>($1, $3);
         }
         |
         Negation sentence
         {
             $$ = std::make_unique<NodeProxy>(
                 std::make_unique<NegatedSentenceNode>(
                     $2
                 )
             );
         }
         |
         sentence Conjunction sentence
         {
             $$ = std::make_unique<ConnectedSentenceNode>(
                 BinaryOperatorTypes::Conjunction,
                 $1,
                 $3
             );
         }
         |
         sentence Disjunction sentence
         {
             $$ = std::make_unique<ConnectedSentenceNode>(
                 BinaryOperatorTypes::Disjunction,
                 $1,
                 $3
             );
         }
         |
         sentence Implication sentence
         {
             $$ = std::make_unique<ConnectedSentenceNode>(
                 BinaryOperatorTypes::Implication,
                 $1,
                 $3
             );
         }
         |
         sentence Biconditional sentence
         {
             $$ = std::make_unique<ConnectedSentenceNode>(
                 BinaryOperatorTypes::Biconditional,
                 $1,
                 $3
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
                $$ = { $1 };
            }
            |
            term_vector Comma term
            {
                $$ = std::move($1);
                $$.push_back($3);
            }
            ;

term :
     Function LeftParenthesis term_vector RightParenthesis
     {
         $$ = std::make_unique<FunctionNode>($1, std::move($3));
     }
     |
     Constant
     {
         $$ = std::make_unique<ConstantNode>($1);
     }
     |
     Variable
     {
         $$ = std::make_unique<VariableNode>($1);
     }
     ;

%%

void optifol::impl::BaseParser::error(const std::string& msg)
{
    static_cast<FOLParser *>(this)->error(msg);
}
