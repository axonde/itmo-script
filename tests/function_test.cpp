#include <interpreter>
#include <gtest/gtest.h>

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

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
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

    Interpreter interpreter(input, output);

    ASSERT_TRUE(interpreter.Interpret(input, false));
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

    Interpreter interpreter(input, output);
    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, FunnySyntaxTestInWidth) {
    std::string code = R"(
        funcs = [function() return 1 end function, function() return 2 end function, function() return 3 end function]

        print(funcs[0]())
        print(funcs[1]())
        print(funcs[2]())
    )";

    std::string expected = "123";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);
    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, FunnySyntaxTestInDepth) {
    std::string code = R"(
        funcs = [
            function() return 1 end function,
            function() return 2 end function,   // support space initializing
            function() return 3 end function,
        ]

        print(funcs[0]())
        print(funcs[1]())
        print(funcs[2]())
    )";

    std::string expected = "123";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);
    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, ReturnNestedFunc) {
    std::string code = R"(
        nested_func = function()
            return function()
                return function()
                    return 1
                end function
            end function
        end function

        print(nested_func()()())
    )";

    std::string expected = "1";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);
    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, MultipleCall) {
    // This is not a simple test. This is a correct behaviour that have been violated previously
    std::string code = R"(
        func = function()
            b = function()
                print(1)
            end function
            b()
        end function

        func() func() func() func() func()
    )";

    std::string expected = "11111";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);
    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, AnonimousFunctionCall) {
    std::string code = R"(
        print(function()
            return "passed"
        end function())
    )";

    std::string expected = "\"passed\"";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);
    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, AnonimousFunctionCallOneLine) {
    std::string code = R"(
        print(function() return "itmo" + " = love" end function())
    )";

    std::string expected = "\"itmo = love\"";

    std::istringstream input(code);
    std::ostringstream output;

    Interpreter interpreter(input, output);
    ASSERT_TRUE(interpreter.Interpret(input, false));
    ASSERT_EQ(output.str(), expected);
}

