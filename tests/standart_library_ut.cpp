#include <interpreter>
#include <gtest/gtest.h>

using namespace Interpreter;

// NUM
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

// STRING
TEST(StringStandartLibraryTest, Lower) {
    std::string code = R"(
        a = "ITMO IS MORE THAN A UNIVERSITY"
        print(lower(a))
    )";

    std::string expected = "\"itmo is more than a university\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringStandartLibraryTest, Upper) {
    std::string code = R"(
        a = "itmo is more than a university"
        print(upper(a))
    )";

    std::string expected = "\"ITMO IS MORE THAN A UNIVERSITY\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringStandartLibraryTest, Split) {
    std::string code = R"(
        a = "itmo is more"
        print(split(a))
    )";

    std::string expected = "[\"itmo\", \"is\", \"more\"]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringStandartLibraryTest, Join) {
    std::string code = R"(
        list = ["itmo", "is", "more"]
        print(join(list, " * "))
    )";

    std::string expected = "\"itmo * is * more\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(StringStandartLibraryTest, Replace) {
    std::string code = R"(
        a = "itmo is more than an university!"
        print(replace(a, "more", "better"))
    )";

    std::string expected = "\"itmo is better than an university!\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

// LIST
TEST(ListStandartLibraryTest, Push) {
    std::string code = R"(
        a = ["base", true]
        push(a, 9)
        print(a)
    )";

    std::string expected = "[\"base\", true, 9]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(ListStandartLibraryTest, Pop) {
    std::string code = R"(
        a = ["base", true]
        print(pop(a))
    )";

    std::string expected = "true";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}