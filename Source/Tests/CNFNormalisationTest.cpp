/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Test normalisation of FOL sentences into Conjunctive Normal Form
 * @author Oliver Dixon
 * @date 2025-06-08
 * @version Development
 */

#include <gtest/gtest.h>

#include "../IR/Sentences/ConnectedSentenceNode.hpp"
#include "../IR/Sentences/PredicationNode.hpp"
#include "../Visitors/Sentences/CNFNormalisers/ImplicationEliminationVisitor.hpp"
#include "GoogleTestSupport.hpp"

namespace optifol
{

class CNFNormalisationTest:
        public testing::Test
{
public:
    /**
     * @brief Despatches the templated CNF visitor on the given test node and verifies that the CNF-normalised result
     *  matches the expected sentence construction.
     * @tparam CNFVisitor The CNF Visitor type to instantiate and despatch on the sentence
     * @param test The sentence on which the CNF Visitor should be tested
     * @param expected The expected sentence following transformation by the CNF Visitor
     * @note It is not necessary nor forbidden to wrap nodes in any level of SentenceRoot objects.
     * @note The equality functor is hash-based; in particular, a commutative hash-combining function is used for
     *  binary-operand sentences. Thus the expected sentence need not pass operands to commutative operators in the same
     *  order as would be produced by the CNF Visitor.
     * @warning If the verification fails, a Google Test assertion failure is raised.
     */
    template<typename CNFVisitor> requires std::derived_from<CNFVisitor, MutatingSentenceVisitorBase>
    static void cnf_test(std::unique_ptr<ISentenceNode>&& test, std::unique_ptr<ISentenceNode>&& expected)
    {
        CNFVisitor visitor;
        test->accept(visitor);
        GoogleTestSupport::test_sentence_equality(*test, *expected);
    }
};

TEST_F(CNFNormalisationTest, ImplicationElimination_Basic)
{
    // clang-format off
    cnf_test<ImplicationEliminationVisitor>(
        ConnectedSentenceNode::build(
            BinaryOperatorTypes::Implication,
            PredicationNode::build("P"),
            PredicationNode::build("Q")
        ),

        ConnectedSentenceNode::build(
            BinaryOperatorTypes::Disjunction,
            PredicationNode::build("P", false),
            PredicationNode::build("Q")
        )
    );
}

TEST_F(CNFNormalisationTest, ImplicationElimination_Biconditional)
{
    // clang-format off
    cnf_test<ImplicationEliminationVisitor>(
        ConnectedSentenceNode::build(
            BinaryOperatorTypes::Biconditional,
            PredicationNode::build("P"),
            PredicationNode::build("Q")
        ),

        ConnectedSentenceNode::build(
            BinaryOperatorTypes::Conjunction,
            ConnectedSentenceNode::build(
                BinaryOperatorTypes::Disjunction,
                PredicationNode::build("P"),
                PredicationNode::build("Q", false)
            ),
            ConnectedSentenceNode::build(
                BinaryOperatorTypes::Disjunction,
                PredicationNode::build("P", false),
                PredicationNode::build("Q")
            )
        )
    );
}

TEST_F(CNFNormalisationTest, ImplicationElimination_UnaryNesting)
{
    // clang-format off
    cnf_test<ImplicationEliminationVisitor>(
        ConnectedSentenceNode::build(
            BinaryOperatorTypes::Biconditional,
            ConnectedSentenceNode::build(
                BinaryOperatorTypes::Implication,
                PredicationNode::build("P"),
                PredicationNode::build("Q")
            ),
            PredicationNode::build("R")
        ),

        ConnectedSentenceNode::build(
            BinaryOperatorTypes::Conjunction,
            ConnectedSentenceNode::build(
                BinaryOperatorTypes::Disjunction,
                ConnectedSentenceNode::build(
                    BinaryOperatorTypes::Disjunction,
                    PredicationNode::build("P", false),
                    PredicationNode::build("Q"),
                    false
                ),
                PredicationNode::build("R")
            ),
            ConnectedSentenceNode::build(
                BinaryOperatorTypes::Disjunction,
                ConnectedSentenceNode::build(
                    BinaryOperatorTypes::Disjunction,
                    PredicationNode::build("P", false),
                    PredicationNode::build("Q")
                ),
                PredicationNode::build("R", false)
            )
        )
    );
}

TEST_F(CNFNormalisationTest, ImplicationElimination_BinaryNesting)
{
    // clang-format off
    cnf_test<ImplicationEliminationVisitor>(
        ConnectedSentenceNode::build(
            BinaryOperatorTypes::Biconditional,
            ConnectedSentenceNode::build(
                BinaryOperatorTypes::Implication,
                PredicationNode::build("P"),
                PredicationNode::build("Q")
            ),
            ConnectedSentenceNode::build(
                BinaryOperatorTypes::Implication,
                PredicationNode::build("R"),
                PredicationNode::build("S")
            )
        ),

        ConnectedSentenceNode::build(
            BinaryOperatorTypes::Conjunction,
            ConnectedSentenceNode::build(
                BinaryOperatorTypes::Disjunction,
                ConnectedSentenceNode::build(
                    BinaryOperatorTypes::Disjunction,
                    PredicationNode::build("P", false),
                    PredicationNode::build("Q"),
                    false
                ),
                ConnectedSentenceNode::build(
                    BinaryOperatorTypes::Disjunction,
                    PredicationNode::build("R", false),
                    PredicationNode::build("S")
                )
            ),
            ConnectedSentenceNode::build(
                BinaryOperatorTypes::Disjunction,
                ConnectedSentenceNode::build(
                    BinaryOperatorTypes::Disjunction,
                    PredicationNode::build("P", false),
                    PredicationNode::build("Q")
                ),
                ConnectedSentenceNode::build(
                    BinaryOperatorTypes::Disjunction,
                    PredicationNode::build("R", false),
                    PredicationNode::build("S"),
                    false
                )
            )
        )
    );
}

}
