/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Test organisation of Literal objects into Clause nodes and SentenceRoot nodes
 * @author Oliver Dixon
 * @date 2026-01-25
 * @version Development
 */

#include <algorithm>
#include <gtest/gtest.h>
#include <random>
#include <ranges>

#include "../IR/Sentences/Clause.hpp"
#include "../IR/Sentences/SentenceRoot.hpp"

namespace optifol
{

/**
 * @class LiteralOrganisationTest
 * @brief Verifies API characteristics of Literal, Clause, and SentenceRoot structures relating to reductions for
 *  tautologies and bottoms, and handles duplicates.
 */
class LiteralOrganisationTest : public testing::Test
{
protected:
    /**
     * @brief Randomly generate a fixed number of non-complementary Literal objects, owned by the returned container,
     *  and inserted into the given Clause.
     * @param clause_size The number of Literal objects to generate.
     * @param target The Clause into which the Literal objects should be registered.
     * @return The owning containers of the constructed Literal objects.
     */
    static std::vector<std::unique_ptr<Literal>> generate_literals(const unsigned int clause_size, Clause& target)
    {
        // Build the literals and hold ownership.
        std::vector<std::unique_ptr<Literal>> owning_container(clause_size);
        unsigned int literal_idx = 0;
        std::ranges::generate(owning_container,
            [&literal_idx] { return Literal::build(std::to_string(++literal_idx)); });

        // Provide weak references (through a C++23 projection) for each owning container to the given clause.
        std::ranges::for_each(owning_container,
            [&target](const Literal * const literal) { target.add_literal(literal); },
            [](const std::unique_ptr<Literal>& literal) { return literal.get(); }
        );

        return owning_container;
    }

    /**
     * @brief Generate a fixed number of non-equal Clause objects, owned by the returned container, and inserted into
     *  the given SentenceRoot.
     * @param clause_count The number of Clauses to insert into the SentenceRoot.
     * @param target The SentenceRoot into which the Clause objects should be registered.
     * @return The owning containers of the constructed Literal objects.
     */
    static std::vector<std::unique_ptr<Literal>> generate_clauses(const unsigned int clause_count, SentenceRoot& target)
    {
        std::vector<std::unique_ptr<Literal>> literals_owner;

        // Set up a generator to produce clauses with increasing numbers of literals.
        auto clauses_view =
            std::views::iota(0U) |
            std::views::transform([&literals_owner](const auto idx) -> Clause
            {
                Clause clause;
                auto new_literals_owner = generate_literals(idx, clause);
                literals_owner.insert(literals_owner.end(),
                    std::make_move_iterator(new_literals_owner.begin()),
                    std::make_move_iterator(new_literals_owner.end()));
                return clause;
            });

        // Sample the fixed number of clauses for the SentenceRoot.
        for (auto clause : clauses_view | std::views::take(clause_count))
            target.add_clause(std::move(clause));

        return literals_owner;
    }
};

/**
 * @brief Verify that a Clause can accept a single pair of complementary Literal objects and reduce to a tautology.
 * @memberof LiteralOrganisationTest
 */
TEST_F(LiteralOrganisationTest, ClauseTriviallyTrue_Simple)
{
    const auto p = Literal::build("P");
    const auto p_complement = Literal::build("P", std::initializer_list<const IProcessedTerm *>{}, false);

    const Clause clause{p.get(), p_complement.get()};

    EXPECT_EQ(clause.get_triviality_state(), Clause::State::TriviallyTrue);
    EXPECT_EQ(clause.order(), 0U);
}

/**
 * @brief Verify that a Clause can accept a large number of Literal objects, in any order, and detect tautologies.
 * @memberof LiteralOrganisationTest
 */
TEST_F(LiteralOrganisationTest, ClauseTriviallyTrue_Complex)
{
    static constexpr unsigned int clause_size = 64;
    Clause clause;
    auto literals = generate_literals(clause_size, clause);

    /*
     * Randomly generate and add the complement of one existing literal, and verify that it pulls the clause into a
     * tautology.
     */
    std::random_device random_device;
    std::mt19937 rng(random_device());
    std::uniform_int_distribution<> dist(1, clause_size);

    literals.emplace_back(Literal::build(std::to_string(dist(rng)),
        std::initializer_list<const IProcessedTerm *>{}, false));
    clause.add_literal(literals.back().get());

    EXPECT_EQ(clause.get_triviality_state(), Clause::State::TriviallyTrue);
    EXPECT_EQ(clause.order(), 0U);
}

/**
 * @brief Verify that an empty Clause is correctly marked as trivially false.
 * @memberof LiteralOrganisationTest
 */
TEST_F(LiteralOrganisationTest, ClauseTriviallyFalse)
{
    const Clause clause{};

    EXPECT_EQ(clause.get_triviality_state(), Clause::State::TriviallyFalse);
    EXPECT_EQ(clause.order(), 0U);
}

/**
 * @brief Verify that a Clause with no complementary pairs is correctly marked as non-trivial and accepts all Literal
 *  objects.
 * @memberof LiteralOrganisationTest
 */
TEST_F(LiteralOrganisationTest, ClauseNonTrivial_Simple)
{
    static constexpr unsigned int clause_size = 32;
    Clause clause;

    [[maybe_unused]] auto literals = generate_literals(clause_size, clause);

    EXPECT_EQ(clause.get_triviality_state(), Clause::State::NotTrivial);
    EXPECT_EQ(clause.order(), clause_size);
}

/**
 * @brief Verify that a Clause with no complementary pairs and duplicates is correctly marked as non-trivial and denies
 *  only non-unique Literal objects.
 * @memberof LiteralOrganisationTest
 */
TEST_F(LiteralOrganisationTest, ClauseNonTrivial_Duplicates)
{
    static constexpr unsigned int group_size = 8;
    Clause clause;

    [[maybe_unused]] auto literals_group_1 = generate_literals(group_size, clause);
    EXPECT_EQ(clause.order(), group_size);

    [[maybe_unused]] auto literals_group_2 = generate_literals(group_size, clause);

    // Crucially, check that the literal duplicates did not increase the order of the clause.
    EXPECT_EQ(clause.order(), group_size);
}

/**
 * @brief Verify that a SentenceRoot with distinct Clause objects accepts them all.
 * @memberof LiteralOrganisationTest
 */
TEST_F(LiteralOrganisationTest, SentenceRoot_Simple)
{
    static constexpr unsigned int clause_count = 8;
    SentenceRoot root;

    [[maybe_unused]] auto literals_owner = generate_clauses(clause_count, root);

    EXPECT_TRUE(root.order() == clause_count);
}

/**
 * @brief Verify that a SentenceRoot refuses duplicate Clause objects.
 * @memberof LiteralOrganisationTest
 */
TEST_F(LiteralOrganisationTest, SentenceRoot_Duplicates)
{
    static constexpr unsigned int group_size = 8;
    SentenceRoot root;

    [[maybe_unused]] auto literals_group_1 = generate_clauses(group_size, root);
    EXPECT_EQ(root.order(), group_size);

    [[maybe_unused]] auto literals_group_2 = generate_clauses(group_size, root);

    // Crucially, check that the clause duplicates did not increase the order of the SentenceRoot.
    EXPECT_EQ(root.order(), group_size);
}

} // namespace optifol
