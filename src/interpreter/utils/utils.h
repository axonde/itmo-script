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

extern std::ostream* err;  // forward declaration for Errors out

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

namespace GeneralErrors {

void FailedOpenFile(std::string&& path);
void WrongLaunch();

} // end GeneralErrors

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
    NotFound(size_t lineno = 1, size_t column = 1) : MemoryError("variable is not declared", lineno, column) {}
};

} // end MemoryErrors

namespace TypeErrors {

struct TypeError : Error {
    template<typename... Args>
    TypeError(Args&&... args) : Error(std::forward<Args>(args)...) {}
};

struct TypeErrorNum final : TypeError {
    TypeErrorNum(size_t lineno = 1, size_t column = 1) : TypeError("expression should be a number", lineno, column) {}
};
struct NonPositiveNumber final : TypeError {
    NonPositiveNumber(size_t lineno = 1, size_t column = 1) : TypeError("number should be positive", lineno, column) {}
};
struct IndexNotInteger final : TypeError {
    IndexNotInteger(size_t lineno = 1, size_t column = 1) : TypeError("index must be an integer", lineno, column) {}
};

struct TypeErrorString final : TypeError {
    TypeErrorString(size_t lineno = 1, size_t column = 1) : TypeError("expression should be a string", lineno, column) {}
};

struct TypeErrorList final : TypeError {
    TypeErrorList(size_t lineno = 1, size_t column = 1) : TypeError("expression should be a list", lineno, column) {}
};

struct TypeErrorFunc final : TypeError {
    TypeErrorFunc(size_t lineno = 1, size_t column = 1) : TypeError("expression should be a func", lineno, column) {}
};

struct TypeErrorStringOrList final : TypeError {
    TypeErrorStringOrList(size_t lineno = 1, size_t column = 1) : TypeError("variable must be a string or list", lineno, column) {}
};

} // end TypeErrors

namespace RunTime {

struct RunTimeError : Error {
    template<typename... Args>
    RunTimeError(Args&&... args) : Error(std::forward<Args>(args)...) {}
};

struct OutOfRange final : RunTimeError {
    OutOfRange(size_t lineno = 1, size_t column = 1) : RunTimeError("out of range", lineno, column) {}
};
struct ExpectedZeroArgs final : RunTimeError {
    ExpectedZeroArgs(size_t lineno = 1, size_t column = 1) : RunTimeError("expected no args for call", lineno, column) {}
};
struct ExpectedOneArg final : RunTimeError {
    ExpectedOneArg() : RunTimeError("expected one arg for call") {}
};
struct ExpectedTwoArgs final : RunTimeError {
    ExpectedTwoArgs(size_t lineno = 1, size_t column = 1) : RunTimeError("expected two args for call", lineno, column) {}
};
struct ExpectedThreeArgs final : RunTimeError {
    ExpectedThreeArgs(size_t lineno = 1, size_t column = 1) : RunTimeError("expected three args for call", lineno, column) {}
};
struct ExpectedAtLeastOneArg final : RunTimeError {
    ExpectedAtLeastOneArg(size_t lineno = 1, size_t column = 1) : RunTimeError("expected at lease one arg for call", lineno, column) {}
};
struct ExpectedFromOneOrTwoArgs final : RunTimeError {
    ExpectedFromOneOrTwoArgs(size_t lineno = 1, size_t column = 1) : RunTimeError("expected one or two args for call", lineno, column) {}
};
struct ExpectedFromOneToThreeArgs final : RunTimeError {
    ExpectedFromOneToThreeArgs(size_t lineno = 1, size_t column = 1) : RunTimeError("expected from one to three args for call", lineno, column) {}
};
struct ZeroStep final : RunTimeError {
    ZeroStep(size_t lineno = 1, size_t column = 1) : RunTimeError("cannot have 0 as step", lineno, column) {}
};
struct WrongArgumentCount final : RunTimeError {
    WrongArgumentCount(size_t lineno = 1, size_t column = 1) : RunTimeError("arguments count on calling function does not match", lineno, column) {}
};
struct AssignLiteral final : RunTimeError {
    AssignLiteral(size_t lineno = 1, size_t column = 1) : RunTimeError("cannot assign to literal", lineno, column) {}
};
struct NotEvaluatedSequence final : RunTimeError {
    NotEvaluatedSequence(size_t lineno = 1, size_t column = 1) : RunTimeError("the evaluated expression must be a sequence", lineno, column) {}
};

}

namespace InternalErrors {

struct InternalError : Error {
    template<typename... Args>
    InternalError(Args&&... args) : Error(std::forward<Args>(args)...) {}
};

struct Panic : InternalError {
    Panic(size_t lineno = 1, size_t column = 1)
    : InternalError("internal operation error occured", lineno, column) {}
};
struct NotImplemented final : InternalError {
    NotImplemented(size_t lineno = 1, size_t column = 1) : InternalError("not implemented yet", lineno, column) {}
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
using RunTimeError = Errors::RunTime::RunTimeError;
using OutOfRange = Errors::RunTime::OutOfRange;

using Closure = Closures::Closure;

namespace Utils {

template<typename Comp>
std::vector<std::string> Split(const std::string& string, const Comp& comp) {
    std::vector<std::string> splitted_string;
    size_t l = 0;
    for (size_t r = 0; r != string.size() + 1; r++) {
        if (r == string.size() || comp(string[r])) {
            splitted_string.push_back(string.substr(l, r - l));
            l = r + 1;
        }
    }
    return splitted_string;
}

} // end Utils
