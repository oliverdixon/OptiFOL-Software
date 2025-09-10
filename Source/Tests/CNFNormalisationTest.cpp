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
#include "../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/DisjunctionDistributionVisitor.hpp"
#include "../Visitors/MutableTargets/Sentences/CNFNormalisers/ImplicationEliminationVisitor.hpp"
#include "GoogleTestSupport.hpp"

namespace optifol
{

/**
 * @class CNFNormalisationTest
 * @brief The CNFNormalisationTest Google Test fixture contains tests of the CNF normalisation visitor-based pipeline.
 * @details <p>The seven-stage pipeline is tested:</p>
 *  <ol>
 *      <li>ImplicationEliminationVisitor</li>
 *      <li>DMLVisitor</li>
 *      <li>SymbolStandardisingVisitor</li>
 *      <li>QuantifierExtractingVisitor</li>
 *      <li>SkolemIntroducingVisitor</li>
 *      <li>UniversalEliminationVisitor</li>
 *      <li>DisjunctionDistributionVisitor</li>
 *  </ol>
 */
class CNFNormalisationTest : public testing::Test
{
protected:
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
     */
    template<typename CNFVisitor>
        requires std::derived_from<CNFVisitor, MutatingSentenceVisitorBase>
    static void cnf_test(std::unique_ptr<IMutableSentence> &&test, std::unique_ptr<IMutableSentence> &&expected)
    {
        CNFVisitor visitor;
        test->accept(visitor);
        GoogleTestSupport::test_sentence_equality(*test, *expected);
    }
};

/**
 * @brief Tests basic non-nested functionality of the ImplicationEliminationVisitor for single-operand implications.
 * @details
 *  <ul>
 *      <li>Input: @f$ P \implies Q @f$</li>
 *      <li>Expected output: @f$ \lnot P \lor Q @f$</li>
 *  </ul>
 * @memberof CNFNormalisationTest
 */
TEST_F(CNFNormalisationTest, ImplicationElimination_Basic)
{
    // clang-format off
    cnf_test<ImplicationEliminationVisitor>(
        MutableBinaryConnected::build(
            BinaryOperatorTypes::Implication,
            MutablePredicate::build("P"),
            MutablePredicate::build("Q")
        ),

        MutableBinaryConnected::build(
            BinaryOperatorTypes::Disjunction,
            MutablePredicate::build("P", false),
            MutablePredicate::build("Q")
        )
    );
    // clang-format on
}

/**
 * @brief Tests basic non-nested functionality of the ImplicationEliminationVisitor for dual-operand implications.
 * @details
 *  <ul>
 *      <li>Input: @f$ P \iff Q @f$</li>
 *      <li>Expected output: @f$ \left( P \lor \lnot Q \right) \land \left( \lnot P \lor Q \right) @f$</li>
 *  </ul>
 * @memberof CNFNormalisationTest
 */
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
    // clang-format on
}

/**
 * @brief Tests functionality of the ImplicationEliminationVisitor for nesting on a single operand.
 * @details
 *  <ul>
 *      <li>Input: @f$ \left( P \implies Q \right) \iff R @f$</li>
 *      <li>Expected output: @f$ \left( \lnot \left( \lnot P \lor Q \right) \lor R \right) \land
 *          \left( \left( \lnot P \lor Q \right) \lor \lnot R \right) @f$</li>
 *  </ul>
 * @memberof CNFNormalisationTest
 */
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
    // clang-format on
}

/**
 * @brief Tests functionality of the ImplicationEliminationVisitor for nesting on both operands.
 * @details
 *  <ul>
 *      <li>Input: @f$ \left( P \implies Q \right) \iff \left( R \implies S \right) @f$</li>
 *      <li>
 *          Expected output:
 *          @f$ \left( \lnot \left( \lnot P \lor Q \right) \lor \left( \lnot R \lor S \right) \right) \land
 *          \left( \left( \lnot P \lor Q \right) \lor \lnot \left( \lnot R \lor S \right) \right)@f$
 *      </li>
 *  </ul>
 * @memberof CNFNormalisationTest
 */
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
    // clang-format on
}

/**
 * @brief Tests functionality of the DisjunctionDistributionVisitor for distribution over a single operand.
 * @details
 *  <ul>
 *      <li>Input: @f$ P \lor \left( Q \land R \right) @f$</li>
 *      <li>Expected output: @f$ \left( P \lor Q \right) \land \left( P \lor R \right) @f$</li>
 *  </ul>
 * @memberof CNFNormalisationTest
 */
TEST_F(CNFNormalisationTest, DisjunctionDistribution_BasicUnary)
{
    // clang-format off
    cnf_test<DisjunctionDistributionVisitor>(
        MutableBinaryConnected::build(
            BinaryOperatorTypes::Disjunction,
            MutablePredicate::build("P"),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Conjunction,
                MutablePredicate::build("Q"),
                MutablePredicate::build("R")
            )
        ),

        MutableBinaryConnected::build(
            BinaryOperatorTypes::Conjunction,
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Disjunction,
                MutablePredicate::build("P"),
                MutablePredicate::build("Q")
            ),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Disjunction,
                MutablePredicate::build("P"),
                MutablePredicate::build("R")
            )
        )
    );
    // clang-format on
}

/**
 * @brief Tests functionality of the DisjunctionDistributionVisitor for distribution over both operands.
 * @details
 *  <ul>
 *      <li>Input: @f$ \left( P \lor \left( Q \land R \right) \right) \land
 *          \left( \left( A \land B \right) \lor C \right) @f$</li>
 *      <li>
 *          Expected output:
 *          @f$ \left( \left( P \lor Q \right) \land \left( P \lor R \right) \right) \land
 *          \left( \left( C \lor A \right) \land \left( C \lor B \right) \right) @f$
 *      </li>
 *  </ul>
 * @memberof CNFNormalisationTest
 */
TEST_F(CNFNormalisationTest, DisjunctionDistribution_BasicBinary)
{
    // clang-format off
    cnf_test<DisjunctionDistributionVisitor>(
        MutableBinaryConnected::build(
            BinaryOperatorTypes::Conjunction,
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Disjunction,
                MutablePredicate::build("P"),
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Conjunction,
                    MutablePredicate::build("Q"),
                    MutablePredicate::build("R")
                )
            ),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Disjunction,
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Conjunction,
                    MutablePredicate::build("A"),
                    MutablePredicate::build("B")
                ),
                MutablePredicate::build("C")
            )
        ),

        MutableBinaryConnected::build(
            BinaryOperatorTypes::Conjunction,
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Conjunction,
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicate::build("P"),
                    MutablePredicate::build("Q")
                ),
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicate::build("P"),
                    MutablePredicate::build("R")
                )
            ),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Conjunction,
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicate::build("C"),
                    MutablePredicate::build("A")
                ),
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicate::build("C"),
                    MutablePredicate::build("B")
                )
            )
        )
    );
    // clang-format on
}

/**
 * @brief Tests functionality of the DisjunctionDistributionVisitor for distribution over both operands, where a
 *  reduction is only applicable to one.
 * @details
 *  <ul>
 *      <li>Input: @f$ A \land \left( P \lor \left( Q \land R \right) \right) @f$</li>
 *      <li>Expected output: @f$ A \land \left( \left( P \lor Q \right) \land \left( P \lor R \right) \right) @f$</li>
 *  </ul>
 * @memberof CNFNormalisationTest
 */
TEST_F(CNFNormalisationTest, DisjunctionDistribution_NestedNoOp)
{
    // clang-format off
    cnf_test<DisjunctionDistributionVisitor>(
        MutableBinaryConnected::build(
            BinaryOperatorTypes::Conjunction,
            MutablePredicate::build("A"),
            MutableBinaryConnected::build(
            BinaryOperatorTypes::Disjunction,
            MutablePredicate::build("P"),
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Conjunction,
                    MutablePredicate::build("Q"),
                    MutablePredicate::build("R")
                )
            )
        ),

        MutableBinaryConnected::build(
            BinaryOperatorTypes::Conjunction,
            MutablePredicate::build("A"),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Conjunction,
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicate::build("P"),
                    MutablePredicate::build("Q")
                ),
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicate::build("P"),
                    MutablePredicate::build("R")
                )
            )
        )
    );
    // clang-format on
}

} // namespace optifol
