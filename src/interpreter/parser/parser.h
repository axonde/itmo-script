#pragma once
#include <cstdint>
#include <memory>
#include <variant>

#include "lexer.h"

class Parser {

    enum Nodes : uint16_t {
        N_NUM_LITERAL,
        N_STRING_LITERAL,
        N_VAR,

        N_NO_OP,
        N_UNARY_OP,
        N_BINARY_OP,

        N_ASSIGNMENT_STATE,

        N_COMPOUND
    };

    struct Node {
        Node(Nodes t) : type(t), value(std::monostate{}) {}
        Node(Nodes t, double v) : type(t), value(v) {}
        Node(Nodes t, const std::string& v) : type(t), value(v) {}
        Node(Nodes t, std::string&& v) : type(t), value(std::move(v)) {}

        Nodes type;
        std::variant<std::monostate, double, std::string> value;
    };

    struct NumLiteral : Node {
        NumLiteral(double value) : Node(Nodes::N_NUM_LITERAL, value) {}
    };
    struct StringLiteral : Node {
        StringLiteral(const std::string& value) : Node(Nodes::N_STRING_LITERAL, value) {}
        StringLiteral(std::string&& value) : Node(Nodes::N_STRING_LITERAL, std::move(value)) {}
    };
    struct Var : Node {
        Var(double value) : Node(Nodes::N_VAR, value) {}
    };

    struct NoOp : Node {
        NoOp() : Node(Nodes::N_NO_OP) {}
    };
    struct UnaryOp : Node {
        template<typename T>
        requires std::derived_from<T, Node>
        UnaryOp(T&& n) : Node(Nodes::N_UNARY_OP), child(std::make_unique<T>(std::forward<T>(n))) {}

        std::unique_ptr<Node> child;
    };
    struct BinaryOp : Node {
        template<typename T, typename U>
        requires std::derived_from<T, Node> && std::derived_from<U, Node>
        BinaryOp(T&& l, U&& r)
        : Node(Nodes::N_BINARY_OP), left(std::make_unique<T>(std::forward<T>(l))), right(std::make_unique<U>(std::forward<U>(r))) {}

        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
    };

    struct AssignmentStatement : Node {
        template<typename T>
        requires std::derived_from<T, Node>
        AssignmentStatement(Var&& v, T&& vl)
        : Node(Nodes::N_ASSIGNMENT_STATE), var(std::move(v)), value(std::forward<T>(vl)) {}

        Var var;
        std::unique_ptr<Node> value;
    };

    struct Compound : Node {
        template<typename... Args>
        requires (std::derived_from<Args, Node> && ...)
        Compound(Args&&... args) : Node(Nodes::N_COMPOUND), children(std::make_unique<Args>(std::forward<Args>(args))...) {}

        std::vector<std::unique_ptr<Node>> children;
    };

public:
    bool Eat();

    Node Factor();
    Node Term();
    Node Expr();
    Node Empty();
    Node AssignStatement();
    Node Statement();
    Node StatementList();

    Lexer::Tokenizer tokenizer;
};