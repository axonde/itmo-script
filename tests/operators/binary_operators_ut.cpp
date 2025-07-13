#include <interpreter>
#include <gtest/gtest.h>

TEST(NumBinaryTest, AssignNum) {
    std::string code = R"(
        a = 12
        print(a)
    )";

    std::string expected = "12";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
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

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumBinaryTest, NumPlusNum) {
    std::string code = R"(print(1 + 1))";

    std::string expected = "2";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumMinusNum) {
    std::string code = R"(print(1 - 1))";

    std::string expected = "0";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumMultNum) {
    std::string code = R"(print(10 * 2))";

    std::string expected = "20";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumDivNum) {
    std::string code = R"(print(10 / 4))";

    std::string expected = "2.5";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumModNum) {
    std::string code = R"(print(10 % 4))";

    std::string expected = "2";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumXorNum) {
    std::string code = R"(print(10 ^ 3))";

    std::string expected = "1000";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumEqualPlusNum) {
    std::string code = R"(
        a = 1
        a += 1
        print(a)
    )";

    std::string expected = "2";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumEqualMinusNum) {
    std::string code = R"(
        a = 1
        a -= 1
        print(a)
    )";

    std::string expected = "0";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumEqualMultNum) {
    std::string code = R"(
        a = 1
        a *= 5
        print(a)
    )";

    std::string expected = "5";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumEqualDivNum) {
    std::string code = R"(
        a = 1
        a /= 5
        print(a)
    )";

    std::string expected = "0.2";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumEqualModNum) {
    std::string code = R"(
        a = 11
        a %= 5
        print(a)
    )";

    std::string expected = "1";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumEqualXorNum) {
    std::string code = R"(
        a = 2
        a ^= 5
        print(a)
    )";

    std::string expected = "32";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
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

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
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

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringBinaryTest, StringPlusString) {
    std::string code = R"(print("abc" + "123"))";

    std::string expected = "\"abc123\"";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringBinaryTest, StringMinusString) {
    std::string code = R"(print("ITMO is a good university, but hard" - ", but hard"))";

    std::string expected = "\"ITMO is a good university\"";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringBinaryTest, StringMultNum) {
    std::string code = R"(print("ITMO ITMO " * 1.5))";

    std::string expected = "\"ITMO ITMO ITMO \"";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringBinaryTest, StringPlusEqual) {
    std::string code = R"(
        a = "ITMO"
        a += " + Irkutsk"
        print(a)
    )";

    std::string expected = "\"ITMO + Irkutsk\"";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringBinaryTest, StringMinusEqual) {
    std::string code = R"(
        a = "ITMO + LETI"
        a -= " + LETI"
        print(a)
    )";

    std::string expected = "\"ITMO\"";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringBinaryTest, StringMultEqual) {
    std::string code = R"(
        a = "ITMO FOREVER "
        a *= 2
        print(a)
    )";

    std::string expected = "\"ITMO FOREVER ITMO FOREVER \"";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

TEST(ListBinaryTest, ListPlusList) {
    std::string code = R"(
        print([1, 2] + [function() end function])
    )";

    std::string expected = "[1, 2, <function>]";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(ListBinaryTest, ListPlusEqualList) {
    std::string code = R"(
        a = [1, 2]
        a += [true]
        print(a)
    )";

    std::string expected = "[1, 2, true]";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(ListBinaryTest, ListMultNum) {
    std::string code = R"(
        print([1, 2] * 2)
    )";

    std::string expected = "[1, 2, 1, 2]";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(ListBinaryTest, ListMultEqualNum) {
    std::string code = R"(
        a = [true, false]
        a *= 2
        print(a)
    )";

    std::string expected = "[true, false, true, false]";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
