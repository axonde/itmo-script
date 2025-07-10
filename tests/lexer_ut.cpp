#include <ranges>
#include <utility>

#include <interpreter>
#include <gtest/gtest.h>

using namespace Lexer;

std::vector<Token> MakeTokensVector(std::string&& str) {
    Tokenizer tokenizer;
    tokenizer << str;
    std::vector<Token> computed;
    Token token;
    tokenizer >> token;
    while (token.token != T_EOF) {
        computed.push_back(token);
        tokenizer >> token;
    }
    computed.push_back(token);
    return std::move(computed);
}

std::vector<Tokens> MakeTokensTypeVector(std::string&& str) {
    Tokenizer tokenizer;
    tokenizer << str;
    std::vector<Tokens> computed;
    Token token;
    tokenizer >> token;
    while (token.token != T_EOF) {
        computed.push_back(token.token);
        tokenizer >> token;
    }
    computed.push_back(token.token);
    return std::move(computed);
}

TEST(LexerTokenizerTest, OnePlusOneTest) {
    std::string program = R"(1 + 1)";

    std::vector<Tokens> computed = MakeTokensTypeVector(std::move(program));

    std::vector<Tokens> expected = {T_NUMBER, T_PLUS, T_NUMBER, T_EOF};
    ASSERT_EQ(computed, expected);
}

TEST(LexerTokenizerTest, SimpleCalculator) {
    std::string program = R"(1 + (1 + 1))";

    std::vector<Tokens> computed = MakeTokensTypeVector(std::move(program));

    std::vector<Tokens> expected = {T_NUMBER, T_PLUS, T_LEFT_BRACKET, T_NUMBER, T_PLUS, T_NUMBER, T_RIGHT_BRACKET, T_EOF};
    ASSERT_EQ(computed, expected);
}

TEST(LexerTokenizerTest, FiveByThree) {
    std::string program = R"(5 / 3)";

    std::vector<Tokens> computed = MakeTokensTypeVector(std::move(program));

    std::vector<Tokens> expected = {T_NUMBER, T_DIV, T_NUMBER, T_EOF};
    ASSERT_EQ(computed, expected);
}

TEST(LexerTokenizerTest, GreaterOrEq) {
    std::string program = R"(5 >= 3)";

    std::vector<Tokens> computed = MakeTokensTypeVector(std::move(program));

    std::vector<Tokens> expected = {T_NUMBER, T_COMP_GREATER_OR_EQ, T_NUMBER, T_EOF};
    ASSERT_EQ(computed, expected);
}

TEST(LexerTokenizerTest, SimpleWrongSyntax) {
    std::string program = R"(###)";

    EXPECT_THROW(
        std::vector<Tokens> computed = MakeTokensTypeVector(std::move(program));
    , Errors::LexerErrors::LexerError);
}

TEST(LexerTokenizerTest, ForCycle) {
    std::string program = R"(
        for i in range(1, 2)
            a = a + 1
        end for
    )";

    std::vector<Tokens> computed = MakeTokensTypeVector(std::move(program));
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
        function(a, b)
            return a + b
        end function
    )";

    std::vector<Tokens> computed = MakeTokensTypeVector(std::move(program));

    std::vector<Tokens> expected = {
        T_EOL,
        T_FUNC, T_LEFT_BRACKET, T_VAR, T_COMMA, T_VAR, T_RIGHT_BRACKET, T_EOL,
        T_RETURN, T_VAR, T_PLUS, T_VAR, T_EOL,
        T_END_FUNC, T_EOL, T_EOF
    };
    ASSERT_EQ(computed, expected);
}


TEST(LexerTokenizerTest, EmptyFunction) {
    std::string program = R"(function() end function)";

    std::vector<Tokens> computed = MakeTokensTypeVector(std::move(program));

    std::vector<Tokens> expected = {
        T_FUNC, T_LEFT_BRACKET, T_RIGHT_BRACKET, T_END_FUNC, T_EOF
    };
    ASSERT_EQ(computed, expected);
}

TEST(LexerTokenizerTest, Empty) {
    std::string program = R"()";

    std::vector<Tokens> computed = MakeTokensTypeVector(std::move(program)); 

    std::vector<Tokens> expected = {T_EOF};
    ASSERT_EQ(computed, expected);
}

TEST(LexerTokenTest, SimplePosTracker) {
     std::string program = R"(a = 1)";

    std::vector<Token> computed = MakeTokensVector(std::move(program));
    auto transformed = computed | std::views::transform([](const Lexer::Token& token) {
        return std::make_pair(token.lineno, token.column);
    });
    std::vector<std::pair<size_t, size_t>> positions(transformed.begin(), transformed.end());

    std::vector<std::pair<size_t, size_t>> expected_positions = {
        {1, 2},
        {1, 4},
        {1, 6},
        {1, 6}
    };

    ASSERT_EQ(positions, expected_positions);
}

TEST(LexerTokenTest, MultiLinePosTracker) {
     std::string program = R"(a = 1
b = 2)";

    std::vector<Token> computed = MakeTokensVector(std::move(program));
    auto transformed = computed | std::views::transform([](const Lexer::Token& token) {
        return std::make_pair(token.lineno, token.column);
    });
    std::vector<std::pair<size_t, size_t>> positions(transformed.begin(), transformed.end());

    std::vector<std::pair<size_t, size_t>> expected_positions = {
        {1, 2},
        {1, 4},
        {1, 6},
        {2, 1},
        {2, 2},
        {2, 4},
        {2, 6},
        {2, 6}
    };

    ASSERT_EQ(positions, expected_positions);
}

TEST(LexerTokenTest, MultiLinePosTrackerWithSpace) {
     std::string program = R"(a = 1
  b = 2)";

    std::vector<Token> computed = MakeTokensVector(std::move(program));
    auto transformed = computed | std::views::transform([](const Lexer::Token& token) {
        return std::make_pair(token.lineno, token.column);
    });
    std::vector<std::pair<size_t, size_t>> positions(transformed.begin(), transformed.end());

    std::vector<std::pair<size_t, size_t>> expected_positions = {
        {1, 2},
        {1, 4},
        {1, 6},
        {2, 1},
        {2, 4},
        {2, 6},
        {2, 8},
        {2, 8}
    };

    ASSERT_EQ(positions, expected_positions);
}

TEST(LexerTokenTest, MultiLinePosTrackerFunction) {
     std::string program = R"(function(a, b)
  a = 1
end function)";

    std::vector<Token> computed = MakeTokensVector(std::move(program));
    auto transformed = computed | std::views::transform([](const Lexer::Token& token) {
        return std::make_pair(token.lineno, token.column);
    });
    std::vector<std::pair<size_t, size_t>> positions(transformed.begin(), transformed.end());

    std::vector<std::pair<size_t, size_t>> expected_positions = {
        {1, 9},     // T_FUNC
        {1, 10},    // T_LEFT_BRACKET
        {1, 11},    // T_VAR
        {1, 12},    // T_COMMA
        {1, 14},    // T_VAR
        {1, 15},    // T_RIGHT_BRACKET
        {2, 1},     // T_EOL
        {2, 4},     // T_VAR
        {2, 6},     // T_EQUAL
        {2, 8},     // T_NUM
        {3, 1},     // T_EOL
        {3, 13},    // T_END_FUNC
        {3, 13}     // T_EOF
    };

    ASSERT_EQ(positions, expected_positions);
}

TEST(LexerInterfaceTest, MultiLineInput) {
    Lexer::Tokenizer tokenizer;

    tokenizer << "a = 1\n";
    tokenizer << "b = 2";

    std::vector<Tokens> computed;
    Token token;
    tokenizer >> token;
    while (token.token != T_EOF) {
        computed.push_back(token.token);
        tokenizer >> token;
    }
    computed.push_back(token.token);

    std::vector<Tokens> expected = {
        T_VAR, T_EQUAL, T_NUMBER, T_EOL,
        T_VAR, T_EQUAL, T_NUMBER, T_EOF };
    ASSERT_EQ(computed, expected);
}

TEST(LexerStressTest, WrongSyntaxStringLiteral) {
    std::string program = R"(
        a = "sdfkjfkjdf
        b = 2
    )";

    Lexer::Tokenizer tokenizer;

    ASSERT_THROW(
        tokenizer << program;
    , Errors::LexerErrors::LexerStringError);
}

TEST(LexerStressTest, WrongSyntaxNumLiteral) {
    std::string program = R"(
        a = 1123.
        b = 2
    )";

    Lexer::Tokenizer tokenizer;

    ASSERT_THROW(
        tokenizer << program;
    , Errors::LexerErrors::LexerNumberError);
}

TEST(LexerStressTest, WrongSyntaxUndefinedSymbols) {
    std::string program = R"(!@!#)";

    Lexer::Tokenizer tokenizer;

    ASSERT_THROW(
        tokenizer << program;
    , Errors::LexerErrors::LexerUnrecognizable);
}

TEST(LexerStressTest, WrongSyntaxIfStatementForgotEndIf) {
    std::string program = R"(
        if i < 3 then
            i = 3
    )";

    Lexer::Tokenizer tokenizer;

    ASSERT_THROW(
        tokenizer << program;
    , Closures::UncaughtClosure);
}

TEST(LexerStressTest, WrongSyntaxForStatementForgotEndFor) {
    std::string program = R"(
        for i in range(3)
            continue
    )";

    Lexer::Tokenizer tokenizer;

    ASSERT_THROW(
        tokenizer << program;
    , Closures::UncaughtClosure);
}

TEST(LexerStressTest, AbnormalInput) {
    std::string program = R"(
        !@O#I1o] 	2-]9u j[9j4n ln;lkj;j]
    )";

    Lexer::Tokenizer tokenizer;

    ASSERT_THROW(
        tokenizer << program;
    , Errors::LexerErrors::LexerUnrecognizable);
}
