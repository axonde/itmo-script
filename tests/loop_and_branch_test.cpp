#include <interpreter>
#include <gtest/gtest.h>

using namespace Interpreter;

TEST(BranchTestSuite, SimpleIfTest) {
    std::string code = R"(
        cond = true
        if cond then
            print("true")
        end if
    )";

    std::string expected = "\"true\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(BranchTestSuite, SimpleElseIfTest) {
    std::string code = R"(
        cond = false
        if cond then
            print("true")
        else
            print("false")
        end if
    )";

    std::string expected = "\"false\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}


TEST(BranchTestSuite, ComplexIfTest) {
    std::string code = R"(
        v = 100 * 2 + 10 * 3 + 9
        if v == 30 then
            print(30)
        else if v == 366 then
            print(366)
        else if v == 239 then
            print(239)
        else
            print(0)
        end if
    )";

    std::string expected = "239";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}


TEST(BranchTestSuite, OneLineIfTest) {
    std::string code = "if 2 * 2 == 4 then print(\"2 * 2 == 4\") else print(\"omg\") end if";
    std::string expected = "\"2 * 2 == 4\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}


TEST(LoopTestSuit, ForLoop) {
    std::string code = R"(
        for i in range(0,5,1)
            print(i)
        end for
    )";

    std::string expected = "01234";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}


TEST(LoopTestSuit, WhileLoop) {
    std::string code = R"(
        s = "ITMO"
        while len(s) < 12
            s = s * 2
        end while
        print(s)
    )";

    std::string expected = "\"ITMOITMOITMOITMO\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
