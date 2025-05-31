#pragma once
#include <concepts>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <expected>
#include <variant>
#include <unordered_map>

#include "lexer.h"
#include "parser.h"
#include "operators.h"
#include "memory.h"

namespace Interpreter {
    inline bool Interpret(std::istream&, std::ostream&, bool);
    inline bool Interpret(std::string&, std::ostream&);

    // ERRORS
    void SyntaxError(const Lexer::Token& token) {
        using ErrorType = std::shared_ptr<Errors::Error>;
        Errors::PrintError("Syntax error", std::get<ErrorType>(token.value).get(), token.column, token.lineno);
    }
}

class Runner {
public:
    template<typename T>
    requires std::same_as<T, std::string>
    Runner(T&& str, std::ostream& o)
    : parser(Lexer::Tokenizer( std::forward<T>(str) )), out(o), stack_frame(BUILT_IN_FUNCTIONS) {
        parser.Parse();
    }

    using Holder = Memory::Holder;
    using Expected = Operators::Expected;

    Expected Run();

private:
    using NodePtr = std::unique_ptr<Parser::Node>;

    Expected Visit(NodePtr&);

    Expected VisitNumLiteral(NodePtr&);
    Expected VisitStringLiteral(NodePtr&);
    Expected VisitBoolLiteral(NodePtr&);
    Expected VisitNilLiteral(NodePtr&);
    
    Expected VisitVar(NodePtr&);
    Expected VisitList(NodePtr&);

    Expected VisitUnaryOp(NodePtr&);
    Expected VisitBinaryOp(NodePtr&);
    Expected VisitSubscript(NodePtr&);

    Expected VisitIf(NodePtr&);
    Expected VisitFor(NodePtr&);
    Expected VisitWhile(NodePtr&);
    Expected VisitBreak(NodePtr&);
    Expected VisitContinue(NodePtr&);
    
    Expected VisitFunc(NodePtr&);
    Expected VisitFuncCall(NodePtr&);
    Expected VisitReturn(NodePtr&);
    Expected VisitCompound(NodePtr&);

    Parser parser;
    Memory::StackFrame stack_frame;
    std::ostream& out;
};
