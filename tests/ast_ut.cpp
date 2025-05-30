#include <serializer.h>
#include <gtest/gtest.h>

TEST(AST_SERIALIZATION, OnePlusOne) {
    std::string program = "1 + 1";

    Serializer serializer(Parser(Lexer::Tokenizer(std::move(program))));

    json expected = {
        {
            "children", {
                {
                    {"left", {
                        {"type", "Num Literal"},
                        {"value", 1.0}
                    }},
                    {"operator", "+"},
                    {"right", {
                        {"type", "Num Literal"},
                        {"value", 1.0}
                    }},
                    {"type", "Binary Op"}
                }
            }
        },
        {"type", "compound"}
    };

    ASSERT_EQ(serializer.tree, expected);
}