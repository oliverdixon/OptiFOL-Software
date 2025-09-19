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
#include "../IR/SymbolRepository.hpp"
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
        symbol_repository = std::make_shared<SymbolRepository>();
        unification_visitor = std::make_unique<UnificationVisitor>(symbol_repository);
    }

    template<typename TermType, class... CtorArgs>
    const TermType * register_symbol(CtorArgs&&... ctor_args) const
    {
        return symbol_repository->add_symbol(std::make_unique<TermType>(std::forward<CtorArgs>(ctor_args)...));
    }

private:
    std::shared_ptr<SymbolRepository> symbol_repository;
};

/**
 * @brief Tests basic functionality of the UnificationVisitor for a single pair of unifiable literals with one
 *  applicable Function / Variable substitution.
 * @details
 *  <ul>
 *      <li>LHS Input: @f$ P \left( C\left(\right), x \right) @f$</li>
 *      <li>RHS Input: @f$ P \left( C\left(\right), D\left(\right) \right) @f$</li>
 *      <li>Expected substitutions: @f$ \left\{ D\left(\right) \mapsto x \right\} @f$</li>
 *  </ul>
 * @memberof UnificationTest
 */
TEST_F(UnificationTest, Positive_SingleBinding_FuncVar)
{
    const auto c = register_symbol<Function>("C");
    const auto d = register_symbol<Function>("D");
    const auto x = register_symbol<Variable>("x");

    const auto p1 = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{ c, x });
    const auto p2 = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{ c, d });

    EXPECT_TRUE(p1->accept(*unification_visitor, *p2));

    const UnificationVisitor::SubstitutionMap expected_subs{{x, d}};
    EXPECT_EQ(unification_visitor->get_substitutions(), expected_subs);
}

/**
 * @brief Tests basic functionality of the UnificationVisitor for a single pair of unifiable literals with two
 *  applicable Function / Variable substitutions.
 * @details
 *  <ul>
 *      <li>LHS Input: @f$ P \left( C\left(\right), x \right) @f$</li>
 *      <li>RHS Input: @f$ P \left( y, D\left(\right) \right) @f$</li>
 *      <li>Expected substitutions: @f$ \left\{ D\left(\right) \mapsto x, C\left(\right) \mapsto y \right\} @f$</li>
 *  </ul>
 * @memberof UnificationTest
 */
TEST_F(UnificationTest, Positive_MultipleBindings_FuncVar)
{
    const auto c = register_symbol<Function>("C");
    const auto d = register_symbol<Function>("D");
    const auto x = register_symbol<Variable>("x");
    const auto y = register_symbol<Variable>("y");

    const auto p1 = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{ c, x });
    const auto p2 = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{ y, d });

    EXPECT_TRUE(p1->accept(*unification_visitor, *p2));

    const UnificationVisitor::SubstitutionMap expected_subs{
        {x, d},
        {y, c}
    };

    EXPECT_EQ(unification_visitor->get_substitutions(), expected_subs);
}

/**
 * @brief Tests basic functionality of the UnificationVisitor for a single pair of unifiable literals with two
 *  applicable Variable / Variable substitutions.
 * @details
 *  <ul>
 *      <li>LHS Input: @f$ P \left( a, b \right) @f$</li>
 *      <li>RHS Input: @f$ P \left( c, d \right) @f$</li>
 *      <li>Expected substitutions: @f$ \left\{ a \mapsto c, b \mapsto d \right\} @f$</li>
 *  </ul>
 * @memberof UnificationTest
 */
TEST_F(UnificationTest, Positive_MultipleBindings_VarVar)
{
    const auto a = register_symbol<Variable>("a");
    const auto b = register_symbol<Variable>("b");
    const auto c = register_symbol<Variable>("c");
    const auto d = register_symbol<Variable>("d");

    const auto p1 = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{ a, b });
    const auto p2 = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{ c, d });

    EXPECT_TRUE(p1->accept(*unification_visitor, *p2));

    const UnificationVisitor::SubstitutionMap expected_subs{
            {c, a},
            {d, b}
    };

    EXPECT_EQ(unification_visitor->get_substitutions(), expected_subs);
}

/**
 * @brief Tests basic functionality of the UnificationVisitor for a single pair of non-unifiable literals.
 * @details
 *  <ul>
 *      <li>LHS Input: @f$ P \left( C\left(\right), x \right) @f$</li>
 *      <li>RHS Input: @f$ P \left( x, D\left(\right) \right) @f$</li>
 *  </ul>
 * @memberof UnificationTest
 */
TEST_F(UnificationTest, Negative_MultipleBindings_VarVar)
{
    const auto x = register_symbol<Variable>("x");
    const auto c = register_symbol<Function>("C");
    const auto d = register_symbol<Function>("D");

    const auto p1 = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{ c, x });
    const auto p2 = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{ x, d });

    EXPECT_FALSE(p1->accept(*unification_visitor, *p2));
}

/**
 * @brief Tests occurs-checking functionality of the UnificationVisitor for a single pair of literals, non-unifiable due
 *  to the trivial case of the occurs-check (does not require substitution to discover).
 * @details
 *  <ul>
 *      <li>LHS Input: @f$ P \left( C\left(\right), x \right) @f$</li>
 *      <li>RHS Input: @f$ P \left( C\left(\right), F\left(x\right) \right) @f$</li>
 *  </ul>
 * @memberof UnificationTest
 */
TEST_F(UnificationTest, Negative_OccursCheck_Trivial)
{
    const auto x = register_symbol<Variable>("x");
    const auto c = register_symbol<Function>("C");
    const auto f = register_symbol<Function>("F", std::vector<const IProcessedTerm *>{ x });

    const auto p1 = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{ c, x });
    const auto p2 = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{ c, f });

    EXPECT_FALSE(p1->accept(*unification_visitor, *p2));
}

/**
 * @brief Tests occurs-checking functionality of the UnificationVisitor for a single pair of literals, non-unifiable due
 *  to the non-trivial case of the occurs-check (requires substitution to discover).
 * @details
 *  <ul>
 *      <li>LHS Input: @f$ P \left( x, F\left(x\right) \right) @f$</li>
 *      <li>RHS Input: @f$ P \left( G\left(y\right), y \right) @f$</li>
 *  </ul>
 *  <p>
 *      The substitution @f$ \left[ x \mapsto G\left(y\right) \right] @f$ is discovered, followed by the substitution
 *      @f$ \left[ y \mapsto F\left(x\right) \right] @f$. Although @f$ y @f$ does not immediately appear in its proposed
 *      binding @f$ F\left(x\right) @f$, expanding on the first substitution to produce
 *      @f$ F\left(G\left( y \right)\right) @f$ exposes the cycle that would be introduced by adding the second
 *      substitution. Hence the LHS and RHS predicates are non-unifiable.
 *  </p>
 * @memberof UnificationTest
 */
TEST_F(UnificationTest, Negative_OccursCheck_Substituted)
{
    const auto x = register_symbol<Variable>("x");
    const auto f_x = register_symbol<Function>("F", std::vector<const IProcessedTerm *>{ x });
    const auto y = register_symbol<Variable>("y");
    const auto g_y = register_symbol<Function>("G", std::vector<const IProcessedTerm *>{ y });

    const auto p1 = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{ x, f_x });
    const auto p2 = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{ g_y, y });

    EXPECT_FALSE(p1->accept(*unification_visitor, *p2));
}

}
