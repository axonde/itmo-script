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
        N_NIL,

        N_NO_OP,
        N_UNARY_OP,
        N_BINARY_OP,

        N_ASSIGNMENT_OP,

        N_COMPOUND,
        N_BAD
    };

    enum Types : uint16_t {
        NUM_TYPE,
        STRING_TYPE,
        NIL_TYPE,
        BOOL_TYPE,
        LIST_TYPE,
        FUNC_TYPE,
        NOT_SET_TYPE
    };

    struct Node {
        Node(Nodes n, Parser* parser) : node(n), type(Types::NOT_SET_TYPE), token(parser->debug_token) {}
        Node(Nodes n, Types t, Parser* parser) : node(n), type(t), token(parser->debug_token) {}

        Nodes node;
        Types type;
        Lexer::Token token;
    };

    struct NumLiteral : Node {
        NumLiteral(double v, Parser* parser) : Node(Nodes::N_NUM_LITERAL, Types::NUM_TYPE, parser), value(v) {}
        double value;
    };
    struct StringLiteral : Node {
        StringLiteral(const std::string& str, Parser* parser)
        : Node(Nodes::N_STRING_LITERAL, Types::STRING_TYPE, parser), value(str) {}
        StringLiteral(std::string&& str, Parser* parser)
        : Node(Nodes::N_STRING_LITERAL, Types::STRING_TYPE, parser), value(std::move(str)) {}

        std::string value;
    };
    struct Var : Node {
        Var(const std::string& id, Parser* parser) : Node(Nodes::N_VAR, parser), id(id) {}
        Var(std::string&& id, Parser* parser) : Node(Nodes::N_VAR, parser), id(std::move(id)) {}

        std::string id;
    };
    struct Nil : Node {
        Nil(Parser* parser) : Node(Nodes::N_NIL, Types::NIL_TYPE, parser) {}
    };

    struct NoOp : Node {
        NoOp(Parser* parser) : Node(Nodes::N_NO_OP, parser) {}
    };
    struct UnaryOp : Node {
        UnaryOp(Lexer::Tokens o, std::unique_ptr<Node>&& node, Parser* parser)
        : Node(Nodes::N_UNARY_OP, parser), op(o), child(std::move(node)) {}

        Lexer::Tokens op;
        std::unique_ptr<Node> child;
    };
    struct BinaryOp : Node {
        BinaryOp(Lexer::Tokens o, std::unique_ptr<Node>&& l, std::unique_ptr<Node>&& r, Parser* parser)
        : Node(Nodes::N_BINARY_OP, parser)
        , op(o), left(std::move(l)), right(std::move(r)) {}

        Lexer::Tokens op;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
    };
    struct AssignOp : Node {
        AssignOp(Var&& v, std::unique_ptr<Node> e, Parser* parser)
        : Node(Nodes::N_ASSIGNMENT_OP, parser),
          var(std::move(v)), expr(std::move(e)) {}

        Var var;
        std::unique_ptr<Node> expr;
    };

    struct Compound : Node {
        Compound(Parser* parser) : Node(Nodes::N_COMPOUND, parser) {}
        std::vector<std::unique_ptr<Node>> children;
    };
    struct Bad : Node {
        Bad(Parser* parser) : Node(Nodes::N_BAD, parser) {}
    };

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

    std::unique_ptr<Node> root;

private:
    Lexer::Token token;
    Lexer::Token debug_token;
    Lexer::Tokenizer tokenizer;

    friend Node;
};