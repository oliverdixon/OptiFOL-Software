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
protected:
    std::unique_ptr<UnificationVisitor> unification_visitor;

    void SetUp() override
    {
        unification_visitor.reset(new UnificationVisitor());
    }
};

TEST_F(UnificationTest, Predicate)
{
    const Function c("C");
    const Function d("D");
    const Variable x("x");

    const Predicate p1("P", { &c, &x });
    const Predicate p2("P", { &c, &d });

    EXPECT_TRUE(p1.accept(*unification_visitor, p2));
    EXPECT_TRUE(unification_visitor->observe_substitutions().has_value());
    EXPECT_EQ(
        unification_visitor->observe_substitutions(),
        Substitution({ { x, d } })
    );
}

}
