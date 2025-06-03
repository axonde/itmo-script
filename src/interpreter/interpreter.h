#pragma once
#include <any>
#include <cmath>
#include <concepts>
#include <expected>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <unordered_map>

#include "lexer.h"
#include "parser.h"
#include "operators.h"
#include "memory.h"

namespace Interpreter {
    bool Interpret(std::istream&, std::ostream&);
    bool Interpret(std::string&, std::istream&, std::ostream&);

    extern std::unique_ptr<Memory::StackFrame> stack_frame;
    extern std::istream* in;
    extern std::ostream* out;

    void Init();

    // ERRORS
    void SyntaxError(const Lexer::Token& token);
    void RunTimeError(const Lexer::Token&);
}

class Runner {
public:
    template<typename T>
    requires std::same_as<T, std::string>
    Runner(T&& str)
    : parser(Lexer::Tokenizer( std::forward<T>(str) )) {
        parser.Parse();
    }

    using HolderPack = Memory::HolderPack;
    using Expected = std::expected<HolderPack, Lexer::Token>;
    Expected Run();

    Parser::NodePtr& GetRoot() { return parser.root; }

private:
    using Holder = Memory::Holder;
    using NodePtr = Parser::NodePtr;
    using RawHolderPack = Memory::RawHolderPack;

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

    Expected VisitReturn(NodePtr&);
    Expected VisitBreak(NodePtr&);
    Expected VisitContinue(NodePtr&);
    
    Expected VisitFunc(NodePtr&);
    Expected VisitFuncCall(NodePtr&);
    Expected VisitUserFuncCall(Parser::FuncCall*, Memory::FuncHolder&, std::vector<HolderPack>&);
    Expected VisitBuiltInFuncCall(Parser::FuncCall*, Memory::FuncHolder&, std::vector<HolderPack>&);


    Expected VisitCompound(NodePtr&);

    // Helpers
    std::expected<int, Lexer::Token> IntegerRequirement(NodePtr&);
    std::expected<int, Lexer::Token> GetIndex(NodePtr&, HolderPack&);
    Expected SubscriptIndexer(Parser::Subscript*, HolderPack&&);
    Expected SubscriptSlicer(Parser::Subscript*, HolderPack&&);

    Parser parser;
};
