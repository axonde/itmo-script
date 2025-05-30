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

TEST(AST_SERIALIZATION, BasicVar) {
    std::string program = R"(
        a = 1
    )";

    Serializer serializer(Parser(Lexer::Tokenizer(std::move(program))));

    json expected = {
        {
            "children", {
                {
                    {"left", {
                        {"id", "a"},
                        {"type", "Var"}
                    }},
                    {"operator", "="},
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

TEST(AST_SERIALIZATION, FunctionDeclaration) {
    std::string program = R"(
        a = function(a, b)
            return a + b
        end function
    )";

    Serializer serializer(Parser(Lexer::Tokenizer(std::move(program))));

    json expected = {
        {"children", {{
            {"left", {
                {"id", "a"},
                {"type", "Var"}
            }},
            {"operator", "="},
            {"right", {
                {"body", {
                    {"children", {
                        {
                            {"expr", {
                                {"left", {
                                    {"id", "a"},
                                    {"type", "Var"}
                                }},
                                {"operator", "+"},
                                {"right", {
                                    {"id", "b"},
                                    {"type", "Var"}
                                }},
                                {"type", "Binary Op"}
                            }},
                            {"type", "return statement"}
                        }
                    }},
                    {"type", "compound"}
                }},
                {"params", {
                    {
                        {"id", "a"},
                        {"type", "Var"}
                    },
                    {
                        {"id", "b"},
                        {"type", "Var"}
                    }
                }},
                {"type", "function"}
            }},
            {"type", "Binary Op"}
        }}},
        {"type", "compound"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AST_SERIALIZATION, NestedFunctionCall) {
    std::string program = R"(
        a(b())
    )";

    Serializer serializer(Parser(Lexer::Tokenizer(std::move(program))));

    json expected = {
        {"children", {
            {
                {"func", {
                    {"id", "a"},
                    {"type", "Var"}
                }},
                {"params", {
                    {
                        {"func", {
                            {"id", "b"},
                            {"type", "Var"}
                        }},
                        {"params", {}},
                        {"type", "function call"}
                    }
                }},
                {"type", "function call"}
            }
        }},
        {"type", "compound"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AST_SERIALIZATION, ForCycle) {
    std::string program = R"(
        for i in range(1, 2)
            a += 1
        end for
    )";

    Serializer serializer(Parser(Lexer::Tokenizer(std::move(program))));

    json expected = {
        {"children", {
            {
                {"body", {
                    {"children", {{
                            {"left", {
                                {"id", "a"},
                                {"type", "Var"}
                            }},
                            {"operator", "+="},
                            {"right", {
                                {"type", "Num Literal"},
                                {"value", 1.0}
                            }},
                            {"type", "Binary Op"}
                    }}},
                    {"type", "compound"}
                }},
                {"iterator", {
                    {"id", "i"},
                    {"type", "Var"}
                }},
                {"range", {
                    {"func", {
                        {"id", "range"},
                        {"type", "Var"}
                    }},
                    {"params", {
                        {
                            {"type", "Num Literal"},
                            {"value", 1.0}
                        },
                        {
                            {"type", "Num Literal"},
                            {"value", 2.0}
                        }
                    }},
                    {"type", "function call"}
                }},
                {"type", "For Block"}
            }
        }},
        {"type", "compound"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AST_SERIALIZATION, WhileCycle) {
    std::string program = R"(
        while true
            print("while true!")
        end while
    )";

    Serializer serializer(Parser(Lexer::Tokenizer(std::move(program))));

    json expected = {
       {"children", {{
            {"body", {
                {"children", {{
                    {"func", {
                        {"id", "print"},
                        {"type", "Var"}
                    }},
                    {"params", {{
                        {"type", "String Literal"},
                        {"value", "while true!"}
                    }}},
                    {"type", "function call"}
                }}},
                {"type", "compound"}
            }},
            {"condition", {
                {"type", "Bool Literal"},
                {"value", "true"}
            }},
            {"type", "While Block"}
        }}},
        {"type", "compound"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AST_SERIALIZATION, IfStatement) {
    std::string program = R"(
        if cond == 1 then
            print("cool!")
        end if
    )";

    Serializer serializer(Parser(Lexer::Tokenizer(std::move(program))));

    json expected = {
        {"children", {{
            {"children", {{
                {"body", {
                    {"children", {{
                        {"func", {
                            {"id", "print"},
                            {"type", "Var"}
                        }},
                        {"params", {{
                            {"type", "String Literal"},
                            {"value", "cool!"}
                        }}},
                        {"type", "function call"}
                    }}},
                    {"type", "compound"}
                }},
                {"condition", {
                    {"left", {
                        {"id", "cond"},
                        {"type", "Var"}
                    }},
                    {"operator", "=="},
                    {"right", {
                        {"type", "Num Literal"},
                        {"value", 1.0}
                    }},
                    {"type", "Binary Op"}
                }},
                {"type", "If Block"}
            }}},
            {"type", "compound"}
        }}},
        {"type", "compound"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AST_SERIALIZATION, WrongSyntaxLiteral) {
    std::string program = R"(
        a = "sdfkjfkjdf
        b = 2
    )";

    Serializer serializer(Parser(Lexer::Tokenizer(std::move(program))));

    json expected = {
        {"error", "unclosed string literal"},
        {"type", "BAD"}
    };

    ASSERT_EQ(serializer.tree, expected);
}