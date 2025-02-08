/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#include <gtest/gtest.h>

#include "../Source/Interpreter/FOLLexer.hpp"

namespace optifol
{

class FOLParserTest :
        public testing::Test
{
protected:
    FOLParserTest() :
            lexer(lexer_input_stream, std::cerr),
            parser(&lexer)
    {}

    void TearDown() override
    {
        lexer_input_stream.clear();
    }

    void test_on_input(const char * input, const char * expected)
    {
        lexer_input_stream.str(input);
        parser.parse();
        EXPECT_STREQ(parser.retrieve_sentence()->to_string().c_str(), expected);
    }

private:
    std::istringstream lexer_input_stream;
    FOLLexer lexer;
    FOLParser parser;
};

TEST_F(FOLParserTest, UniversalQuantifier_Basic)
{
    test_on_input("%Ux(P(x))", "ForAll x (P(x))");
}

}
