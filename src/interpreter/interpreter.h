#pragma once
#include "lexer.h"
#include "parser.h"
#include "memory.h"
#include <concepts>
#include <cstdint>
#include <iostream>

extern std::istream* in;
extern std::ostream* out;

class Interpreter {
public:
    /// @brief configure the input and output streams which are used in streams functions
    ///        and init the mandatory classes.
    Interpreter(std::istream& = std::cin, std::ostream& = std::cout, std::ostream& = std::cerr);

    /// @brief  reads cmds from istream and puts the result of execution to ostream
    /// @return true if at execution no error happend
    /// @return false if at least one error occured
    bool Interpret(std::istream&, std::ostream&, bool is_repl);

    bool InterpretFile(std::istream&, std::ostream&);
    bool InterpretRepl(std::istream&, std::ostream&);

    using HolderPack = Memory::HolderPack;

private:
    Lexer::Tokenizer tokenizer;
    std::unique_ptr<Memory::StackFrame> stack_frame;

    using NodePtr = Parser::NodePtr;
    using ListHolderPtr = Memory::ListHolderPtr;
    using FuncHolderPtr = Memory::FuncHolderPtr;

    using FuncHolder = Memory::FuncHolder;

    HolderPack Visit(NodePtr&);

    HolderPack VisitNumLiteral(NodePtr&);
    HolderPack VisitStringLiteral(NodePtr&);
    HolderPack VisitBoolLiteral(NodePtr&);
    HolderPack VisitNilLiteral(NodePtr&);

    HolderPack VisitVar(NodePtr&);
    HolderPack VisitList(NodePtr&);

    HolderPack VisitUnaryOp(NodePtr&);
    HolderPack VisitBinaryOp(NodePtr&);
    HolderPack VisitSubscript(NodePtr&);

    HolderPack VisitIf(NodePtr&);
    HolderPack VisitFor(NodePtr&);
    HolderPack VisitWhile(NodePtr&);

    HolderPack VisitReturn(NodePtr&);
    HolderPack VisitBreak(NodePtr&);
    HolderPack VisitContinue(NodePtr&);

    HolderPack VisitFunc(NodePtr&);
    HolderPack VisitFuncCall(NodePtr&);
    HolderPack VisitUserFuncCall(Parser::FuncCall*, Memory::FuncHolder&, std::vector<HolderPack>&);
    HolderPack VisitBuiltInFuncCall(Parser::FuncCall*, Memory::FuncHolder&, std::vector<HolderPack>&);


    HolderPack VisitCompound(NodePtr&);

    // Helpers
    template<typename E>
    requires std::derived_from<E, Error>
    E MakeError(Parser::NodePtr& node);

    template<typename E>
    requires std::derived_from<E, Error>
    E MakeError(Parser::Node* node);

    template<typename Func>
    requires std::invocable<Func>
    bool RunSafely(Func&&, std::vector<std::string>&);

    int64_t IntegerRequirement(NodePtr&);
    int64_t GetIndex(NodePtr&, HolderPack&);
    HolderPack SubscriptIndexer(Parser::Subscript*, HolderPack&&);
    HolderPack SubscriptSlicer(Parser::Subscript*, HolderPack&&);
};

