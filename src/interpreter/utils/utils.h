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

struct LexerNumberError final : Error {
    LexerNumberError(size_t lineno, size_t column) : Error(lineno, column) {
        error = "wrong number literal";
    }
};
struct LexerStringError final : Error {
    LexerStringError(size_t lineno, size_t column) : Error(lineno, column) {
        error = "unclosed string literal";
    }
};
struct LexerKeyWordError final : Error {
    LexerKeyWordError(size_t lineno, size_t column) : Error(lineno, column) {
        error = "non-existant key word";
    }
};
struct LexerUnrecognizable final : Error {
    LexerUnrecognizable(size_t lineno, size_t column) : Error(lineno, column) {
        error = "unrecognizable symbols";
    }
};

} // end LexerErrors

namespace ParserErrors {


struct Panic final : Error {
    Panic(size_t lineno, size_t column) : Error(lineno, column) {
        error = "parser panic";
    }
};
struct FactorError final : Error {
    FactorError(size_t lineno, size_t column) : Error(lineno, column) {
        error = "cannot correctly evaluate expression";
    }
};
struct ExpectedAssignment final : Error {
    ExpectedAssignment(size_t lineno, size_t column) : Error(lineno, column) {
        error = "expected an assignment operator";
    }
};
struct ExpectedEmpty final : Error {
    ExpectedEmpty(size_t lineno, size_t column) : Error(lineno, column) {
        error = "expected end of line";
    }
};
struct ExpectedThen final : Error {
    ExpectedThen(size_t lineno, size_t column) : Error(lineno, column) {
        error = "expected then";
    }
};
struct ExpectedEndIf final : Error {
    ExpectedEndIf(size_t lineno, size_t column) : Error(lineno, column) {
        error = "expected end if";
    }
};
struct ExpectedIn final : Error {
    ExpectedIn(size_t lineno, size_t column) : Error(lineno, column) {
        error = "expected in";
    }
};
struct ExpectedEndFor final : Error {
    ExpectedEndFor(size_t lineno, size_t column) : Error(lineno, column) {
        error = "expected end for";
    }
};
struct ExpectedEndWhile final : Error {
    ExpectedEndWhile(size_t lineno, size_t column) : Error(lineno, column) {
        error = "expected end while";
    }
};
struct ExpectedFuntionName final : Error {
    ExpectedFuntionName(size_t lineno, size_t column) : Error(lineno, column) {
        error = "expected function name";
    }
};
struct ExpectedLeftBracket final : Error {
    ExpectedLeftBracket(size_t lineno, size_t column) : Error(lineno, column) {
        error = "expected '('";
    }
};
struct ExpectedRightBracket final : Error {
    ExpectedRightBracket(size_t lineno, size_t column) : Error(lineno, column) {
        error = "expected ')'";
    }
};
struct ExpectedLeftSquareBracket final : Error {
    ExpectedLeftSquareBracket(size_t lineno, size_t column) : Error(lineno, column) {
        error = "expected '['";
    }
};
struct ExpectedRightSquareBracket final : Error {
    ExpectedRightSquareBracket(size_t lineno, size_t column) : Error(lineno, column) {
        error = "expected ']'";
    }
};
struct FunctionParamsError final : Error {
    FunctionParamsError(size_t lineno, size_t column) : Error(lineno, column) {
        error = "cannot evaluate given function's parameters";
    }
};
struct ExpectedEndFunc final : Error {
    ExpectedEndFunc(size_t lineno, size_t column) : Error(lineno, column) {
        error = "expected end function";
    }
};
struct ExpectedVarExpr final : Error {
    ExpectedVarExpr(size_t lineno, size_t column) : Error(lineno, column) {
        error = "expected variable expression";
    }
};

} // end ParserErrors

namespace OperatorErrors {

struct OperatorUnaryError final : Error {
    OperatorUnaryError(const std::string& op, const std::string& type, size_t lineno, size_t column) : Error(lineno, column) {
        using namespace std::string_literals;
        error = "unknow unary operation '"s + op + "' and "s + type;
    }
};

struct OperatorBinaryError final : Error {
    OperatorBinaryError(const std::string& op, const std::string& left, const std::string& right, size_t lineno = 1, size_t column = 1) : Error(lineno, column) {
        using namespace std::string_literals;
        error = "unknow binary operation '"s + op + "' between "s + left + " and "s + right; 
    }
};

} // end OperatorErrors

namespace MemoryErrors {

struct NotFound final : Error {
    NotFound() : Error("variable is not declared") {}
};

} // end MemoryErrors

namespace TypeErrors {

struct TypeErrorNum final : Error {
    TypeErrorNum() : Error("expression should be a number") {}
};
struct NonPositiveNumber final : Error {
    NonPositiveNumber() : Error("number should be positive") {}
};
struct IndexNotInteger final : Error {
    IndexNotInteger() : Error("index must be an integer") {}
};

struct TypeErrorString final : Error {
    TypeErrorString() : Error("expression should be a string") {}
};

struct TypeErrorList final : Error {
    TypeErrorList() : Error("expression should be a list") {}
};

struct TypeErrorFunc final : Error {
    TypeErrorFunc() : Error("expression should be a func") {}
};

struct TypeErrorStringOrList final : Error {
    TypeErrorStringOrList() : Error("variable must be a string or list") {}
};

} // end TypeErrors

namespace RunTime {

struct OutOfRange final : Error {
    OutOfRange() : Error("out of range") {}
};
struct ExpectedZeroArgs final : Error {
    ExpectedZeroArgs() : Error("expected no args for call") {}
};
struct ExpectedOneArg final : Error {
    ExpectedOneArg() : Error("expected one arg for call") {}
};
struct ExpectedTwoArgs final : Error {
    ExpectedTwoArgs() : Error("expected two args for call") {}
};
struct ExpectedThreeArgs final : Error {
    ExpectedThreeArgs() : Error("expected three args for call") {}
};
struct ExpectedAtLeastOneArg final : Error {
    ExpectedAtLeastOneArg() : Error("expected at lease one arg for call") {}
};
struct ExpectedFromOneOrTwoArgs final : Error {
    ExpectedFromOneOrTwoArgs() : Error("expected one or two args for call") {}
};
struct ExpectedFromOneToThreeArgs final : Error {
    ExpectedFromOneToThreeArgs() : Error("expected from one to three args for call") {}
};
struct ZeroStep final : Error {
    ZeroStep() : Error("cannot have 0 as step") {}
};
struct WrongArgumentCount final : Error {
    WrongArgumentCount() : Error("arguments count on calling function does not match") {}
};
struct AssignLiteral final : Error {
    AssignLiteral() : Error("cannot assign to literal") {}
};
struct NotEvaluatedSequence final : Error {
    NotEvaluatedSequence() : Error("the evaluated expression must be a sequence") {}
};

}

namespace InternalErrors {

struct InternalError final : Error {
    InternalError(size_t lineno, size_t column) : Error("internal operation error occured", lineno, column) {}
    InternalError() : InternalError(1, 1) {}
};
struct NotImplemented final : Error {
    NotImplemented(size_t lineno, size_t column) : Error("not implemented yet", lineno, column) {}
    NotImplemented() : NotImplemented(1, 1) {}
};

} // end InternalErrors

void PrintError(std::string header, const Error& error);

void PrintSyntaxError(const Error& error);
void PrintRunTimeError(const Error& error);
void PrintPanic(const Error& error = InternalError());

} // end Errors

namespace Closures {

struct Closure {
    Closure(std::any t) : token(std::move(t)) {}
    const char* what() const { return closure.c_str(); }

    std::string closure = "closure error";
    size_t lineno = 1;
    size_t column = 1;
    std::any token; // TODO delete token depending
};

struct UncaughtClosure final : Closure {
    UncaughtClosure(std::any t, const std::string& str)
    : Closure(std::move(t)) { closure = "uncaughted " + str + " closure"; }
};

struct NonExistantClosure final : Closure {
    NonExistantClosure(std::any t) : Closure(std::move(t)) { closure = "close non-existant closure"; }
};

struct Return final : Closure {
    Return(std::any hp, std::any t) : Closure(std::move(t)), holder_pack(std::move(hp)) { closure = "uncaught return closure"; }
    std::any holder_pack;
};

struct Break final : Closure {
    Break(std::any t) : Closure(std::move(t)) { closure = "uncaught break closure"; }
};

struct Continue final : Closure {
    Continue(std::any t) : Closure(std::move(t)) { closure = "uncaught continue closure"; }
};

void PrintClosureError(const Closure& c);

} // end Closures

using Error = Errors::Error;
using ParserError = Errors::ParserErrors::Panic;
using InternalError = Errors::InternalErrors::InternalError;
using OutOfRange = Errors::RunTime::OutOfRange;

using Closure = Closures::Closure;

namespace Utils {

template<typename Comp>
std::vector<std::string> Split(const std::string& string, const Comp& comp);

} // end Utils
