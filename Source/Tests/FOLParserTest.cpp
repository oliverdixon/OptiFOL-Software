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
    void equality_on_input(const char *test, std::unique_ptr<ISentenceNode>&& expected)
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
    std::vector<std::unique_ptr<ITermNode>> p_args;
    p_args.push_back(VariableNode::build<ITermNode>("x"));

    // clang-format off
    equality_on_input(
        "%Ux(P(x))",

        QuantifiedSentenceNode::build(
            QuantifierTypes::Universal,
            VariableNode::build("x"),
            PredicationNode::build("P", std::move(p_args))
        )
    );
}

TEST_F(FOLParserTest, Quantifier_NegativeExistential)
{
    std::vector<std::unique_ptr<ITermNode>> q_args;
    q_args.push_back(VariableNode::build<ITermNode>("y"));

    // clang-format off
    equality_on_input(
        "%Ey(~Q(y))",

        QuantifiedSentenceNode::build(
            QuantifierTypes::Existential,
            VariableNode::build("y"),
            PredicationNode::build("Q", false, std::move(q_args))
        )
    );
}

TEST_F(FOLParserTest, Quantifier_Nested)
{
    std::vector<std::unique_ptr<ITermNode>> p_args;
    p_args.push_back(VariableNode::build<ITermNode>("x"));

    std::vector<std::unique_ptr<ITermNode>> q_args;
    q_args.push_back(VariableNode::build<ITermNode>("y"));

    // clang-format off
    equality_on_input(
        "%Ux(%Ey(P(x) & Q(y)))",

        QuantifiedSentenceNode::build(
            QuantifierTypes::Universal,
            VariableNode::build("x"),
            QuantifiedSentenceNode::build(
                QuantifierTypes::Existential,
                VariableNode::build("y"),
                ConnectedSentenceNode::build(
                    BinaryOperatorTypes::Conjunction,
                    PredicationNode::build("P", std::move(p_args)),
                    PredicationNode::build("Q", std::move(q_args))
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

        IdentitySentenceNode::build(
            ConstantNode::build("_X"),
            ConstantNode::build("_Y")
        )
    );
}

}
