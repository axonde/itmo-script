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
