/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Test lexing and parsing of FOL sentences in the Optifol grammar
 * @author Oliver Dixon
 * @date 2025-02-16
 * @version Development
 */

#include <gtest/gtest.h>

#include "../Interpreter/FOLLexer.hpp"
#include "GoogleTestSupport.hpp"

namespace optifol
{

/**
 * @class FOLParserTest
 * @brief Provide a convenient input-streamer to the FOL lexer and parser for use with the Google Test framework
 */
class FOLParserTest :
        public testing::Test
{
protected:
    void TearDown() override
    {
        lexer_input_stream.clear();
    }

    /**
     * @brief Tests that the given input correctly lexes and parses to the equivalent given typed structure.
     * @param test The raw input string to pass to the lexer.
     * @param expected The expected output sentence structure. The sentence is wrapped in a positive
     *  MutableSentenceRoot.
     */
    void equality_on_input(const char *test, std::unique_ptr<IMutableSentence>&& expected)
    {
        lexer_input_stream.str(test);
        parser.parse();

        const auto sentence = parser.retrieve_sentence();
        const auto expected_root = std::make_unique<MutableSentenceRoot>(std::move(expected));

        GoogleTestSupport::test_sentence_equality(*sentence, *expected_root);
    }

private:
    std::istringstream lexer_input_stream;
    FOLLexer lexer{lexer_input_stream, std::cerr};
    FOLParser parser{&lexer};
};

TEST_F(FOLParserTest, Quantifier_Universal)
{
    std::vector<std::unique_ptr<IMutableTerm>> p_args;
    p_args.push_back(MutableVariable::build<IMutableTerm>("x"));

    // clang-format off
    equality_on_input(
        "%Ux(P(x))",

        MutableQuantified::build(
            QuantifierTypes::Universal,
            MutableVariable::build("x"),
            MutablePredicate::build("P", std::move(p_args))
        )
    );
}

TEST_F(FOLParserTest, Quantifier_NegativeExistential)
{
    std::vector<std::unique_ptr<IMutableTerm>> q_args;
    q_args.push_back(MutableVariable::build<IMutableTerm>("y"));

    // clang-format off
    equality_on_input(
        "%Ey(~Q(y))",

        MutableQuantified::build(
            QuantifierTypes::Existential,
            MutableVariable::build("y"),
            MutablePredicate::build("Q", false, std::move(q_args))
        )
    );
}

TEST_F(FOLParserTest, Quantifier_Nested)
{
    std::vector<std::unique_ptr<IMutableTerm>> p_args;
    p_args.push_back(MutableVariable::build<IMutableTerm>("x"));

    std::vector<std::unique_ptr<IMutableTerm>> q_args;
    q_args.push_back(MutableVariable::build<IMutableTerm>("y"));

    // clang-format off
    equality_on_input(
        "%Ux(%Ey(P(x) & Q(y)))",

        MutableQuantified::build(
            QuantifierTypes::Universal,
            MutableVariable::build("x"),
            MutableQuantified::build(
                QuantifierTypes::Existential,
                MutableVariable::build("y"),
                MutableBinaryConnected::build(
                    BinaryOperatorTypes::Conjunction,
                    MutablePredicate::build("P", std::move(p_args)),
                    MutablePredicate::build("Q", std::move(q_args))
                )
            )
        )
    );
}

TEST_F(FOLParserTest, TermBuilder_NoArguments)
{
    // clang-format off
    equality_on_input(
        "%Ux(P())",

        MutableQuantified::build(
            QuantifierTypes::Universal,
            MutableVariable::build("x"),
            MutablePredicate::build("P", true)
        )
    );
}

}
