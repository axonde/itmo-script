#include <interpreter>
#include <gtest/gtest.h>

using namespace Interpreter;

TEST(NumStandartLibraryTest, Abs) {
    std::string code = R"( print(abs(-1)) )";

    std::string expected = "1";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumStandartLibraryTest, Ceil) {
    std::string code = R"( print(ceil(4.43)) )";

    std::string expected = "5";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumStandartLibraryTest, Floor) {
    std::string code = R"(
        print(floor(2.5))
    )";

    std::string expected = "2";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}


TEST(NumStandartLibraryTest, Round) {
    std::string code = R"(
        print(round(2.5), round(2.4))
    )";

    std::string expected = "3 2";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumStandartLibraryTest, Sqrt) {
    std::string code = R"(
        print(sqrt(4), sqrt(2))
    )";

    std::string expected = "2 1.41421";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}