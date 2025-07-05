#pragma once
#include <concepts>
#include <expected>
#include <iostream>
#include <memory>
#include <string>

#include "lexer.h"
#include "parser.h"
#include "memory.h"

class Runner {
public:
    Runner() = default;

    void operator<<(const std::string&);

    using HolderPack = Memory::HolderPack;
    using Expected = std::expected<HolderPack, Error>;
    Expected Run();

private:
    Lexer::Tokenizer tokenizer;

    using NodePtr = Parser::NodePtr;
    using ListHolderPtr = Memory::ListHolderPtr;
    using FuncHolderPtr = Memory::FuncHolderPtr;

    using FuncHolder = Memory::FuncHolder;

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
};

extern std::istream* in;
extern std::ostream* out;

class Interpreter {
public:
    /// @brief initialize the Interpreter
    /// @brief configure the input and output streams which are used in streams functions
    Interpreter(std::istream&, std::ostream&);

    /// @brief reads cmds from istream and puts the result of execution to ostream
    bool Interpret(std::istream&, std::ostream&);

    std::shared_ptr<Memory::StackFrame> stack_frame;

    size_t GetClosureSize() const;

private:
    Runner runner;
};


