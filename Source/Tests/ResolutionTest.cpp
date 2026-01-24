/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Test resolution of FOL knowledge bases to deduce goals
 * @author Oliver Dixon
 * @date 2025-09-13
 * @version Development
 */

#include <gtest/gtest.h>

#include "../IR/MutableVariants/Sentences/MutableBinaryConnected.hpp"
#include "../IR/MutableVariants/Sentences/MutablePredicate.hpp"
#include "../IR/MutableVariants/Sentences/MutableQuantified.hpp"
#include "../IR/MutableVariants/Sentences/MutableSentenceRoot.hpp"
#include "../IR/MutableVariants/Terms/MutableConstant.hpp"
#include "../IR/MutableVariants/Terms/MutableVariable.hpp"
#include "../IR/SymbolRepository.hpp"
#include "../Inference/ExpressionFactory.hpp"
#include "../Inference/KnowledgeBase.hpp"

namespace optifol
{

/**
 * @class ResolutionTest
 */
class ResolutionTest : public testing::Test
{
protected:
    void SetUp() override
    {
        symbol_repository = std::make_shared<SymbolRepository>();
        knowledge_base = std::make_unique<KnowledgeBase>(symbol_repository);
    }

    std::shared_ptr<SymbolRepository> symbol_repository;
    std::unique_ptr<KnowledgeBase> knowledge_base;
};

/**
 * @brief Tests Resolution functionality for a Modus Ponens knowledge base over a single universally quantified variable
 *  @f$ x @f$ and a constant @f$ C @f$.
 * @details
 *  <table>
 *      <tr>
 *          <td />
 *          <th>FOL Sentence</th>
 *          <th>CNF Sentence</th>
 *          <th>Conjunct-Disjunct Set</th>
 *      </tr>
 *      <tr>
 *          <th rowspan="2">Knowledge Base</th>
 *          <td>@f$ \forall x \left( P\left(x\right) \implies Q\left(x\right) \right) @f$</td>
 *          <td>@f$ \lnot P\left(x\right) \lor Q\left(x\right) @f$</td>
 *          <td>@f$ \left\{ \left\{ \lnot P\left(x\right), Q\left(x\right) \right\} \right\} @f$</td>
 *      </tr>
 *      <tr>
 *          <td>@f$ P\left(C\right) @f$</td>
 *          <td>@f$ P\left(C\right) @f$</td>
 *          <td>@f$ \left\{ \left\{ P\left(C\right) \right\} \right\} @f$</td>
 *      </tr>
 *      <tr>
 *          <th>Goal</th>
 *          <td>@f$ Q\left(C\right) @f$</td>
 *          <td>@f$ Q\left(C\right) @f$</td>
 *          <td>@f$ \left\{ \left\{ Q\left(C\right) \right\} \right\} @f$</td>
 *      </tr>
 *  </table>
 * @memberof ResolutionTest
 */
TEST_F(ResolutionTest, ModusPonens_Quantified)
{
    std::vector<std::unique_ptr<IMutableTerm>> s1_p_args;
    s1_p_args.push_back(MutableVariable::build<IMutableTerm>("x"));

    std::vector<std::unique_ptr<IMutableTerm>> s1_q_args;
    s1_q_args.push_back(MutableVariable::build<IMutableTerm>("x"));

    std::vector<std::unique_ptr<IMutableTerm>> s2_p_args;
    s2_p_args.push_back(MutableConstant::build<IMutableTerm>("C"));

    std::vector<std::unique_ptr<IMutableTerm>> query_args;
    query_args.push_back(MutableConstant::build<IMutableTerm>("C"));

    // clang-format off

    const auto sentence1 = ExpressionFactory::build_sentence(MutableSentenceRoot::build(
        MutableQuantified::build(
            QuantifierTypes::Universal,
            MutableVariable::build("x"),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Implication,
                MutablePredicate::build("P", std::move(s1_p_args)),
                MutablePredicate::build("Q", std::move(s1_q_args))
            )
        )
    ), symbol_repository);

    // clang-format on

    const auto sentence2 = ExpressionFactory::build_sentence(MutableSentenceRoot::build(
        MutablePredicate::build("P", std::move(s2_p_args))
    ), symbol_repository);

    const auto query = ExpressionFactory::build_sentence(
        MutableSentenceRoot::build(MutablePredicate::build("Q", false, std::move(query_args))
    ), symbol_repository);

    knowledge_base->tell(*sentence1);
    knowledge_base->tell(*sentence2);

    EXPECT_TRUE(knowledge_base->query_negative(*query));
}

} // namespace optifol
