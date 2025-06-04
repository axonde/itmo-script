#include <interpreter>
#include <gtest/gtest.h>

using namespace Interpreter;

TEST(NumBinaryTest, AssignNum) {
    std::string code = R"(
        a = 12
        print(a)
    )";

    std::string expected = "12";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumBinaryTest, ReassignNum) {
    std::string code = R"(
        a = 12
        a = -12
        print(a)
    )";

    std::string expected = "-12";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumBinaryTest, NumPlusNum) {
    std::string code = R"(print(1 + 1))";

    std::string expected = "2";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumMinusNum) {
    std::string code = R"(print(1 - 1))";

    std::string expected = "0";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumMultNum) {
    std::string code = R"(print(10 * 2))";

    std::string expected = "20";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumDivNum) {
    std::string code = R"(print(10 / 4))";

    std::string expected = "2.5";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumModNum) {
    std::string code = R"(print(10 % 4))";

    std::string expected = "2";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumXorNum) {
    std::string code = R"(print(10 ^ 3))";

    std::string expected = "1000";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringBinaryTest, AssignString) {
    std::string code = R"(
        a = "abc"
        print(a)
    )";

    std::string expected = "\"abc\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringBinaryTest, ReassignString) {
    std::string code = R"(
        a = "abc"
        a = "123"
        print(a)
    )";

    std::string expected = "\"123\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringBinaryTest, StringPlusString) {
    std::string code = R"(print("abc" + "123"))";

    std::string expected = "\"abc123\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringBinaryTest, StringMinusString) {
    std::string code = R"(print("ITMO is a good university, but hard" - ", but hard"))";

    std::string expected = "\"ITMO is a good university\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringBinaryTest, StringMultNum) {
    std::string code = R"(print("ITMO ITMO " * 1.5))";

    std::string expected = "\"ITMO ITMO ITMO \"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
