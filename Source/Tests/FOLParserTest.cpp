/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Basic parsing tester for FOL sentences
 * @author Oliver Dixon
 * @date 2025-02-16
 * @version Development
 */

#include <gtest/gtest.h>

#include "../Interpreter/FOLLexer.hpp"
#include "../Visitors/Sentences/Serialisers/JSONSerialiserVisitor.hpp"

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
     * @brief Tests that the given input correctly lexes and parses to the equivalent given JSON object
     * @param input The raw input string to pass to the lexer
     * @param expected The expected output JSON object
     */
    void equality_on_input(const char * input, nlohmann::json&& expected)
    {
        lexer_input_stream.str(input);
        parser.parse();

        const auto sentence = parser.retrieve_sentence();
        sentence->accept(json_serialiser);
        EXPECT_EQ(json_serialiser.extract(), expected);
    }

private:
    JSONSerialiserVisitor json_serialiser;
    std::istringstream lexer_input_stream;
    FOLLexer lexer{lexer_input_stream, std::cerr};
    FOLParser parser{&lexer};
};

TEST_F(FOLParserTest, Quantifier_Universal)
{
    equality_on_input(
        "%Ux(P(x))",
        {
            { "type", "quantified" },
            { "nature", "universal" },
            { "variable", "x" },
            { "sentence", {
                { "type", "predicate" },
                { "name", "P" },
                { "arguments", { "x" } }
            } }
        }
    );
}

TEST_F(FOLParserTest, Quantifier_Existential)
{
    equality_on_input(
        "%Ey(Q(y))",
        {
            { "type", "quantified" },
            { "nature", "existential" },
            { "variable", "y" },
            { "sentence", {
                { "type", "predicate" },
                { "name", "Q" },
                { "arguments", { "y" } }
            } }
        }
    );
}

TEST_F(FOLParserTest, Quantifier_Nested)
{
    equality_on_input(
        "%Ux(%Ey(P(x) & Q(y)))",
        {
            { "type", "quantified" },
            { "nature", "universal" },
            { "variable", "x" },
            { "sentence", {
                { "type", "quantified" },
                { "nature", "existential" },
                { "variable", "y" },
                { "sentence", {
                    { "type", "binary" },
                    { "operator", "conjunction" },
                    { "lhs", {
                        { "type", "predicate" },
                        { "name", "P" },
                        { "arguments", { "x" } }
                    } },
                    { "rhs", {
                        { "type", "predicate" },
                        { "name", "Q" },
                        { "arguments", { "y" } }
                    } }
                } }
            } }
        }
    );
}

TEST_F(FOLParserTest, Identity_Constants)
{
    equality_on_input(
        "_X = _Y",
        {
            { "type", "equality" },
            { "lhs", "_X" },
            { "rhs", "_Y" }
        }
    );
}

}
