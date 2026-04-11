/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Test unidirectional unification of FOL sentences and terms
 * @author Oliver Dixon
 * @date 2025-02-03
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

/**
 * @class UnidirectionalUnificationTest
 * @brief Tests the UnificationVisitor to verify that valid substitutions are constructed in the
 * Generalisation only to unify pairs of sentences.
 */
class UnidirectionalUnificationTest : public testing::Test
{
protected:
    std::unique_ptr<UnificationVisitor> unification_visitor;

    void SetUp() override
    {
        symbol_repository = std::make_shared<SymbolRepository>();
        unification_visitor = std::make_unique<UnificationVisitor>(symbol_repository);
    }

    template<typename TermType, class... CtorArgs>
    [[nodiscard]] const TermType *register_symbol(CtorArgs &&...ctor_args) const
    {
        return symbol_repository->add_symbol(
                std::make_unique<TermType>(std::forward<CtorArgs>(ctor_args)...));
    }

private:
    std::shared_ptr<SymbolRepository> symbol_repository;
};

/**
 * @brief Tests basic functionality of the UnidirectionalUnificationTest for a single pair of unifiable
 * literals with one applicable Function / Variable substitution.
 * @details
 *  <ul>
 *      <li>Generalisation: @f$ P \left( C\left(\right), x \right) @f$</li>
 *      <li>Instance: @f$ P \left( C\left(\right), D\left(\right) \right) @f$</li>
 *      <li>Expected substitutions: @f$ \left\{ x \mapsto D\left(\right) \right\} @f$</li>
 *  </ul>
 * @memberof UnidirectionalUnificationTest
 */
TEST_F(UnidirectionalUnificationTest, Positive_SingleBinding_FuncVar)
{
    const auto c = register_symbol<Function>("C");
    const auto d = register_symbol<Function>("D");
    const auto x = register_symbol<Variable>("x");

    const auto gen = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{c, x});
    const auto inst = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{c, d});

    EXPECT_TRUE(gen->accept(*unification_visitor, *inst));

    const Unifier expected_subs{{{x, d}}};
    EXPECT_EQ(*unification_visitor->observe_substitutions(), expected_subs);
}

/**
 * @brief Tests basic functionality of the UnidirectionalUnificationTest for a single pair of non-unifiable
 * literals, verifying that valid substitutions in the instance are refused.
 * @details
 *  <ul>
 *      <li>Generalisation: @f$ P \left( C\left(\right), x \right) @f$</li>
 *      <li>Instance: @f$ P \left( C\left(\right), D\left(\right) \right) @f$</li>
 *  </ul>
 * @memberof UnidirectionalUnificationTest
 */
TEST_F(UnidirectionalUnificationTest, Negative_SingleBinding_VarFunc)
{
    const auto c = register_symbol<Function>("C");
    const auto d = register_symbol<Function>("D");
    const auto x = register_symbol<Variable>("x");

    const auto gen = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{c, d});
    const auto inst = register_symbol<Literal>("P", std::vector<const IProcessedTerm *>{c, x});

    EXPECT_FALSE(gen->accept(*unification_visitor, *inst));
}

} // namespace optifol
