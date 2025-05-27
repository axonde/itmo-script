#pragma once
#include <concepts>
#include <cstdint>
#include <memory>
#include <optional>
#include <variant>
#include <stack>

#include "lexer.h"

class Parser {
public:
    enum Nodes : uint16_t {
        N_NUM_LITERAL,
        N_STRING_LITERAL,
        N_VAR,
        N_NIL,

        N_UNARY_OP,
        N_BINARY_OP,

        N_ASSIGNMENT_OP,
        
        N_EMPTY,
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
        Node(Nodes n, Lexer::Token&& t) : node(n), type(Types::NOT_SET_TYPE), token(std::move(t)) {}
        Node(Nodes n, Types t, Lexer::Token&& tkn) : node(n), type(t), token(std::move(tkn)) {}

        Nodes node;
        Types type;
        Lexer::Token token;
    };

    struct NumLiteral : Node {
        NumLiteral(double v, Lexer::Token&& token) : Node(Nodes::N_NUM_LITERAL, Types::NUM_TYPE, std::move(token)), value(v) {}
        double value;
    };
    struct StringLiteral : Node {
        StringLiteral(const std::string& str, Lexer::Token&& token)
        : Node(Nodes::N_STRING_LITERAL, Types::STRING_TYPE, std::move(token)), value(str) {}
        StringLiteral(std::string&& str, Lexer::Token&& token)
        : Node(Nodes::N_STRING_LITERAL, Types::STRING_TYPE, std::move(token)), value(std::move(str)) {}

        std::string value;
    };
    struct Var : Node {
        Var(const std::string& id, Lexer::Token&& token) : Node(Nodes::N_VAR, std::move(token)), id(id) {}
        Var(std::string&& id, Lexer::Token&& token) : Node(Nodes::N_VAR, std::move(token)), id(std::move(id)) {}

        std::string id;
    };
    struct Nil : Node {
        Nil(Lexer::Token&& token) : Node(Nodes::N_NIL, Types::NIL_TYPE, std::move(token)) {}
    };

    struct UnaryOp : Node {
        UnaryOp(Lexer::Tokens o, std::unique_ptr<Node>&& node, Lexer::Token&& token)
        : Node(Nodes::N_UNARY_OP, std::move(token)), op(o), child(std::move(node)) {}

        Lexer::Tokens op;
        std::unique_ptr<Node> child;
    };
    struct BinaryOp : Node {
        BinaryOp(Lexer::Tokens o, std::unique_ptr<Node>&& l, std::unique_ptr<Node>&& r, Lexer::Token&& token)
        : Node(Nodes::N_BINARY_OP, std::move(token))
        , op(o), left(std::move(l)), right(std::move(r)) {}

        Lexer::Tokens op;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
    };
    struct AssignOp : Node {
        AssignOp(Var&& v, std::unique_ptr<Node> e, Lexer::Token&& token)
        : Node(Nodes::N_ASSIGNMENT_OP, std::move(token)),
          var(std::move(v)), expr(std::move(e)) {}

        Var var;
        std::unique_ptr<Node> expr;
    };

    struct Empty : Node {
        Empty(Lexer::Token&& token) : Node(Nodes::N_EMPTY, std::move(token)) {}
    };
    struct Compound : Node {
        Compound(Lexer::Token&& token) : Node(Nodes::N_COMPOUND, std::move(token)) {}
        std::vector<std::unique_ptr<Node>> children;
    };
    struct Bad : Node {
        Bad(Lexer::Token&& token) : Node(Nodes::N_BAD, std::move(token)) {}
    };

    Parser(Lexer::Tokenizer&& t) : tokenizer(t) {}

    bool Eat(Lexer::Tokens);
    Lexer::Token GetTraitedToken();
    std::unique_ptr<Node> MakeBadNode();

    template<typename T>
    requires std::derived_from<T, Error>
    std::unique_ptr<Node> MakeBadNode(T&& e);

    std::unique_ptr<Node> Factor();
    std::unique_ptr<Node> Term();
    std::unique_ptr<Node> Expr();

    std::unique_ptr<Node> RValueStatement();
    std::unique_ptr<Node> AssignmentStatement(Var&& var);
    std::unique_ptr<Node> Statement();

    std::unique_ptr<Node> VarDecomposition();
    std::unique_ptr<Node> Block();

    void Parse();

    std::unique_ptr<Node> root;

private:
    Lexer::Token token;
    std::stack<Lexer::Token> tokens_traits;
    Lexer::Tokenizer tokenizer;

    friend Node;
};