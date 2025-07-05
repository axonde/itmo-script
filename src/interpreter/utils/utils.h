#pragma once
#include <any>
#include <iostream>
#include <string>
#include <vector>

namespace Patterns {

static std::string RED = "\e[1;31m";
static std::string WHITE = "\e[0m";
static std::string BLUE = "\e[1;34m";

static std::string WHITEBOLD = "\e[1;37m";

static std::string CMD = "\e[3;34mitmo> \e[0m";

static std::string STACK_TRACE_HEADER = "STACK TRACE (from top to bottom)\n================================\n";

inline void Welcome() {
    std::cout << "Welcome to " + BLUE + "ITMO SCRIPT" + WHITE + "!" << std::endl;
}

}

namespace Errors {

struct Error {
    Error() = default;
    Error(const std::string& e) : error(e) {}
    Error(size_t l, size_t c) : lineno(l), column(c) {}
    Error(const std::string& e, size_t l, size_t c) : error(e), lineno(l), column(c) {}
    const char* what() const { return error.c_str(); }

    std::string error = "Panic!";
    size_t lineno = 1;
    size_t column = 1;
};

namespace CommonErrors {

inline void ErrorOpenFile() {
    std::cerr << Patterns::RED << "Error" << Patterns::WHITE << ": could not read the file." << std::endl;
}

} // end CommonErrors

namespace LexerErrors {

struct LexerError : Error {
    template<typename... Args>
    LexerError(Args&&... args) : Error(std::forward<Args>(args)...) {}
};

struct LexerNumberError final : LexerError {
    LexerNumberError(size_t lineno, size_t column) : LexerError(lineno, column) {
        error = "wrong number literal";
    }
};
struct LexerStringError final : LexerError {
    LexerStringError(size_t lineno, size_t column) : LexerError(lineno, column) {
        error = "unclosed string literal";
    }
};
struct LexerKeyWordError final : LexerError {
    LexerKeyWordError(size_t lineno, size_t column) : LexerError(lineno, column) {
        error = "non-existant key word";
    }
};
struct LexerUnrecognizable final : LexerError {
    LexerUnrecognizable(size_t lineno, size_t column) : LexerError(lineno, column) {
        error = "unrecognizable symbols";
    }
};

} // end LexerErrors

namespace ParserErrors {

struct ParserError : Error {
    template<typename... Args>
    ParserError(Args&&... args) : Error(std::forward<Args>(args)...) {}
};

struct Panic final : ParserError {
    Panic(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "parser panic";
    }
};
struct FactorError final : ParserError {
    FactorError(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "cannot correctly evaluate expression";
    }
};
struct ExpectedAssignment final : ParserError {
    ExpectedAssignment(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "expected an assignment operator";
    }
};
struct ExpectedEmpty final : ParserError {
    ExpectedEmpty(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "expected end of line";
    }
};
struct ExpectedThen final : ParserError {
    ExpectedThen(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "expected then";
    }
};
struct ExpectedEndIf final : ParserError {
    ExpectedEndIf(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "expected end if";
    }
};
struct ExpectedIn final : ParserError {
    ExpectedIn(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "expected in";
    }
};
struct ExpectedEndFor final : ParserError {
    ExpectedEndFor(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "expected end for";
    }
};
struct ExpectedEndWhile final : ParserError {
    ExpectedEndWhile(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "expected end while";
    }
};
struct ExpectedFuntionName final : ParserError {
    ExpectedFuntionName(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "expected function name";
    }
};
struct ExpectedLeftBracket final : ParserError {
    ExpectedLeftBracket(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "expected '('";
    }
};
struct ExpectedRightBracket final : ParserError {
    ExpectedRightBracket(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "expected ')'";
    }
};
struct ExpectedLeftSquareBracket final : ParserError {
    ExpectedLeftSquareBracket(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "expected '['";
    }
};
struct ExpectedRightSquareBracket final : ParserError {
    ExpectedRightSquareBracket(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "expected ']'";
    }
};
struct FunctionParamsError final : ParserError {
    FunctionParamsError(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "cannot evaluate given function's parameters";
    }
};
struct ExpectedEndFunc final : ParserError {
    ExpectedEndFunc(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "expected end function";
    }
};
struct ExpectedVarExpr final : ParserError {
    ExpectedVarExpr(size_t lineno, size_t column) : ParserError(lineno, column) {
        error = "expected variable expression";
    }
};

} // end ParserErrors

namespace OperatorErrors {

struct OperatorError : Error {
    template<typename... Args>
    OperatorError(Args&&... args) : Error(std::forward<Args>(args)...) {}
};

struct OperatorUnaryError final : OperatorError {
    OperatorUnaryError(const std::string& op, const std::string& type, size_t lineno, size_t column) : OperatorError(lineno, column) {
        using namespace std::string_literals;
        error = "unknow unary operation '"s + op + "' and "s + type;
    }
};

struct OperatorBinaryError final : OperatorError {
    OperatorBinaryError(const std::string& op, const std::string& left, const std::string& right, size_t lineno = 1, size_t column = 1) : OperatorError(lineno, column) {
        using namespace std::string_literals;
        error = "unknow binary operation '"s + op + "' between "s + left + " and "s + right;
    }
};

} // end OperatorErrors

namespace MemoryErrors {

struct MemoryError : Error {
    template<typename... Args>
    MemoryError(Args&&... args) : Error(std::forward<Args>(args)...) {}
};

struct NotFound final : MemoryError {
    NotFound() : MemoryError("variable is not declared") {}
};

} // end MemoryErrors

namespace TypeErrors {

struct TypeError : Error {
    template<typename... Args>
    TypeError(Args&&... args) : Error(std::forward<Args>(args)...) {}
};

struct TypeErrorNum final : TypeError {
    TypeErrorNum() : TypeError("expression should be a number") {}
};
struct NonPositiveNumber final : TypeError {
    NonPositiveNumber() : TypeError("number should be positive") {}
};
struct IndexNotInteger final : TypeError {
    IndexNotInteger() : TypeError("index must be an integer") {}
};

struct TypeErrorString final : TypeError {
    TypeErrorString() : TypeError("expression should be a string") {}
};

struct TypeErrorList final : TypeError {
    TypeErrorList() : TypeError("expression should be a list") {}
};

struct TypeErrorFunc final : TypeError {
    TypeErrorFunc() : TypeError("expression should be a func") {}
};

struct TypeErrorStringOrList final : TypeError {
    TypeErrorStringOrList() : TypeError("variable must be a string or list") {}
};

} // end TypeErrors

namespace RunTime {

struct RunTimeError : Error {
    template<typename... Args>
    RunTimeError(Args&&... args) : Error(std::forward<Args>(args)...) {}
};

struct OutOfRange final : RunTimeError {
    OutOfRange() : RunTimeError("out of range") {}
};
struct ExpectedZeroArgs final : RunTimeError {
    ExpectedZeroArgs() : RunTimeError("expected no args for call") {}
};
struct ExpectedOneArg final : RunTimeError {
    ExpectedOneArg() : RunTimeError("expected one arg for call") {}
};
struct ExpectedTwoArgs final : RunTimeError {
    ExpectedTwoArgs() : RunTimeError("expected two args for call") {}
};
struct ExpectedThreeArgs final : RunTimeError {
    ExpectedThreeArgs() : RunTimeError("expected three args for call") {}
};
struct ExpectedAtLeastOneArg final : RunTimeError {
    ExpectedAtLeastOneArg() : RunTimeError("expected at lease one arg for call") {}
};
struct ExpectedFromOneOrTwoArgs final : RunTimeError {
    ExpectedFromOneOrTwoArgs() : RunTimeError("expected one or two args for call") {}
};
struct ExpectedFromOneToThreeArgs final : RunTimeError {
    ExpectedFromOneToThreeArgs() : RunTimeError("expected from one to three args for call") {}
};
struct ZeroStep final : RunTimeError {
    ZeroStep() : RunTimeError("cannot have 0 as step") {}
};
struct WrongArgumentCount final : RunTimeError {
    WrongArgumentCount() : RunTimeError("arguments count on calling function does not match") {}
};
struct AssignLiteral final : RunTimeError {
    AssignLiteral() : RunTimeError("cannot assign to literal") {}
};
struct NotEvaluatedSequence final : RunTimeError {
    NotEvaluatedSequence() : RunTimeError("the evaluated expression must be a sequence") {}
};

}

namespace InternalErrors {

struct InternalError : Error {
    template<typename... Args>
    InternalError(Args&&... args) : Error(std::forward<Args>(args)...) {}
};

struct Panic : InternalError {
    Panic(size_t lineno, size_t column)
    : InternalError("internal operation error occured", lineno, column) {}
    Panic() : InternalError(1, 1) {}
};
struct NotImplemented final : InternalError {
    NotImplemented(size_t lineno, size_t column) : InternalError("not implemented yet", lineno, column) {}
    NotImplemented() : NotImplemented(1, 1) {}
};

} // end InternalErrors

void PrintError(std::string header, const Error& error);

void PrintSyntaxError(const Error& error);
void PrintOperatorError(const Error& error);
void PrintTypeError(const Error& error);
void PrintRunTimeError(const Error& error);
void PrintPanic(const Error& error);

void PrintProgramSnippet(std::vector<std::string>&, size_t, size_t);

} // end Errors

namespace Closures {

struct Closure {
    Closure(size_t l, size_t c) : lineno(l), column(c) {}
    const char* what() const { return closure.c_str(); }

    std::string closure = "closure error";
    size_t lineno = 1;
    size_t column = 1;
};

struct UncaughtClosure final : Closure {
    UncaughtClosure(const std::string& str, size_t lineno, size_t column)
    : Closure(lineno, column) { closure = "uncaughted " + str + " closure"; }
};

struct NonExistantClosure final : Closure {
    NonExistantClosure(size_t lineno, size_t column) : Closure(lineno, column) {
        closure = "close non-existant closure";
    }
};

struct Return final : Closure {
    Return(std::any hp, size_t lineno, size_t column)
    : Closure(lineno, column), holder_pack(std::move(hp)) {
        closure = "uncaught return closure";
    }
    std::any holder_pack;
};

struct Break final : Closure {
    Break(size_t lineno, size_t column) : Closure(lineno, column) {
        closure = "uncaught break closure";
    }
};

struct Continue final : Closure {
    Continue(size_t lineno, size_t column) : Closure(lineno, column) {
        closure = "uncaught continue closure";
    }
};

void PrintClosureError(const Closure& c);

} // end Closures

using Error = Errors::Error;
using ParserError = Errors::ParserErrors::Panic;
using InternalError = Errors::InternalErrors::Panic;
using OutOfRange = Errors::RunTime::OutOfRange;

using Closure = Closures::Closure;

namespace Utils {

template<typename Comp>
std::vector<std::string> Split(const std::string& string, const Comp& comp);

} // end Utils
