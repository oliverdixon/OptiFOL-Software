/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Test registration of CNF-normalised FOL sentences into a centralised SymbolRepository
 * @author Oliver Dixon
 * @date 2025-09-10
 * @version Development
 */

#include <gtest/gtest.h>

#include "../IR/MutableVariants/Sentences/IMutableSentence.hpp"
#include "../IR/MutableVariants/Sentences/MutableBinaryConnected.hpp"
#include "../IR/MutableVariants/Sentences/MutablePredicate.hpp"
#include "../IR/MutableVariants/Sentences/MutableSentenceRoot.hpp"
#include "../IR/MutableVariants/Terms/MutableVariable.hpp"
#include "../IR/Sentences/Literal.hpp"
#include "../IR/SymbolRepository.hpp"
#include "../IR/Terms/Variable.hpp"
#include "../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"

namespace optifol
{

/**
 * @class RepositoryBuildingTest
 * @brief The RepositoryBuildingTest Google Test fixture contains tests of the RepositoryBuildingVisitor.
 */
class RepositoryBuildingTest : public testing::Test
{
protected:
    /**
     * @brief Despatches the RepositoryBuildingVisitor with a fresh SymbolRepository over the given IMutableSentence and
     *  compares the actual results to the expected.
     * @param test The initial node to test, not wrapped inside of a MutableSentenceRoot.
     * @param expected The expected set of ordered clauses, each of which contains the expected set of ordered literals.
     */
    static void repo_build_test(
            std::unique_ptr<IMutableSentence> &&test, const std::vector<std::vector<const Literal *>> &expected)
    {
        const auto repository = std::make_shared<SymbolRepository>();
        RepositoryBuildingVisitor visitor(repository);
        const auto wrapped_test_input = std::make_unique<MutableSentenceRoot>(std::move(test));

        wrapped_test_input->accept(visitor);
        const auto transformed_root = visitor.take_last_root();

        auto actual_clause_begin = transformed_root->cbegin();
        const auto actual_clause_end = transformed_root->cend();

        for (const auto &expected_clause: expected) {
            // Verify that the actual clause list does not exceed the expected clause list.
            EXPECT_NE(actual_clause_begin, actual_clause_end);

            auto actual_literal_begin = actual_clause_begin->cbegin();
            const auto actual_literal_end = actual_clause_begin->cend();

            for (const auto expected_literal: expected_clause) {
                // Verify that the actual literal list does not exceed the expected literal list for the fixed clause.
                EXPECT_NE(actual_literal_begin, actual_literal_end);

                // Verify equality of the Literals.
                EXPECT_EQ(**actual_literal_begin, *expected_literal);

                // Verify insertion into repository.
                EXPECT_TRUE(repository->get_symbol_handle(*expected_literal).has_value());

                ++actual_literal_begin;
            }

            // Verify that the actual literal list is exhausted at the same point as the expected literal list.
            EXPECT_EQ(actual_literal_begin, actual_literal_end);
            ++actual_clause_begin;
        }

        // Verify that the actual clause list is exhausted at the same point as the expected clause list.
        EXPECT_EQ(actual_clause_begin, actual_clause_end);
    }
};

/**
 * @brief Tests basic functionality of the RepositoryBuildingVisitor for a single predicate with arguments.
 * @details
 *  <ul>
 *      <li>Input: @f$ P \left( x, y, z \right) @f$</li>
 *      <li>Expected output: @f$ \left\{ \left\{ P \left( x, y, z \right) \right\} \right\} @f$</li>
 *  </ul>
 * @memberof RepositoryBuildingTest
 */
TEST_F(RepositoryBuildingTest, SingleClause_SinglePredicate)
{
    // clang-format off

    std::vector<std::unique_ptr<IMutableTerm>> p_args;
    p_args.push_back(MutableVariable::build<IMutableTerm>("x"));
    p_args.push_back(MutableVariable::build<IMutableTerm>("y"));
    p_args.push_back(MutableVariable::build<IMutableTerm>("z"));

    const Variable x("x");
    const Variable y("y");
    const Variable z("z");
    const Literal p("P", { &x, &y, &z });

    repo_build_test(
        MutablePredicate::build("P", std::move(p_args)),
        { { &p } }
    );

    // clang-format on
}

/**
 * @brief Tests basic functionality of the RepositoryBuildingVisitor for multiple predicates within a single CNF clause.
 * @details
 *  <ul>
 *      <li>Input: @f$ P \lor Q @f$</li>
 *      <li>Expected output: @f$ \left\{ \left\{ P, Q \right\} \right\} @f$</li>
 *  </ul>
 * @memberof RepositoryBuildingTest
 */
TEST_F(RepositoryBuildingTest, SingleClause_MultiplePredicates)
{
    // clang-format off

    const Literal p("P");
    const Literal q("Q");

    repo_build_test(
        MutableBinaryConnected::build(
            BinaryOperatorTypes::Disjunction,
            MutablePredicate::build("P"),
            MutablePredicate::build("Q")
        ),

        { { &p, &q } }
    );

    // clang-format on
}

/**
 * @brief Tests basic functionality of the RepositoryBuildingVisitor for single predicates over multiple CNF clauses.
 * @details
 *  <ul>
 *      <li>Input: @f$ P \land Q @f$</li>
 *      <li>Expected output: @f$ \left\{ \left\{ P \right\}, \left\{ Q \right\} \right\} @f$</li>
 *  </ul>
 * @memberof RepositoryBuildingTest
 */
TEST_F(RepositoryBuildingTest, MultipleClauses_SinglePredicate)
{
    // clang-format off

    const Literal p("P");
    const Literal q("Q");

    repo_build_test(
        MutableBinaryConnected::build(
            BinaryOperatorTypes::Conjunction,
            MutablePredicate::build("P"),
            MutablePredicate::build("Q")
        ),

        { { &p }, { &q } }
    );

    // clang-format on
}

/**
 * @brief Tests basic functionality of the RepositoryBuildingVisitor for multiple predicates over multiple CNF clauses.
 * @details
 *  <ul>
 *      <li>Input: @f$ \left( P \lor Q \right) \land \left( R \lor S \right) @f$</li>
 *      <li>Expected output: @f$ \left\{ \left\{ P, Q \right\}, \left\{ R, S \right\} \right\} @f$</li>
 *  </ul>
 * @memberof RepositoryBuildingTest
 */
TEST_F(RepositoryBuildingTest, MultipleClauses_MultiplePredicates)
{
    // clang-format off

    const Literal p("P");
    const Literal q("Q");
    const Literal r("R");
    const Literal s("S");

    repo_build_test(
        MutableBinaryConnected::build(
            BinaryOperatorTypes::Conjunction,
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Disjunction,
                MutablePredicate::build("P"),
                MutablePredicate::build("Q")
            ),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Disjunction,
                MutablePredicate::build("R"),
                MutablePredicate::build("S")
            )
        ),

        { { &p, &q }, { &r, &s } }
    );

    // clang-format on
}

/**
 * @brief Tests basic functionality of the RepositoryBuildingVisitor for multiple predicates over multiple CNF clauses,
 *  including nested CNF formulas.
 * @details
 *  <ul>
 *      <li>
 *          Input:
 *          @f$ \left( \left( A \lor B \right) \lor C \right) \land
 *          \left( D \land \left( E \land \left( F \lor G \right) \right) \right) @f$
 *      </li>
 *      <li>
 *          Expected output:
 *          @f$\left\{
 *              \left\{ A, B, C \right\},
 *              \left\{ D \right\},
 *              \left\{ E \right\},
 *              \left\{ F, G \right\}
 *          \right\}@f$
 *      </li>
 *  </ul>
 * @memberof RepositoryBuildingTest
 */
TEST_F(RepositoryBuildingTest, Nested)
{
    // clang-format off

    const Literal a("A");
    const Literal b("B");
    const Literal c("C");
    const Literal d("D");
    const Literal e("E");
    const Literal f("F");
    const Literal g("G");

    repo_build_test(
        MutableBinaryConnected::build(
            BinaryOperatorTypes::Conjunction,
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Disjunction,
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Disjunction,
                    MutablePredicate::build("A"),
                    MutablePredicate::build("B")
                ),
                MutablePredicate::build("C")
            ),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Conjunction,
                MutablePredicate::build("D"),
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Conjunction,
                    MutablePredicate::build("E"),
                    MutableBinaryConnected::build(
                        BinaryOperatorTypes::Disjunction,
                        MutablePredicate::build("F"),
                        MutablePredicate::build("G")
                    )
                )
            )
        ),

        {
            { &a, &b, &c },
            { &d },
            { &e },
            { &f, &g }
        }
    );

    // clang-format on
}

} // namespace optifol
