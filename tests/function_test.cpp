#include <interpreter>
#include <gtest/gtest.h>

using namespace Interpreter;

TEST(FunctionTestSuite, SimpleFunctionTest) {
    std::string code = R"(
        incr = function(value)
            return value + 1
        end function

        x = incr(2)
        print(x)
    )";

    std::string expected = "3";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}


TEST(FunctionTestSuite, FunctionAsArgTest) {
    std::string code = R"(
        incr = function(value)
            return value + 1
        end function

        printresult = function(value, func)
            result = func(value)
            print(result)
        end function

        printresult(2, incr)
    )";

    std::string expected = "3";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}


TEST(FunctionTestSuite, NestedFunctionTest) {
    std::string code = R"(
        // NB: inner and outer `value` are different symbols.
        // You are not required to implement closures (aka lambdas).

        incr_and_print = function(value)
            incr = function(value)
                return value + 1
            end function

            print(incr(value))
        end function

        incr_and_print(2)
    )";

    std::string expected = "3";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, FunnySyntaxTest) {
    std::string code = R"(
        funcs = [function() return 1 end function, function() return 2 end function, function() return 3 end function]

        print(funcs[0]())
        print(funcs[1]())
        print(funcs[2]())
    )";

    std::string expected = "123";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

// TEST(FunctionTestSuite, FunnySyntaxTest) {
//     std::string code = R"(
//         funcs = [
//             function() return 1 end function,
//             function() return 2 end function,   // need to add \n support
//             function() return 3 end function,
//         ]

//         print(funcs[0]())
//         print(funcs[1]())
//         print(funcs[2]())
//     )";

//     std::string expected = "123";

//     std::istringstream input(code);
//     std::ostringstream output;

//     ASSERT_TRUE(Interpret(input, output));
//     ASSERT_EQ(output.str(), expected);
// }
