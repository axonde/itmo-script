#pragma once
#include <memory>

#include "lexer.h"

class Parser {

    /// @brief Own string, number, nil, logic literals.
    struct Node {
        Node(Lexer::Token&& t) : token(std::forward<Lexer::Token>(t)) {}
        Lexer::Token token;
    };
    struct Var {
        Var(Lexer::Token&& t) : token(std::forward<Lexer::Token>(t)) {}
        Lexer::Token token;
    };

    struct NoOp {};

    template<typename T>
    struct UnaryOp {
        UnaryOp(Lexer::Token&& t, T&& c)
        : token(std::forward<Lexer::Token>(t)), child(std::forward<T>(c)) {}
        Lexer::Token token;
        T child;
    };

    template<typename T, typename U>
    struct BinaryOp {
        BinaryOp(Lexer::Token&& t, T&& l, U&& r)
        : token(std::forward<Lexer::Token>(t)), left(std::forward<T>(l)), right(std::forward<U>(r)) {}
        Lexer::Token token;
        T left;
        U right;
    };

    template<typename T>
    struct AssignmentStatement {
        AssignmentStatement(Var&& v, T&& vl)
        : var(std::forward<Var>(v)), value(std::forward<T>(vl)) {}
        Var var;
        T value;
    };

public:
    bool Eat();

    std::unique_ptr<Node> Factor();
    std::unique_ptr<Node> Term();
    std::unique_ptr<Node> Expr();
    std::unique_ptr<Node> Empty();
    std::unique_ptr<Node> AssignStatement();
    std::unique_ptr<Node> Statement();
    std::unique_ptr<Node> StatementList();

    Lexer::Tokenizer tokenizer;
};