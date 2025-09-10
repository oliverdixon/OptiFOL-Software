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

#include "../IR/Sentences/Literal.hpp"
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

    template<typename Iterator>
    static bool are_substitution_sets_equal(const Iterator given_begin, const Iterator given_end,
        const std::unordered_map<const Variable *, const IProcessedTerm *>& expected_map)
    {
        const std::unordered_map<const Variable *, const IProcessedTerm *> given_map(given_begin, given_end);
        return given_map == expected_map;
    }

};

TEST_F(UnificationTest, Predicate)
{
    const Function c("C");
    const Function d("D");
    const Variable x("x");

    const Literal p1("P", { &c, &x });
    const Literal p2("P", { &c, &d });

    EXPECT_TRUE(p1.accept(*unification_visitor, p2));

    const std::unordered_map<const Variable *, const IProcessedTerm *> expected_subs{{&x, &d}};
    EXPECT_TRUE(are_substitution_sets_equal(unification_visitor->get_substitutions_cbegin(),
        unification_visitor->get_substitutions_cend(), expected_subs));
}

}
