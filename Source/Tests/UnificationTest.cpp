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

#include "../IR/Sentences/Predicate.hpp"
#include "../IR/Terms/Function.hpp"
#include "../IR/Terms/Variable.hpp"
#include "../Visitors/RegularTargets/Unification/UnificationVisitor.hpp"

namespace optifol
{

class UnificationTest :
        public testing::Test
{
    // TODO does something need to be here?
};

TEST_F(UnificationTest, Predicate)
{
    UnificationVisitor unification_visitor;

    const Function john("John");
    const Function jane("Jane");
    const Variable xvar("x");

    const Predicate john_knows_x("Knows", { &john, &xvar });
    const Predicate john_knows_jane("Knows", { &john, &jane });

    EXPECT_TRUE(john_knows_x.accept(unification_visitor, john_knows_jane));
    EXPECT_TRUE(unification_visitor.observe_substitutions().has_value());
    EXPECT_EQ(
        *unification_visitor.observe_substitutions(),
        Substitution({ { xvar, jane } })
    );
}

}
