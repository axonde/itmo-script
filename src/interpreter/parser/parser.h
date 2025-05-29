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
        N_BOOL_LITERAL,
        N_NIL_LITERAL,
        N_VAR,
        N_LIST,

        N_UNARY_OP,
        N_BINARY_OP,
        N_SUBSCRIPT,

        N_IF,

        N_FOR,
        N_WHILE,
        N_BREAK,
        N_CONTINUE,

        N_FUNC,
        N_FUNC_CALL,
        N_RETURN,

        N_COMPOUND,
        N_BAD,
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
        Node(Nodes n) : node(n) {}
        Node(Nodes n, Lexer::Token&& t) : node(n), type(Types::NOT_SET_TYPE), token(std::move(t)) {}
        Node(Nodes n, Types t, Lexer::Token&& tkn) : node(n), type(t), token(std::move(tkn)) {}

        Nodes node;
        Types type;
        Lexer::Token token;
    };

    using NodePtr = std::unique_ptr<Node>;

    /// Base
    struct Compound : Node {
        Compound() : Node(Nodes::N_COMPOUND) {}
        std::vector<NodePtr> data;
    };
    struct Bad : Node {
        Bad(Lexer::Token&& token) : Node(Nodes::N_BAD, std::move(token)) {}
    };

    /// Literals
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
    struct BoolLiteral : Node {
        BoolLiteral(bool v, Lexer::Token&& token)
        : Node(Nodes::N_BOOL_LITERAL, Types::BOOL_TYPE, std::move(token)), value(v) {}
        
        bool value;
    };
    struct NilLiteral : Node {
        NilLiteral(Lexer::Token&& token) : Node(Nodes::N_NIL_LITERAL, Types::NIL_TYPE, std::move(token)) {}
    };
    struct Var : Node {
        Var(const std::string& id, Lexer::Token&& token) : Node(Nodes::N_VAR, std::move(token)), id(id) {}
        Var(std::string&& id, Lexer::Token&& token) : Node(Nodes::N_VAR, std::move(token)), id(std::move(id)) {}

        std::string id;
    };
    struct List : Node {
        List(Lexer::Token&& token) : Node(Nodes::N_LIST, std::move(token)) {}

        Compound data;
    };

    /// Operators
    struct UnaryOp : Node {
        UnaryOp(Lexer::Tokens o, NodePtr&& node, Lexer::Token&& token)
        : Node(Nodes::N_UNARY_OP, std::move(token)), op(o), operand(std::move(node)) {}

        Lexer::Tokens op;
        NodePtr operand;
    };
    struct BinaryOp : Node {
        BinaryOp(Lexer::Tokens o, NodePtr&& l, NodePtr&& r, Lexer::Token&& token)
        : Node(Nodes::N_BINARY_OP, std::move(token))
        , op(o), left(std::move(l)), right(std::move(r)) {}

        Lexer::Tokens op;
        NodePtr left;
        NodePtr right;
    };
    struct Subscript : Node {
        Subscript(NodePtr&& v, NodePtr&& l, NodePtr&& r, bool i_s, Lexer::Token&& token)
        : Node(Nodes::N_SUBSCRIPT, std::move(token))
        , var_expr(std::move(v)), left(std::move(l)), right(std::move(r)), is_slice(i_s) {}

        NodePtr var_expr;
        NodePtr left;
        NodePtr right;
        bool is_slice;
    };

    struct If : Node {
        If(NodePtr&& c, NodePtr&& b, Lexer::Token token)
        : Node(Nodes::N_IF, std::move(token)), condition(std::move(c)), body(std::move(b)) {}

        NodePtr condition;  // should be a bool type in run-time
        NodePtr body;
    };

    /// Cycles
    struct For : Node {
        For(For&&) = default;
        For(NodePtr&& i, NodePtr&& r, NodePtr&& b, Lexer::Token token)
        : Node(Nodes::N_FOR, std::move(token)), iterator(std::move(i)), range(std::move(r)), body(std::move(b)) {}
        
        NodePtr iterator;  // should be a var symbol (+ num type) in run-time
        NodePtr range;
        NodePtr body;
    };
    struct While : Node {
        While(While&&) = default;
        While(NodePtr&& c, NodePtr&& b, Lexer::Token token)
        : Node(Nodes::N_WHILE, std::move(token)), condition(std::move(c)), body(std::move(b)) {}
        
        NodePtr condition;  // should be a bool type in run-time
        NodePtr body;
    };
    struct Break : Node {
        Break(Lexer::Token&& token) : Node(Nodes::N_BREAK, std::move(token)) {}
    };
    struct Continue : Node {
        Continue(Lexer::Token&& token) : Node(Nodes::N_CONTINUE, std::move(token)) {}
    };

    /// Functions
    struct Func : Node {
        Func(Var&& i, Compound&& p, NodePtr&& b, Lexer::Token&& token)
        : Node(Nodes::N_FUNC, std::move(token))
        , id(std::move(i)), params(std::move(p)), body(std::move(b)) {}

        Var id;
        Compound params;
        NodePtr body;
    };
    struct FuncCall : Node {
        FuncCall(FuncCall&&) = default;
        FuncCall(NodePtr&& f, Lexer::Token&& token) : Node(Nodes::N_FUNC_CALL, std::move(token)), func(std::move(f)) {}
    
        NodePtr func;
        std::vector<NodePtr> params;
    };
    struct Return : Node {
        Return(NodePtr&& ex, Lexer::Token&& token)
        : Node(Nodes::N_RETURN, std::move(token)), expr(std::move(ex)) {}

        NodePtr expr;
    };

    Parser(Lexer::Tokenizer&& t) : tokenizer(t) {}

    bool Eat(Lexer::Tokens);
    Lexer::Token GetTraitedToken();

    NodePtr MakeBadNode();
    template<typename T>
    requires std::derived_from<T, Error>
    NodePtr MakeBadNode(T&& e);

    NodePtr VarExpr();
    NodePtr FuncExpr();
    NodePtr ReturnExpr();

    NodePtr Factor();
    NodePtr Term();
    NodePtr Expr();

    NodePtr Assignment();
    NodePtr BreakExpr();
    NodePtr ContinueExpr();

    NodePtr Statement();
    NodePtr IfBlock();
    NodePtr ForBlock();
    NodePtr WhileBlock();

    NodePtr StatementList();
    NodePtr Block();

    NodePtr VarDispatch();

    void Parse();

    NodePtr root;

private:
    Lexer::Token token;
    std::stack<Lexer::Token> tokens_traits;
    Lexer::Tokenizer tokenizer;

    friend Node;
};