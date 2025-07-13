#include <interpreter>
#include <gtest/gtest.h>

TEST(TypesTestSuite, IntTest) {
    std::string code = R"(
        x = 1
        y = 2
        z = 3 * x + y
        print(z)
    )";

    std::string expected = "5";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(TypesTestSuite, StringTest) {
    std::string code = R"(
        x = "type test-"
        y = " yupi"
        z = x * 2 + y
        print(z)
    )";

    std::string expected = "\"type test-type test- yupi\"";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(TypesTestSuite, BoolTest) {
    std::string code = R"(
        x = true
        y = false
        print(x or y)
    )";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(TypesTestSuite, NilTest) {
    std::string code = R"(
        a = [1, 2]
        b = 123
        c = function() end function
        a = nil
        b = nil
        c = nil
        print(a, b, c)
    )";

    std::string expected = "nil nil nil";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(TypesTestSuite, ListTest) {
    std::string code = R"(
        a = [1, 2, 3]
        b = [function()
                return 1
            end function,
            function()
                return "str"
            end function
        ]
        print(a + b)
    )";

    std::string expected = "[1, 2, 3, <function>, <function>]";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
TEST(TypesTestSuite, FuncTest) {
    std::string code = R"(
        a = function()
            return "Hello from func test!"
        end function
        print(a)
    )";

    std::string expected = "<function>";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}
