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
    bool Interpret(std::istream&, std::ostream&, bool);
    bool Interpret(std::string&, std::ostream&);

    // ERRORS
    void SyntaxError(const Lexer::Token& token);
    void RunTimeError(const Lexer::Token&);
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
    using HolderPack = Memory::HolderPack;
    using Expected = std::expected<HolderPack, Lexer::Token>;

    Expected Run();

    Parser::NodePtr& GetRoot() { return parser.root; }

private:
    using NodePtr = std::unique_ptr<Parser::Node>;
    using HolderTypes = Memory::HolderTypes;

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

    Expected VisitCompound(NodePtr&);

    // Helpers
    std::expected<int, Lexer::Token> GetIndex(NodePtr& node, HolderPack& var);
    Expected SubscriptIndexer(Parser::Subscript* ptr, HolderPack&& var);
    Expected SubscriptSlicer(Parser::Subscript* ptr, HolderPack&& var);

    Parser parser;
    Memory::StackFrame stack_frame;
    std::ostream& out;
};
