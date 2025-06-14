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
     * @brief Tests that the given input correctly lexes and parses to the equivalent given typed structure
     * @param test The raw input string to pass to the lexer
     * @param expected The expected output sentence structure
     */
    void equality_on_input(const char *test, std::unique_ptr<IMutableSentenceNode>&& expected)
    {
        lexer_input_stream.str(test);
        parser.parse();

        const auto sentence = parser.retrieve_sentence();
        GoogleTestSupport::test_sentence_equality(*sentence, *expected);
    }

private:
    std::istringstream lexer_input_stream;
    FOLLexer lexer{lexer_input_stream, std::cerr};
    FOLParser parser{&lexer};
};

TEST_F(FOLParserTest, Quantifier_Universal)
{
    std::vector<std::unique_ptr<IMutableTermNode>> p_args;
    p_args.push_back(MutableVariableNode::build<IMutableTermNode>("x"));

    // clang-format off
    equality_on_input(
        "%Ux(P(x))",

        MutableQuantifiedSentenceNode::build(
            QuantifierTypes::Universal,
            MutableVariableNode::build("x"),
            MutablePredicationNode::build("P", std::move(p_args))
        )
    );
}

TEST_F(FOLParserTest, Quantifier_NegativeExistential)
{
    std::vector<std::unique_ptr<IMutableTermNode>> q_args;
    q_args.push_back(MutableVariableNode::build<IMutableTermNode>("y"));

    // clang-format off
    equality_on_input(
        "%Ey(~Q(y))",

        MutableQuantifiedSentenceNode::build(
            QuantifierTypes::Existential,
            MutableVariableNode::build("y"),
            MutablePredicationNode::build("Q", false, std::move(q_args))
        )
    );
}

TEST_F(FOLParserTest, Quantifier_Nested)
{
    std::vector<std::unique_ptr<IMutableTermNode>> p_args;
    p_args.push_back(MutableVariableNode::build<IMutableTermNode>("x"));

    std::vector<std::unique_ptr<IMutableTermNode>> q_args;
    q_args.push_back(MutableVariableNode::build<IMutableTermNode>("y"));

    // clang-format off
    equality_on_input(
        "%Ux(%Ey(P(x) & Q(y)))",

        MutableQuantifiedSentenceNode::build(
            QuantifierTypes::Universal,
            MutableVariableNode::build("x"),
            MutableQuantifiedSentenceNode::build(
                QuantifierTypes::Existential,
                MutableVariableNode::build("y"),
                MutableConnectedSentenceNode::build(
                    BinaryOperatorTypes::Conjunction,
                    MutablePredicationNode::build("P", std::move(p_args)),
                    MutablePredicationNode::build("Q", std::move(q_args))
                )
            )
        )
    );
}

TEST_F(FOLParserTest, Identity_Constants)
{
    // clang-format off
    equality_on_input(
        "_X = _Y",

        MutableIdentitySentenceNode::build(
            MutableConstantNode::build("_X"),
            MutableConstantNode::build("_Y")
        )
    );
}

}
