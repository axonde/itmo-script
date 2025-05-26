#pragma once
#include <concepts>
#include <functional>
#include <iostream>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

#include "lexer.h"
#include "parser.h"
#include "operators.h"

namespace Interpreter {
    inline bool Interpret(std::istream&, std::ostream&);
    inline bool Interpret(std::string&, std::ostream&);

    // ERRORS
    void SyntaxError(const Lexer::Token& token) {
        using ErrorType = std::shared_ptr<Errors::Error>;
        Errors::PrintError("Syntax error", std::get<ErrorType>(token.value).get(), token.pos, token.line);
    }
}

class Runner {
public:
    template<typename T>
    requires std::same_as<T, std::string>
    Runner(T&& str)
    : parser(Lexer::Tokenizer( std::forward<T>(str) )) {
        parser.Parse();
    }

    using Value = Operators::Value;
    using Expected = Operators::Expected;

    Expected Run();

private:
    using NodePtr = std::unique_ptr<Parser::Node>;

    Expected Visit(NodePtr&);

    Expected VisitNumLiteral(NodePtr&);
    Expected VisitStringLiteral(NodePtr&);
    Expected VisitVar(NodePtr&);
    Expected VisitNil(NodePtr&);

    Expected VisitNoOp(NodePtr&);
    Expected VisitUnaryOp(NodePtr&);
    Expected VisitBinaryOp(NodePtr&);

    Expected VisitAssignmentOp(NodePtr&);

    Expected VisitCompound(NodePtr&);
    Expected VisitBad(NodePtr&);

    Parser parser;
};
