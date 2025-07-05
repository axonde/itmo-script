#pragma once
#include "lexer.h"
#include "parser.h"
#include "memory.h"
#include <concepts>
#include <expected>
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
    using Expected = std::expected<HolderPack, Error>;

private:
    Lexer::Tokenizer tokenizer;
    std::unique_ptr<Memory::StackFrame> stack_frame;

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
    std::unexpected<Error> MakeError(const Error& e, Parser::Node* node);
    std::unexpected<Error> MakeError(const Error& e, Parser::NodePtr& node);

    template<typename E>
    requires std::derived_from<E, Error>
    std::unexpected<Error> MakeError(Parser::NodePtr& node);

    template<typename E>
    requires std::derived_from<E, Error>
    std::unexpected<Error> MakeError(Parser::Node* node);

    template<typename Func>
    requires std::invocable<Func>
    bool RunSafely(Func&&);

    std::expected<int, Error> IntegerRequirement(NodePtr&);
    std::expected<int, Error> GetIndex(NodePtr&, HolderPack&);
    Expected SubscriptIndexer(Parser::Subscript*, HolderPack&&);
    Expected SubscriptSlicer(Parser::Subscript*, HolderPack&&);
};

