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

#include "../IR/Mutable/Sentences/MutableConnectedSentenceNode.hpp"
#include "../IR/Mutable/Sentences/MutablePredicationNode.hpp"
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
     * @note It is not necessary nor forbidden to wrap nodes in any level of MutableSentenceRoot objects.
     * @note The equality functor is hash-based; in particular, a commutative hash-combining function is used for
     *  binary-operand sentences. Thus the expected sentence need not pass operands to commutative operators in the same
     *  order as would be produced by the CNF Visitor.
     * @warning If the verification fails, a Google Test assertion failure is raised.
     */
    template<typename CNFVisitor> requires std::derived_from<CNFVisitor, MutatingSentenceVisitorBase>
    static void cnf_test(std::unique_ptr<IMutableSentenceNode>&& test, std::unique_ptr<IMutableSentenceNode>&& expected)
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
        MutableConnectedSentenceNode::build(
            BinaryOperatorTypes::Implication,
            MutablePredicationNode::build("P"),
            MutablePredicationNode::build("Q")
        ),

        MutableConnectedSentenceNode::build(
            BinaryOperatorTypes::Disjunction,
            MutablePredicationNode::build("P", false),
            MutablePredicationNode::build("Q")
        )
    );
}

TEST_F(CNFNormalisationTest, ImplicationElimination_Biconditional)
{
    // clang-format off
    cnf_test<ImplicationEliminationVisitor>(
        MutableConnectedSentenceNode::build(
            BinaryOperatorTypes::Biconditional,
            MutablePredicationNode::build("P"),
            MutablePredicationNode::build("Q")
        ),

        MutableConnectedSentenceNode::build(
            BinaryOperatorTypes::Conjunction,
            MutableConnectedSentenceNode::build(
                BinaryOperatorTypes::Disjunction,
                MutablePredicationNode::build("P"),
                MutablePredicationNode::build("Q", false)
            ),
            MutableConnectedSentenceNode::build(
                BinaryOperatorTypes::Disjunction,
                MutablePredicationNode::build("P", false),
                MutablePredicationNode::build("Q")
            )
        )
    );
}

TEST_F(CNFNormalisationTest, ImplicationElimination_UnaryNesting)
{
    // clang-format off
    cnf_test<ImplicationEliminationVisitor>(
        MutableConnectedSentenceNode::build(
            BinaryOperatorTypes::Biconditional,
            MutableConnectedSentenceNode::build(
                BinaryOperatorTypes::Implication,
                MutablePredicationNode::build("P"),
                MutablePredicationNode::build("Q")
            ),
            MutablePredicationNode::build("R")
        ),

        MutableConnectedSentenceNode::build(
            BinaryOperatorTypes::Conjunction,
            MutableConnectedSentenceNode::build(
                BinaryOperatorTypes::Disjunction,
                MutableConnectedSentenceNode::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicationNode::build("P", false),
                    MutablePredicationNode::build("Q"),
                    false
                ),
                MutablePredicationNode::build("R")
            ),
            MutableConnectedSentenceNode::build(
                BinaryOperatorTypes::Disjunction,
                MutableConnectedSentenceNode::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicationNode::build("P", false),
                    MutablePredicationNode::build("Q")
                ),
                MutablePredicationNode::build("R", false)
            )
        )
    );
}

TEST_F(CNFNormalisationTest, ImplicationElimination_BinaryNesting)
{
    // clang-format off
    cnf_test<ImplicationEliminationVisitor>(
        MutableConnectedSentenceNode::build(
            BinaryOperatorTypes::Biconditional,
            MutableConnectedSentenceNode::build(
                BinaryOperatorTypes::Implication,
                MutablePredicationNode::build("P"),
                MutablePredicationNode::build("Q")
            ),
            MutableConnectedSentenceNode::build(
                BinaryOperatorTypes::Implication,
                MutablePredicationNode::build("R"),
                MutablePredicationNode::build("S")
            )
        ),

        MutableConnectedSentenceNode::build(
            BinaryOperatorTypes::Conjunction,
            MutableConnectedSentenceNode::build(
                BinaryOperatorTypes::Disjunction,
                MutableConnectedSentenceNode::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicationNode::build("P", false),
                    MutablePredicationNode::build("Q"),
                    false
                ),
                MutableConnectedSentenceNode::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicationNode::build("R", false),
                    MutablePredicationNode::build("S")
                )
            ),
            MutableConnectedSentenceNode::build(
                BinaryOperatorTypes::Disjunction,
                MutableConnectedSentenceNode::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicationNode::build("P", false),
                    MutablePredicationNode::build("Q")
                ),
                MutableConnectedSentenceNode::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicationNode::build("R", false),
                    MutablePredicationNode::build("S"),
                    false
                )
            )
        )
    );
}

}
