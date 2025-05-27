#include "parser.h"

bool Parser::Eat(Lexer::Tokens token_type) {
    tokens_traits.push(token);
    if (token.token == token_type) {
        tokenizer >> token;
        return true;
    }
    return false;
}
Lexer::Token Parser::GetTraitedToken() {
    if (tokens_traits.empty()) { return token; }
    Lexer::Token tkn = tokens_traits.top();
    tokens_traits.pop();
    return tkn;
}
std::unique_ptr<Parser::Node> Parser::MakeBadNode() {
    return std::make_unique<Bad>(GetTraitedToken());
}

std::unique_ptr<Parser::Node> Parser::Factor() {
    using namespace Lexer;

    if (token.token == Tokens::T_NUMBER) {
        double value = std::get<double>(token.value);
        if (!Eat(token.token)) return MakeBadNode();
        return std::make_unique<NumLiteral>(std::move(value), GetTraitedToken());
    }
    if (token.token == Tokens::T_STRING) {
        std::string value = std::get<std::string>(token.value);
        if (!Eat(token.token)) return MakeBadNode();
        return std::make_unique<StringLiteral>(std::move(value), GetTraitedToken());
    }
    if (token.token == Tokens::T_VAR) {
        std::string id = std::get<std::string>(token.value);
        if (!Eat(token.token)) return MakeBadNode();
        return std::make_unique<Var>(std::move(id), GetTraitedToken());
    }
    if (token.token == Tokens::T_NIL) {
        if (!Eat(token.token)) return MakeBadNode();
        return std::make_unique<Nil>(GetTraitedToken());
    }
    if (token.token == Tokens::T_MINUS || token.token == Tokens::T_PLUS) {
        Tokens op = token.token;
        if (!Eat(token.token)) return MakeBadNode();
        return std::make_unique<UnaryOp>(op, Factor(), GetTraitedToken());
    }
    if (token.token == Tokens::T_LEFT_BRACKET) {
        if (!Eat(token.token)) return MakeBadNode();
        auto expr = Expr();
        if (!Eat(Tokens::T_RIGHT_BRACKET)) return MakeBadNode();
        return expr;
    }
    if (token.token == Tokens::T_BAD) {
        return MakeBadNode();
    }
    return MakeBadNode();
}
std::unique_ptr<Parser::Node> Parser::Term() {
    std::unique_ptr<Parser::Node> term = Factor();
    while (token.token == Lexer::Tokens::T_MULT || token.token == Lexer::Tokens::T_DIV) {
        auto op = token.token;
        if (!Eat(token.token)) { return MakeBadNode(); }
        term = std::make_unique<BinaryOp>(op, std::move(term), Term(), GetTraitedToken());
    }
    return term;
}
std::unique_ptr<Parser::Node> Parser::Expr() {
    std::unique_ptr<Node> expr = Term();
    while (token.token == Lexer::Tokens::T_MINUS || token.token == Lexer::Tokens::T_PLUS) {
        auto op = token.token;
        if (!Eat(token.token)) { return MakeBadNode(); }
        expr = std::make_unique<BinaryOp>(op, std::move(expr), Term(), GetTraitedToken());
    }
    return expr;
}
std::unique_ptr<Parser::Node> Parser::Empty() {
    switch (token.token) {
        case Lexer::Tokens::T_EOL:
        case Lexer::Tokens::T_EOF:
            if (!Eat(token.token)) { return MakeBadNode(); }
            return std::make_unique<NoOp>(GetTraitedToken());
        default:
            return MakeBadNode();
    }
}
std::unique_ptr<Parser::Node> Parser::AssignmentStatement() {
    Var var(std::get<std::string>(token.value), GetTraitedToken());
    if (!Eat(Lexer::Tokens::T_VAR)) { return MakeBadNode(); }

    if (!Eat(Lexer::Tokens::T_EQUAL)) { return MakeBadNode(); }

    return std::make_unique<AssignOp>(std::move(var), Expr(), GetTraitedToken());
}
std::unique_ptr<Parser::Node> Parser::Statement() {
    switch (token.token) {
        case Lexer::Tokens::T_VAR:
            return AssignmentStatement();
        case Lexer::Tokens::T_EOL:
        case Lexer::Tokens::T_EOF:
            return Empty();
        default:
            return Expr();
    }
}
std::unique_ptr<Parser::Node> Parser::StatementList() {
    Compound node(GetTraitedToken());
    node.children.push_back(Statement());
    while (token.token == Lexer::Tokens::T_EOL) {
        if (!Eat(Lexer::Tokens::T_EOL)) { return MakeBadNode(); }
        node.children.push_back(Statement());
    }
    return std::make_unique<Compound>(std::move(node));
}

void Parser::Parse() {
    tokenizer >> token;
    root = StatementList();
}