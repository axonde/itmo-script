#include <sstream>

#include <interpreter>
#include <gtest/gtest.h>

using namespace Lexer;

std::vector<Tokens> MakeTokensVector(std::string&& str) {
    Tokenizer tokenizer(std::move(str));
    std::vector<Tokens> computed;
    Token token;
    tokenizer >> token;
    while (token.token != T_BAD && token.token != T_EOF) {
        computed.push_back(token.token);
        tokenizer >> token;
    }
    computed.push_back(token.token);
    return std::move(computed);
}


TEST(LexerTokenizerTest, OnePlusOneTest) {
    std::string program = R"(1 + 1)";

    std::vector<Tokens> computed = MakeTokensVector(std::move(program));

    std::vector<Tokens> expected = {T_NUMBER, T_PLUS, T_NUMBER, T_EOF};
    ASSERT_EQ(computed, expected);
}

TEST(LexerTokenizerTest, SimpleCalculator) {
    std::string program = R"(1 + (1 + 1))";

    std::vector<Tokens> computed = MakeTokensVector(std::move(program));

    std::vector<Tokens> expected = {T_NUMBER, T_PLUS, T_LEFT_BRACKET, T_NUMBER, T_PLUS, T_NUMBER, T_RIGHT_BRACKET, T_EOF};
    ASSERT_EQ(computed, expected);
}

TEST(LexerTokenizerTest, SimpleWrongSyntax) {
    std::string program = R"(###)";

    std::vector<Tokens> computed = MakeTokensVector(std::move(program));

    std::vector<Tokens> expected = {T_BAD};
    ASSERT_EQ(computed, expected);
}

TEST(LexerTokenizerTest, ForCycle) {
    std::string program = R"(
        for i in range(1, 2)
            a = a + 1
        end for
    )";

    std::vector<Tokens> computed = MakeTokensVector(std::move(program));

    std::vector<Tokens> expected = {
        T_EOL,
        T_FOR, T_VAR, T_IN, T_VAR, T_LEFT_BRACKET, T_NUMBER, T_COMMA, T_NUMBER, T_RIGHT_BRACKET, T_EOL,
        T_VAR, T_EQUAL, T_VAR, T_PLUS, T_NUMBER, T_EOL,
        T_END_FOR, T_EOL, T_EOF
    };
    ASSERT_EQ(computed, expected);
}

TEST(LexerTokenizerTest, Functions) {
    std::string program = R"(
        function calc(a, b)
            return a + b
        end function
    )";

    std::vector<Tokens> computed = MakeTokensVector(std::move(program));

    std::vector<Tokens> expected = {
        T_EOL,
        T_FUNC, T_VAR, T_LEFT_BRACKET, T_VAR, T_COMMA, T_VAR, T_RIGHT_BRACKET, T_EOL,
        T_RETURN, T_VAR, T_PLUS, T_VAR, T_EOL,
        T_END_FUNC, T_EOL, T_EOF
    };
    ASSERT_EQ(computed, expected);
}

TEST(LexerTokenizerTest, Empty) {
    std::string program = R"()";

    std::vector<Tokens> computed = MakeTokensVector(std::move(program));

    std::vector<Tokens> expected = {T_EOF};
    ASSERT_EQ(computed, expected);
}