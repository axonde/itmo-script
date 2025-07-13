#include <interpreter>
#include <gtest/gtest.h>

TEST(NumUnaryTest, PlusNum) {
    std::string code = R"(print(+++++1))";

    std::string expected = "1";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumUnaryTest, MinusNum) {
    std::string code = R"(print(-1))";

    std::string expected = "-1";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumUnaryTest, DoubleMinusNum) {
    std::string code = R"(print(--1))";

    std::string expected = "1";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumUnaryTest, NotNumFalse) {
    std::string code = R"(print(not 1))";

    std::string expected = "false";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumUnaryTest, NotNumTrue) {
    std::string code = R"(print(not 0))";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumUnaryTest, NotStringTrue) {
    std::string code = R"(print(not ""))";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumUnaryTest, NotStringFalse) {
    std::string code = R"(print(not "abc"))";

    std::string expected = "false";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumUnaryTest, NotBool) {
    std::string code = R"(print(not true))";

    std::string expected = "false";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumUnaryTest, NotNil) {
    std::string code = R"(print(not nil))";

    std::string expected = "nil";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumUnaryTest, PlusMinusNil) {
    std::string code = R"(print(+-+nil))";

    std::string expected = "nil";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
