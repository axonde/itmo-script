#include "parser.h"

bool Parser::Eat(Lexer::Tokens token_type) {
    if (token.token == token_type) {
        tokenizer >> token;
        return true;
    }
    return false;
}
std::unique_ptr<Parser::Node> Parser::MakeBadNode() {
    return std::make_unique<Bad>(std::move(token));
}

std::unique_ptr<Parser::Node> Parser::Term() {
    
}
std::unique_ptr<Parser::Node> Parser::Expr() {
    std::unique_ptr<Node> term = Term();
    while (token.token == Lexer::Tokens::T_MINUS || token.token == Lexer::Tokens::T_PLUS) {
        auto operator = token.token;
        if (!Eat(token.token)) { return MakeBadNode(); }
        term = std::make_unique<BinaryOp>(operator, std::move(term), Term());
    }
    return term;
}
std::unique_ptr<Parser::Node> Parser::Empty() {
    Lexer::Token curr_token = token;
    if (!Eat(Lexer::Tokens::T_EOL)) { return MakeBadNode(); }
    return std::make_unique<NoOp>()
}
std::unique_ptr<Parser::Node> Parser::AssignmentStatement() {
    Var var(std::get<std::string>(token.value));
    if (!Eat(Lexer::Tokens::T_VAR)) { return MakeBadNode(); }

    if (!Eat(Lexer::Tokens::T_EQUAL)) { return MakeBadNode(); }

    return std::make_unique<AssignOp>(std::move(var), Expr());
}
std::unique_ptr<Parser::Node> Parser::Statement() {
    switch (token.token) {
    case Lexer::Tokens::T_VAR:
        return AssignmentStatement();
    case Lexer::Tokens::T_EOL:
        return Empty();
    default:
        return Expr();
    }
}
std::unique_ptr<Parser::Node> Parser::StatementList() {
    Compound node;
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