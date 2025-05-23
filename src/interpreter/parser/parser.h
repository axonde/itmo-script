#pragma once
#include <memory>

#include "lexer.h"

class Parser {
    struct Node {};

    

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