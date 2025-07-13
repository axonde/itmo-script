#include <interpreter>
#include <gtest/gtest.h>

TEST(IndexTest, GetIndex) {
    std::string code = R"(
        a = "this is a string"
        print(a[1])
    )";

    std::string expected = "\"h\"";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(IndexTest, OutOfRange) {
    std::string code = R"(
        a = "this is a string"
        print(a[100])
    )";

    std::istringstream input(code);
    std::ostringstream output;
    std::ostringstream error;

    Interpreter interpreter(input, output, error);

    ASSERT_FALSE(interpreter.Interpret(input, false));
}

TEST(SlicesTest, SliceConcept) {
    std::string code = R"(
        a = [1, 2, 3]
        b = a[0:2]
        print(b)
    )";

    std::string expected = "[1, 2]";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(SlicesTest, SimpleCopy) {
    std::string code = R"(
        a = [1, 2, 3]
        b = a[:]
        c = a[::]
        print((b == a) and (c == a))
    )";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(SlicesTest, AssignmentNewInstance) {
    std::string code = R"(
        a = [1, 2, 3]
        b = a[0:2]
        b = [5, 5]
        print(a)
    )";

    std::string expected = "[1, 2, 3]";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
