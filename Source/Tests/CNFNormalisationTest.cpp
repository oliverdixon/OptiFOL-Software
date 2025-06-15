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

#include "../IR/MutableVariants/Sentences/MutableBinaryConnected.hpp"
#include "../IR/MutableVariants/Sentences/MutablePredicate.hpp"
#include "../IR/MutableVariants/Terms/IMutableTerm.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/ImplicationEliminationVisitor.hpp"
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
    static void cnf_test(std::unique_ptr<IMutableSentence>&& test, std::unique_ptr<IMutableSentence>&& expected)
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
        MutableBinaryConnected::build(
            BinaryOperatorTypes::Implication,
            MutablePredicate::build("P"),
            MutablePredicate::build("A")
        ),

        MutableBinaryConnected::build(
            BinaryOperatorTypes::Disjunction,
            MutablePredicate::build("P", false),
            MutablePredicate::build("Q")
        )
    );
}

TEST_F(CNFNormalisationTest, ImplicationElimination_Biconditional)
{
    // clang-format off
    cnf_test<ImplicationEliminationVisitor>(
        MutableBinaryConnected::build(
            BinaryOperatorTypes::Biconditional,
            MutablePredicate::build("P"),
            MutablePredicate::build("Q")
        ),

        MutableBinaryConnected::build(
            BinaryOperatorTypes::Conjunction,
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Disjunction,
                MutablePredicate::build("P"),
                MutablePredicate::build("Q", false)
            ),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Disjunction,
                MutablePredicate::build("P", false),
                MutablePredicate::build("Q")
            )
        )
    );
}

TEST_F(CNFNormalisationTest, ImplicationElimination_UnaryNesting)
{
    // clang-format off
    cnf_test<ImplicationEliminationVisitor>(
        MutableBinaryConnected::build(
            BinaryOperatorTypes::Biconditional,
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Implication,
                MutablePredicate::build("P"),
                MutablePredicate::build("Q")
            ),
            MutablePredicate::build("R")
        ),

        MutableBinaryConnected::build(
            BinaryOperatorTypes::Conjunction,
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Disjunction,
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicate::build("P", false),
                    MutablePredicate::build("Q"),
                    false
                ),
                MutablePredicate::build("R")
            ),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Disjunction,
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicate::build("P", false),
                    MutablePredicate::build("Q")
                ),
                MutablePredicate::build("R", false)
            )
        )
    );
}

TEST_F(CNFNormalisationTest, ImplicationElimination_BinaryNesting)
{
    // clang-format off
    cnf_test<ImplicationEliminationVisitor>(
        MutableBinaryConnected::build(
            BinaryOperatorTypes::Biconditional,
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Implication,
                MutablePredicate::build("P"),
                MutablePredicate::build("Q")
            ),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Implication,
                MutablePredicate::build("R"),
                MutablePredicate::build("S")
            )
        ),

        MutableBinaryConnected::build(
            BinaryOperatorTypes::Conjunction,
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Disjunction,
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicate::build("P", false),
                    MutablePredicate::build("Q"),
                    false
                ),
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicate::build("R", false),
                    MutablePredicate::build("S")
                )
            ),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Disjunction,
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicate::build("P", false),
                    MutablePredicate::build("Q")
                ),
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicate::build("R", false),
                    MutablePredicate::build("S"),
                    false
                )
            )
        )
    );
}

}
