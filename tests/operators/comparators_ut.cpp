#include <interpreter>
#include <gtest/gtest.h>

using namespace Interpreter;

TEST(NumBinaryTest, IntEqInt) {
    std::string code = R"(print(1.1e-2 == 1.1e-2))";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, IntNonEqInt) {
    std::string code = R"(print(2.1e+3 - 1 != 2.1e+3))";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, IntSmallerInt) {
    std::string code = R"(print(1.1 < 2.1e+3))";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, IntSmallerOrEqInt) {
    std::string code = R"(print(1.1 <= 1.1))";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, IntGreaterInt) {
    std::string code = R"(print(1.1 > 1.1))";

    std::string expected = "false";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(NumBinaryTest, NumGreaterOrEqInt) {
    std::string code = R"(print(1.1231e-12 >= 1.1231e-12))";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringBinaryTest, StringEqString) {
    std::string code = R"( print("itmo+" == "itmo+") )";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringBinaryTest, StringNonEqString) {
    std::string code = R"(print("itmo-" != "itmo+") )";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringBinaryTest, StringSmallerString) {
    std::string code = R"(print("itmo+" < "itmo++"))";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringBinaryTest, StringSmallerOrEqString) {
    std::string code = R"(print("itmo+" <= "itmo+"))";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringBinaryTest, StringGreaterString) {
    std::string code = R"(print("itmo++++" > "itmo+"))";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringBinaryTest, StringGreaterOrEqString) {
    std::string code = R"(print("itmo+imto=imto++" >= "itmo+"))";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NilBinaryTest, NilNonEq) {
    std::string code = R"(
print( nil == 1, nil == "a", nil == true, nil == (function(a, b) end function), nil == [1, 2] ) )";

    std::string expected = "false false false false false";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(NilBinaryTest, NilEq) {
    std::string code = R"( print(nil == nil) )";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NilBinaryTest, ListEqList) {
    std::string code = R"( print([1, 2, 3] == [1, 2, 3]) )";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(BoolBinaryTest, BoolAnd) {
    std::string code = R"( print(true and true) )";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(BoolBinaryTest, BoolOr) {
    std::string code = R"( print(false or true) )";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(BoolBinaryTest, ComplexBool) {
    std::string code = R"(
        a = 1 b = 2
        print((a < 2) and (a <= 2))
    )";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ListBinaryTest, ListEq) {
    std::string code = R"( print([1, 2, [1, "str"]] == [1, 2, [1, "str"]]) )";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}