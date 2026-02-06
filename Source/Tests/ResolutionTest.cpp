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
#include "../Inference/Prover.hpp"
#include "../Inference/QueryResult.hpp"
#include "../Inference/Resolvent.hpp"

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
        knowledge_base = std::make_unique<Prover>(symbol_repository);
    }

    std::shared_ptr<SymbolRepository> symbol_repository;
    std::unique_ptr<Prover> knowledge_base;
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
    // TODO fix these constructors so I don't have to specify arguments like this... !!! Deduction guides?
    std::vector<std::unique_ptr<IMutableTerm>> s1_p_args;
    s1_p_args.push_back(MutableVariable::build<IMutableTerm>("x"));

    std::vector<std::unique_ptr<IMutableTerm>> s1_q_args;
    s1_q_args.push_back(MutableVariable::build<IMutableTerm>("x"));

    std::vector<std::unique_ptr<IMutableTerm>> s2_p_args;
    s2_p_args.push_back(MutableConstant::build<IMutableTerm>("C"));

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

    knowledge_base->tell(*sentence1);
    knowledge_base->tell(*sentence2);

    std::vector<std::unique_ptr<IMutableTerm>> query_args;
    query_args.push_back(MutableConstant::build<IMutableTerm>("C"));

    const auto result = knowledge_base->ask(MutableSentenceRoot::build(MutablePredicate::build("Q",
        std::move(query_args))));

    EXPECT_EQ(result.outcome, QueryResult::ConjectureStatus::Consistent);
}

TEST_F(ResolutionTest, Reject_Trivial)
{
    std::vector<std::unique_ptr<IMutableTerm>> s1_p_args;
    s1_p_args.push_back(MutableVariable::build<IMutableTerm>("x"));

    std::vector<std::unique_ptr<IMutableTerm>> s1_q_args;
    s1_q_args.push_back(MutableVariable::build<IMutableTerm>("x"));

    std::vector<std::unique_ptr<IMutableTerm>> s2_p_args;
    s2_p_args.push_back(MutableConstant::build<IMutableTerm>("C"));

    const auto sentence1 = ExpressionFactory::build_sentence(MutableSentenceRoot::build(
        MutablePredicate::build("P", std::move(s2_p_args))
    ), symbol_repository);

    const auto sentence2 = ExpressionFactory::build_sentence(MutableSentenceRoot::build(
        MutablePredicate::build("Q", std::move(s2_p_args))
    ), symbol_repository);

    knowledge_base->tell(*sentence1);
    knowledge_base->tell(*sentence2);

    std::vector<std::unique_ptr<IMutableTerm>> query_args;
    query_args.push_back(MutableConstant::build<IMutableTerm>("C"));

    const auto result = knowledge_base->ask(MutableSentenceRoot::build(MutablePredicate::build("R",
        std::move(query_args))));

    EXPECT_EQ(result.outcome, QueryResult::ConjectureStatus::Inconsistent);
}

TEST_F(ResolutionTest, CuriosityKilledTheCat)
{
    // Anybody who loves all animals is themselves loved by somebody.
    std::vector<std::unique_ptr<IMutableTerm>> animal_args_1;
    animal_args_1.push_back(MutableVariable::build("y"));

    std::vector<std::unique_ptr<IMutableTerm>> loves_args_1;
    loves_args_1.push_back(MutableVariable::build("x"));
    loves_args_1.push_back(MutableVariable::build("y"));

    std::vector<std::unique_ptr<IMutableTerm>> loves_args_2;
    loves_args_2.push_back(MutableVariable::build("y"));
    loves_args_2.push_back(MutableVariable::build("x"));

    const auto loves_all_animals = ExpressionFactory::build_sentence(MutableSentenceRoot::build(
        MutableQuantified::build(
            QuantifierTypes::Universal,
            MutableVariable::build("x"),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Implication,
                MutableQuantified::build(
                    QuantifierTypes::Universal,
                    MutableVariable::build("y"),
                    MutableBinaryConnected::build(
                        BinaryOperatorTypes::Implication,
                        MutablePredicate::build("Animal", std::move(animal_args_1)),
                        MutablePredicate::build("Loves", std::move(loves_args_1))
                    )
                ),
                MutableQuantified::build(
                    QuantifierTypes::Existential,
                    MutableVariable::build("y"),
                    MutablePredicate::build("Loves", std::move(loves_args_2))
                )
            )
        )
    ), symbol_repository);

    // Anybody who kills an animal is loved by nobody.
    std::vector<std::unique_ptr<IMutableTerm>> animal_args_2;
    animal_args_2.push_back(MutableVariable::build("z"));

    std::vector<std::unique_ptr<IMutableTerm>> kills_args_1;
    kills_args_1.push_back(MutableVariable::build("x"));
    kills_args_1.push_back(MutableVariable::build("z"));

    std::vector<std::unique_ptr<IMutableTerm>> loves_args_3;
    loves_args_3.push_back(MutableVariable::build("y"));
    loves_args_3.push_back(MutableVariable::build("x"));

    const auto kills_an_animal = ExpressionFactory::build_sentence(MutableSentenceRoot::build(
        MutableQuantified::build(
            QuantifierTypes::Universal,
            MutableVariable::build("x"),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Implication,
                MutableQuantified::build(
                    QuantifierTypes::Existential,
                    MutableVariable::build("z"),
                    MutableBinaryConnected::build(
                        BinaryOperatorTypes::Conjunction,
                        MutablePredicate::build("Animal", std::move(animal_args_2)),
                        MutablePredicate::build("Kills", std::move(kills_args_1))
                    )
                ),
                MutableQuantified::build(
                    QuantifierTypes::Universal,
                    MutableVariable::build("y"),
                    MutablePredicate::build("Loves", false, std::move(loves_args_3))
                )
            )
        )
    ), symbol_repository);

    // Jack loves all animals.
    std::vector<std::unique_ptr<IMutableTerm>> animal_args_3;
    animal_args_3.push_back(MutableVariable::build("x"));

    std::vector<std::unique_ptr<IMutableTerm>> loves_args_4;
    loves_args_4.push_back(MutableConstant::build("Jack"));
    loves_args_4.push_back(MutableVariable::build("x"));

    const auto jack_loves_animals = ExpressionFactory::build_sentence(MutableSentenceRoot::build(
        MutableQuantified::build(
            QuantifierTypes::Universal,
            MutableVariable::build("x"),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Implication,
                MutablePredicate::build("Animal", std::move(animal_args_3)),
                MutablePredicate::build("Loves", std::move(loves_args_4))
            )
        )
    ), symbol_repository);

    // Tuna is killed by Jack or Curiosity.
    std::vector<std::unique_ptr<IMutableTerm>> kills_args_2;
    kills_args_2.push_back(MutableConstant::build("Jack"));
    kills_args_2.push_back(MutableConstant::build("Tuna"));

    std::vector<std::unique_ptr<IMutableTerm>> kills_args_3;
    kills_args_3.push_back(MutableConstant::build("Curiosity"));
    kills_args_3.push_back(MutableConstant::build("Tuna"));

    const auto tuna_is_killed = ExpressionFactory::build_sentence(MutableSentenceRoot::build(
        MutableBinaryConnected::build(
            BinaryOperatorTypes::Disjunction,
            MutablePredicate::build("Kills", std::move(kills_args_2)),
            MutablePredicate::build("Kills", std::move(kills_args_3))
        )
    ), symbol_repository);

    // Tuna is a cat.
    std::vector<std::unique_ptr<IMutableTerm>> cat_args_1;
    cat_args_1.push_back(MutableConstant::build("Tuna"));

    const auto tuna_is_cat = ExpressionFactory::build_sentence(MutableSentenceRoot::build(
        MutablePredicate::build("Cat", std::move(cat_args_1))
    ), symbol_repository);

    // Cats are animals.
    std::vector<std::unique_ptr<IMutableTerm>> cat_args_2;
    cat_args_2.push_back(MutableVariable::build("x"));

    std::vector<std::unique_ptr<IMutableTerm>> animal_args_4;
    animal_args_4.push_back(MutableVariable::build("x"));

    const auto cats_are_animals = ExpressionFactory::build_sentence(MutableSentenceRoot::build(
        MutableQuantified::build(
            QuantifierTypes::Universal,
            MutableVariable::build("x"),
            MutableBinaryConnected::build(
                BinaryOperatorTypes::Implication,
                MutablePredicate::build("Cat", std::move(cat_args_2)),
                MutablePredicate::build("Animal", std::move(animal_args_4))
            )
        )
    ), symbol_repository);

    // Tell the KB the facts...
    knowledge_base->tell(*tuna_is_killed);
    knowledge_base->tell(*tuna_is_cat);
    knowledge_base->tell(*loves_all_animals);
    knowledge_base->tell(*kills_an_animal);
    knowledge_base->tell(*jack_loves_animals);
    knowledge_base->tell(*cats_are_animals);

    // Did Curiosity kill Tuna?
    std::vector<std::unique_ptr<IMutableTerm>> kills_args_4;
    kills_args_4.push_back(MutableConstant::build("Curiosity"));
    kills_args_4.push_back(MutableConstant::build("Tuna"));

    const auto result = knowledge_base->ask(MutableSentenceRoot::build(MutablePredicate::build("Kills",
        std::move(kills_args_4))));

    EXPECT_EQ(result.outcome, QueryResult::ConjectureStatus::Consistent);
}

} // namespace optifol
