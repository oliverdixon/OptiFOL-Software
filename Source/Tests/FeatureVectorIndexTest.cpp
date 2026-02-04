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
    auto x_var = std::vector<const IProcessedTerm *>{symbol_repository->add_symbol(Variable::build("x"))};
    auto f_fun = std::vector<const IProcessedTerm *>{symbol_repository->add_symbol(Function::build("F", std::move(x_var)))};
    const auto p_literal = symbol_repository->add_symbol(Literal::build("P", std::move(f_fun)));

    Clause clause;
    clause.add_literal(p_literal);
}

}
