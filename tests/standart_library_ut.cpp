#include <interpreter>
#include <gtest/gtest.h>

class StandartLibraryTestFixture : public testing::Test {
protected:
    std::istringstream input;
    std::ostringstream output;
    std::ostringstream error;

    Interpreter interpreter;

    StandartLibraryTestFixture() : interpreter(input, output, error) {}

    void RunTest(std::string&& code) {
        input.str(code);

        ASSERT_FALSE(interpreter.Interpret(input, false));
    }
    void RunTest(std::string&& code, std::string&& expected) {
        input.str(code);

        ASSERT_TRUE(interpreter.Interpret(input, false));
        ASSERT_EQ(output.str(), expected);
    }
};

class NumStandartLibraryTest : public StandartLibraryTestFixture {};

// NUM
TEST_F(NumStandartLibraryTest, Abs) {
    RunTest(R"( print(abs(-1)) )", "1");
}
TEST_F(NumStandartLibraryTest, Ceil) {
    std::string code = R"( print(ceil(4.43)) )";

    RunTest(std::move(code), "5");
}
TEST_F(NumStandartLibraryTest, Floor) {
    std::string code = R"(
        print(floor(2.5))
    )";

    RunTest(std::move(code), "2");
}
TEST_F(NumStandartLibraryTest, Round) {
    std::string code = R"(
        print(round(2.5), round(2.4))
    )";

    RunTest(std::move(code), "3 2");
}
TEST_F(NumStandartLibraryTest, Sqrt) {
    std::string code = R"(
        print(sqrt(4), sqrt(2))
    )";

    std::string expected = "2 1.41421";

    RunTest(std::move(code), std::move(expected));
}
TEST_F(NumStandartLibraryTest, Rnd) {
    std::string code = R"(
        a = rnd(100)
        if (a >= 0) and (a <= 100) then
            print("good")
        end if
    )";

    std::string expected = "\"good\"";

    RunTest(std::move(code), std::move(expected));
}
TEST_F(NumStandartLibraryTest, ParseNum) {
    std::string code = R"(
        a = "12.23"
        print(parse_num(a))
    )";

    std::string expected = "12.23";

    RunTest(std::move(code), std::move(expected));
}
TEST_F(NumStandartLibraryTest, ToString) {
    std::string code = R"(
        a = 12.23
        print(to_string(a))
    )";

    std::string expected = "\"12.230000\"";

    RunTest(std::move(code), std::move(expected));
}

// STRING
class StringStandartLibraryTest : public StandartLibraryTestFixture {};

TEST_F(StringStandartLibraryTest, Lower) {
    std::string code = R"(
        a = "ITMO IS MORE THAN A UNIVERSITY"
        print(lower(a))
    )";

    std::string expected = "\"itmo is more than a university\"";

    RunTest(std::move(code), std::move(expected));
}
TEST_F(StringStandartLibraryTest, Upper) {
    std::string code = R"(
        a = "itmo is more than a university"
        print(upper(a))
    )";

    std::string expected = "\"ITMO IS MORE THAN A UNIVERSITY\"";

    RunTest(std::move(code), std::move(expected));
}
TEST_F(StringStandartLibraryTest, Split) {
    std::string code = R"(
        a = "itmo is more"
        print(split(a))
    )";

    std::string expected = "[\"itmo\", \"is\", \"more\"]";

    RunTest(std::move(code), std::move(expected));
}
TEST_F(StringStandartLibraryTest, Join) {
    std::string code = R"(
        list = ["itmo", "is", "more"]
        print(join(list, " * "))
    )";

    std::string expected = "\"itmo * is * more\"";

    RunTest(std::move(code), std::move(expected));
}
TEST_F(StringStandartLibraryTest, Replace) {
    std::string code = R"(
        a = "itmo is more than an university!"
        print(replace(a, "more", "better"))
    )";

    std::string expected = "\"itmo is better than an university!\"";

    RunTest(std::move(code), std::move(expected));
}

// LIST
class ListStandartLibraryTest : public StandartLibraryTestFixture {};

TEST_F(ListStandartLibraryTest, Push) {
    std::string code = R"(
        a = ["base", true]
        push(a, 9)
        print(a)
    )";

    std::string expected = "[\"base\", true, 9]";

    RunTest(std::move(code), std::move(expected));
}
TEST_F(ListStandartLibraryTest, Pop) {
    std::string code = R"(
        a = ["base", true]
        print(pop(a))
    )";

    std::string expected = "true";

    RunTest(std::move(code), std::move(expected));
}
TEST_F(ListStandartLibraryTest, Insert) {
    std::string code = R"(
        a = ["base", true]
        insert(a, 0, 1)
        insert(a, -1, 2)
        insert(a, 1000, 3)
        insert(a, -1000, 4)

        print(a)
    )";

    std::string expected = "[4, 1, \"base\", 2, true, 3]";

    RunTest(std::move(code), std::move(expected));
}
TEST_F(ListStandartLibraryTest, Remove) {
    std::string code = R"(
        a = ["base", true]
        print(remove(a, 0), remove(a, 1000), remove(a, -1))
    )";

    std::string expected = "\"base\" nil true";

    RunTest(std::move(code), std::move(expected));
}
TEST_F(ListStandartLibraryTest, Sort) {
    std::string code = R"(
        a = [5, 4, 3, 2]
        print(sort(a))
    )";

    std::string expected = "[2, 3, 4, 5]";

    RunTest(std::move(code), std::move(expected));
}
TEST_F(ListStandartLibraryTest, WrongSort) {
    std::string code = R"(
        a = [5, 4, 3, "123"]
        print(sort(a))
    )";

    RunTest(std::move(code));
}
TEST_F(ListStandartLibraryTest, Copy) {
    std::string code = R"(
        a = ["axonde"]
        b = copy(a)
        a[0] = "itmo"
        print(a, b)
    )";

    std::string expected = "[\"itmo\"] [\"axonde\"]";

    RunTest(std::move(code), std::move(expected));
}

// SYSTEM
class SystemStandartLibraryTest : public StandartLibraryTestFixture {};

TEST_F(SystemStandartLibraryTest, Exit) {
    std::string code = R"(
        print("ITMO")
        exit()
        print(" looser")
    )";

    std::string expected = "\"ITMO\"";

    RunTest(std::move(code), std::move(expected));
}
