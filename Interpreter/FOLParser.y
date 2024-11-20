%{
#include <iostream>
#include <string>
%}

%require "3.7.4"
%language "C++"

%define api.parser.class {FOLParser}
%define api.namespace {optifol}
%define api.value.type variant
%define parse.error detailed
%define parse.lac full
%parse-param {FOLLexer* scanner}

%code requires
{
    #include "AST/VariableASTNode.hpp"
    #include "AST/FunctionASTNode.hpp"
    #include "AST/ConstantASTNode.hpp"

    #include "AST/UnaryFormulaASTNode.hpp"
    #include "AST/BinaryFormulaASTNode.hpp"
    #include "AST/PredicateASTNode.hpp"
    #include "AST/IdentityTermASTNode.hpp"

    namespace optifol
    {
        class FOLLexer;
        extern std::shared_ptr<IFormulaASTNode> yyroot;
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

%type <std::shared_ptr<IFormulaASTNode>> logical_expression
%type <std::shared_ptr<IAtomicASTNode>> atomic_sentence
%type <std::shared_ptr<ITermASTNode>> term
%type <std::vector<std::shared_ptr<ITermASTNode>>> term_vector

%start tmp_start

%%

sentence : quantified_sentence End { return 0; }
         | logical_expression End { return 0; }
         ;

quantified_sentence : Existential Variable sentence
                    | Universal Variable sentence
                    ;

tmp_start : logical_expression End {
              yyroot = $1;
              return 0;
          }

          | error End {
              return -1;
          }
          ;

logical_expression : LeftParenthesis logical_expression RightParenthesis {
                       $$ = $2;
                   }

                   | Negation logical_expression {
                       $$ = std::make_shared<UnaryFormulaASTNode<UnaryFormulaASTTypes::Negation>>($2);
                   }

                   | logical_expression Conjunction logical_expression {
                       $$ = std::make_shared<BinaryFormulaASTNode<BinaryFormulaASTTypes::Conjunction>>($1, $3);
                   }

                   | logical_expression Disjunction logical_expression {
                       $$ = std::make_shared<BinaryFormulaASTNode<BinaryFormulaASTTypes::Disjunction>>($1, $3);
                   }

                   | logical_expression Implication logical_expression {
                       $$ = std::make_shared<BinaryFormulaASTNode<BinaryFormulaASTTypes::Implication>>($1, $3);
                   }

                   | logical_expression Biconditional logical_expression {
                       $$ = std::make_shared<BinaryFormulaASTNode<BinaryFormulaASTTypes::Biconditional>>($1, $3);
                   }

                   | atomic_sentence {
                       $$ = $1;
                   }
                   ;

atomic_sentence : term Identity term {
                    $$ = std::make_shared<IdentityTermASTNode>($1, $3);
                }

                | Predicate LeftParenthesis term_vector RightParenthesis {
                    $$ = std::make_shared<PredicateASTNode>($1, std::move($3));
                }
                ;

term_vector : term {
                $$ = { $1 };
            }

            | term_vector Comma term {
                $$ = std::move($1);
                $$.push_back($3);
            }
            ;

term : Function LeftParenthesis term_vector RightParenthesis {
         $$ = std::make_shared<FunctionASTNode>($1, std::move($3));
     }

     | Constant {
         $$ = std::make_shared<ConstantASTNode>($1);
     }

     | Variable {
         $$ = std::make_shared<VariableASTNode>($1);
     }
     ;

%%

void optifol::FOLParser::error(const std::string& msg)
{
    std::cerr << msg << '\n';
}
