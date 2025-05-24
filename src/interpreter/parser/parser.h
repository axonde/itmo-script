#pragma once
#include <concepts>
#include <cstdint>
#include <memory>
#include <optional>
#include <variant>

#include "lexer.h"

class Parser {

public:
    enum Nodes : uint16_t {
        N_NUM_LITERAL,
        N_STRING_LITERAL,
        N_VAR,

        N_EMPTY,
        N_UNARY_OP,
        N_BINARY_OP,

        N_ASSIGNMENT_OP,

        N_COMPOUND,
        N_BAD
    };

private:
    struct Node {
        Node(Nodes t) : type(t) {}
        Nodes type;
    };

    struct NumLiteral : Node {
        NumLiteral(double v) : Node(Nodes::N_NUM_LITERAL), value(v) {}
        double value;
    };
    struct StringLiteral : Node {
        StringLiteral(const std::string& str) : Node(Nodes::N_STRING_LITERAL), value(str) {}
        StringLiteral(std::string&& str) : Node(Nodes::N_STRING_LITERAL), value(std::move(str)) {}
        std::string value;
    };
    struct Var : Node {
        Var(const std::string& id) : Node(Nodes::N_VAR), id(id) {}
        Var(std::string&& id) : Node(Nodes::N_VAR), id(std::move(id)) {}
        std::string id;
    };

    struct NoOp : Node {
        NoOp() : Node(Nodes::N_EMPTY) {}
    };
    struct UnaryOp : Node {
        UnaryOp(Lexer::Tokens o, std::unique_ptr<Node>&& node)
        : Node(Nodes::N_UNARY_OP), op(o), child(std::move(node)) {}

        Lexer::Tokens op;
        std::unique_ptr<Node> child;
    };
    struct BinaryOp : Node {
        BinaryOp(Lexer::Tokens o, std::unique_ptr<Node>&& l, std::unique_ptr<Node>&& r)
        : Node(Nodes::N_BINARY_OP), op(o), left(std::move(l)), right(std::move(r)) {}

        Lexer::Tokens op;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
    };
    struct AssignOp : Node {
        AssignOp(Var&& vr, std::unique_ptr<Node> vl)
        : Node(Nodes::N_ASSIGNMENT_OP),
          var(std::move(vr)), value(std::move(vl)) {}

        Var var;
        std::unique_ptr<Node> value;
    };

    struct Compound : Node {
        Compound() : Node(Nodes::N_COMPOUND) {}
        std::vector<std::unique_ptr<Node>> children;
    };
    struct Bad : Node {
        template<typename T>
        requires std::same_as<T, Lexer::Token>
        Bad(T&& t) : Node(Nodes::N_BAD), token(std::forward<T>(t)) {}
        Lexer::Token token;
    };

public:
    Parser(Lexer::Tokenizer&& t) : tokenizer(t) {}

    bool Eat(Lexer::Tokens);
    std::unique_ptr<Node> MakeBadNode();

    std::unique_ptr<Node> Factor();
    std::unique_ptr<Node> Term();
    std::unique_ptr<Node> Expr();
    std::unique_ptr<Node> Empty();
    std::unique_ptr<Node> AssignmentStatement();
    std::unique_ptr<Node> Statement();
    std::unique_ptr<Node> StatementList();

    void Parse();

private:
    std::unique_ptr<Node> root;
    Lexer::Token token;
    Lexer::Tokenizer tokenizer;
};