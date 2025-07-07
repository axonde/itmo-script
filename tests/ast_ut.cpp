#include <serializer.h>
#include <gtest/gtest.h>

TEST(AstSerialization, OnePlusOne) {
    std::string program = "1 + 1";
    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

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

TEST(AstSerialization, BasicVar) {
    std::string program = R"(
        a = 1
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

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

TEST(AstSerialization, FunctionDeclaration) {
    std::string program = R"(
        a = function(a, b)
            return a + b
        end function
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

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
                {"args", {
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

TEST(AstSerialization, NestedFunctionCall) {
    std::string program = R"(
        a(b())
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

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

TEST(AstSerialization, ForCycle) {
    std::string program = R"(
        for i in range(1, 2)
            a += 1
        end for
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

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

TEST(AstSerialization, WhileCycle) {
    std::string program = R"(
        while true
            print("while true!")
        end while
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

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
                {"left", {
                    {"type", "Bool Literal"},
                    {"value", "true"}
                }},
                {"operator", "=="},
                {"right", {
                    {"type", "Bool Literal"},
                    {"value", "true"}
                }},
                {"type", "Binary Op"}
            }},
            {"type", "While Block"}
        }}},
        {"type", "compound"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AstSerialization, IfStatement) {
    std::string program = R"(
        if cond == 1 then
            print("cool!")
        end if
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

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
                    {"operator", "=="},
                    {"right", {
                        {"type", "Bool Literal"},
                        {"value", "true"}
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

TEST(AstSerialization, WrongSyntaxStringLiteral) {
    std::string program = R"(
        a = "sdfkjfkjdf
        b = 2
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

    json expected = {
        {"error", "unclosed string literal"},
        {"type", "BAD"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AstSerialization, WrongSyntaxNumLiteral) {
    std::string program = R"(
        a = 1123.
        b = 2
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

    json expected = {
        {"error", "wrong number literal"},
        {"type", "BAD"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AstSerialization, WrongSyntaxUndefinedSymbols) {
    std::string program = R"(!@!#)";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

    json expected = {
        {"error", "unrecognizable symbols"},
        {"type", "BAD"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AstSerialization, WrongSyntaxIfStatementExpThen) {
    std::string program = R"(
        if i < 3
            i = 3
        end if
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

    json expected = {
        {"error", "expected then"},
        {"type", "BAD"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AstSerialization, WrongSyntaxIfStatementExpExpr) {
    std::string program = R"(
        if then
            i = 3
        end if
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

    json expected = {
        {"error", "cannot correctly evaluate expression"},
        {"type", "BAD"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AstSerialization, WrongSyntaxIfStatementForgotEndIf) {
    std::string program = R"(
        if i < 3 then
            i = 3
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

    json expected = {
        {"error", "expected end if"},
        {"type", "BAD"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AstSerialization, WrongSyntaxForStatementExpIn) {
    std::string program = R"(
        for i range(3)
            print("Mega For")
        end for
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

    json expected = {
        {"error", "expected in"},
        {"type", "BAD"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AstSerialization, WrongSyntaxForStatementExpr) {
    std::string program = R"(
        for in range(3)
            print("Mega For")
        end for
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

    json expected = {
        {"error", "cannot correctly evaluate expression"},
        {"type", "BAD"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AstSerialization, WrongSyntaxForStatementRange) {
    std::string program = R"(
        for in
            continue
        end for
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

    json expected = {
        {"error", "cannot correctly evaluate expression"},
        {"type", "BAD"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AstSerialization, WrongSyntaxForStatementForgotEndFor) {
    std::string program = R"(
        for i in range(3)
            continue
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

    json expected = {
        {"error", "expected end for"},
        {"type", "BAD"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AstSerialization, AbnormalInput) {
    std::string program = R"(
        !@O#I1o] 	2-]9u j[9j4n ln;lkj;j]
    )";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

    json expected = {
        {"error", "unrecognizable symbols"},
        {"type", "BAD"}
    };

    ASSERT_EQ(serializer.tree, expected);
}

TEST(AstSerialization, EmptyInput) {
    std::string program = R"()";

    Lexer::Tokenizer tokenizer;
    tokenizer << program;
    Serializer serializer(Parser(std::move(tokenizer)));

    json expected = {
        {"children", {}},
        {"type", "compound"}
    };

    ASSERT_EQ(serializer.tree, expected);
}
