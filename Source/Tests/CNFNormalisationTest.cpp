/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Test normalisation of FOL sentences into Conjunctive Normal Form
 * @author Oliver Dixon
 * @date 2025-07-08
 * @version Development
 */

#include <gtest/gtest.h>

#include "../IR/Sentences/ConnectedSentenceNode.hpp"
#include "../IR/Sentences/PredicationNode.hpp"
#include "../IR/Terms/ITermNode.hpp"
#include "../Visitors/Sentences/CNFNormalisers/ImplicationEliminationVisitor.hpp"

namespace optifol
{

template<typename T>
concept GoogleTestable = requires(const T& lhs, const T& rhs, std::ostream& ostream)
{
    { lhs == rhs } -> std::convertible_to<bool>;
    { ostream << lhs } -> std::same_as<std::ostream&>;
};

class CNFNormalisationTest:
        public testing::Test
{
public:
    template<typename CNFVisitor> requires std::derived_from<CNFVisitor, MutatingSentenceVisitorBase>
    static void cnf_test(std::unique_ptr<ISentenceNode>&& test, std::unique_ptr<ISentenceNode>&& expected)
    {
        CNFVisitor visitor;
        test->accept(visitor);
        test_sentence_equality(*test, *expected);
    }

private:
    template<GoogleTestable ReceivedType, GoogleTestable ExpectedType>
    static void test_sentence_equality(const ReceivedType& received, const ExpectedType& expected)
    {
        EXPECT_EQ(received, expected);
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
