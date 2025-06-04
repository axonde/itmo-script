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

TEST(NumStandartLibraryTest, Rnd) {
    std::string code = R"(
        a = rnd(100)
        if (a > 0) and (a < 100) then
            print("good")
        end if
    )";

    std::string expected = "\"good\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumStandartLibraryTest, ParseNum) {
    std::string code = R"(
        a = "12.23"
        print(parse_num(a))
    )";

    std::string expected = "12.23";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(NumStandartLibraryTest, ToString) {
    std::string code = R"(
        a = 12.23
        print(to_string(a))
    )";

    std::string expected = "\"12.230000\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}