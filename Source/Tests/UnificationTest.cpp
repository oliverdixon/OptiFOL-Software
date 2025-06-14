/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Test unification of FOL sentences and terms
 * @author Oliver Dixon
 * @date 2025-06-08
 * @version Development
 */

#include <gtest/gtest.h>

#include "../IR/Mutable/Sentences/MutablePredicationNode.hpp"
#include "../IR/Mutable/Terms/MutableFunctionNode.hpp"
#include "../IR/Mutable/Terms/MutableVariableNode.hpp"

namespace optifol
{

class UnificationTest :
        public testing::Test
{
public:
    template<typename... Args>
    static std::vector<std::unique_ptr<optifol::IMutableTermNode>> make_terms(Args &&...args)
    {
        std::vector<std::unique_ptr<optifol::IMutableTermNode>> term_vector;
        (term_vector.emplace_back(std::forward<Args>(args)), ...);
        return term_vector;
    }
};

TEST_F(UnificationTest, Predicate)
{
    UnificationVisitor unification_visitor;

    const auto john = MutableFunctionNode::build("John");
    const auto jane = MutableFunctionNode::build("Jane");
    const auto xvar = MutableVariableNode::build("x");

    const MutablePredicationNode john_knows_x("Knows", UnificationTest::make_terms(john->clone(), xvar->clone()));
    const MutablePredicationNode john_knows_jane("Knows", make_terms(john->clone(), jane->clone()));

    EXPECT_TRUE(john_knows_x.accept(unification_visitor, john_knows_jane));
    EXPECT_TRUE(unification_visitor.observe_substitutions().has_value());
    EXPECT_EQ(
        *unification_visitor.observe_substitutions(),
        Substitution({ { *xvar, *jane } })
    );
}

}
