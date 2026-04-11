/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Test the Feature Vector Indexing on Clauses
 * @author Oliver Dixon
 * @date 2026-01-30
 * @version Development
 */

#include <gtest/gtest.h>

#include "../IR/Sentences/Clause.hpp"
#include "../IR/Sentences/Literal.hpp"
#include "../IR/SymbolRepository.hpp"
#include "../IR/Terms/Function.hpp"
#include "../Inference/FVIKnowledgeBase.hpp"

namespace optifol
{

class FeatureVectorIndexTest : public testing::Test
{
protected:
    void SetUp() override
    {
        symbol_repository = std::make_shared<SymbolRepository>();
    }

    std::shared_ptr<SymbolRepository> symbol_repository;
};

/**
 * @brief
 * @details
 * @memberof FeatureVectorIndexTest
 */
TEST_F(FeatureVectorIndexTest, Test1)
{
    const auto p_literal = symbol_repository->add_symbol(Literal::build("P"));
    const auto q_literal = symbol_repository->add_symbol(Literal::build("Q"));
    const auto r_literal = symbol_repository->add_symbol(Literal::build("R"));

    auto c1 = Clause::build();
    c1->add_literal(p_literal);
    c1->add_literal(q_literal);

    auto c2 = Clause::build();
    c2->add_literal(p_literal);
    c2->add_literal(r_literal);

    FVIKnowledgeBase kb(symbol_repository);
    kb.add_clause(std::move(c1));
    kb.add_clause(std::move(c2));

    auto ct = Clause::build();
    ct->add_literal(p_literal);

    kb.add_clause(std::move(ct));
}

} // namespace optifol
