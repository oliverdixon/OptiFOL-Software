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

#include "../IR/Sentences/Literal.hpp"
#include "../IR/SymbolRepository.hpp"
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
 *  @f$ x @f$.
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
 *          <td>@f$ P\left(x\right) @f$</td>
 *          <td>@f$ P\left(x\right) @f$</td>
 *          <td>@f$ \left\{ \left\{ P\left(x\right) \right\} \right\} @f$</td>
 *      </tr>
 *      <tr>
 *          <th>Goal</th>
 *          <td>@f$ Q\left(x\right) @f$</td>
 *          <td>@f$ Q\left(x\right) @f$</td>
 *          <td>@f$ \left\{ \left\{ Q\left(x\right) \right\} \right\} @f$</td>
 *      </tr>
 *  </table>
 * @memberof ResolutionTest
 */
TEST_F(ResolutionTest, ModusPonens_Quantified)
{
    const auto x = symbol_repository->add_symbol(std::make_unique<Variable>("x"));
    const auto p_neg = symbol_repository->add_symbol(std::make_unique<Literal>(
        "P", std::vector<const IProcessedTerm *>{ x }, false));
    const auto q = symbol_repository->add_symbol(std::make_unique<Literal>(
        "Q", std::vector<const IProcessedTerm *>{ x }));


}

} // namespace optifol
