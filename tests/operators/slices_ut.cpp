#include <interpreter>
#include <gtest/gtest.h>

using namespace Interpreter;

TEST(IndexTest, GetIndex) {
    std::string code = R"(
        a = "this is a string"
        print(a[1])
    )";

    std::string expected = "\"h\"";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
TEST(IndexTest, OutOfRange) {
    std::string code = R"(
        a = "this is a string"
        print(a[100])
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_FALSE(Interpret(input, output));
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

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
// TEST(SlicesTest, SimpleCopy) {  /// NEED TO FIX CODE TO DOUBLE INDIRECTION PRINCIPE
//     std::string code = R"(
//         a = [1, 2, 3]
//         b = a[:]
//         c = a[::]
//         print((b == a) and (c == a))
//     )";

//     std::string expected = "true";

//     std::istringstream input(code);
//     std::ostringstream output;

//     ASSERT_TRUE(Interpret(input, output));
//     ASSERT_EQ(output.str(), expected);
// }
// TEST(SlicesTest, CopyByRef) {
//     std::string code = R"(
//         a = [1, 2, 3]
//         b = a[0:2]
//         b = [5, 5]
//         print(a)
//     )";

//     std::string expected = "";

//     std::istringstream input(code);
//     std::ostringstream output;

//     ASSERT_TRUE(Interpret(input, output));
//     ASSERT_EQ(output.str(), expected);
// }