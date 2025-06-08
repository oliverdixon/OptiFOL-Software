/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Test unification of FOL sentences and terms
 * @author Oliver Dixon
 * @date 2025-07-08
 * @version Development
 */

#include <gtest/gtest.h>

#include "../IR/Sentences/PredicationNode.hpp"
#include "../IR/Terms/FunctionNode.hpp"
#include "../IR/Terms/VariableNode.hpp"

namespace optifol
{

class UnificationTest :
        public testing::Test
{
public:
    template<typename... Args>
    static std::vector<std::unique_ptr<optifol::ITermNode>> make_terms(Args &&...args)
    {
        std::vector<std::unique_ptr<optifol::ITermNode>> term_vector;
        (term_vector.emplace_back(std::forward<Args>(args)), ...);
        return term_vector;
    }
};

TEST_F(UnificationTest, Predicate)
{
    optifol::UnificationVisitor unification_visitor;

    const auto john = FunctionNode::build("John");
    const auto jane = FunctionNode::build("Jane");
    const auto xvar = VariableNode::build("x");

    const PredicationNode john_knows_x("Knows", UnificationTest::make_terms(john->clone(), xvar->clone()));
    const PredicationNode john_knows_jane("Knows", make_terms(john->clone(), jane->clone()));

    EXPECT_TRUE(john_knows_x.accept(unification_visitor, john_knows_jane));
    EXPECT_TRUE(unification_visitor.observe_substitutions().has_value());
    EXPECT_EQ(
        *unification_visitor.observe_substitutions(),
        Substitution({ { *xvar, *jane } })
    );
}

}
