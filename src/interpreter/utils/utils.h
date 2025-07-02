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

} // end Patterns

namespace Errors {

struct Error {
    Error(size_t l, size_t c) : lineno(l), column(c) {}
    virtual const char* what() const { return error.c_str(); }
    virtual ~Error() = 0;

    std::string error;
    size_t lineno;
    size_t column;
};

namespace CommonErrors {

inline void ErrorOpenFile() {
    std::cerr << Patterns::RED << "Error" << Patterns::WHITE << ": could not read the file." << std::endl;
}

} // end CommonErrors

namespace LexerErrors {

struct LexerNumberError final : Error {
    const char* what() const override { return "wrong number literal"; }
};
struct LexerStringError final : Error {
    const char* what() const override { return "unclosed string literal"; }
};
struct LexerKeyWordError final : Error {
    const char* what() const override { return "non-existant key word"; }
};
struct LexerUnrecognizable final : Error {
    const char* what() const override { return "unrecognizable symbols"; }
};

} // end LexerErrors

namespace ParserErrors {

struct Panic final : Error {
    const char* what() const override { return "parser panic"; }
};
struct FactorError final : Error {
    const char* what() const override { return "cannot correctly evaluate expression"; }
};
struct ExpectedAssignment final : Error {
    const char* what() const override { return "expected an assignment operator"; }
};
struct ExpectedEmpty final : Error {
    const char* what() const override { return "expected end of line"; }
};
struct ExpectedThen final : Error {
    const char* what() const override { return "expected then"; }
};
struct ExpectedEndIf final : Error {
    const char* what() const override { return "expected end if"; }
};
struct ExpectedIn final : Error {
    const char* what() const override { return "expected in"; }
};
struct ExpectedEndFor final : Error {
    const char* what() const override { return "expected end for"; }
};
struct ExpectedEndWhile final : Error {
    const char* what() const override { return "expected end while"; }
};
struct ExpectedFuntionName final : Error {
    const char* what() const override { return "expected function name"; }
};
struct ExpectedLeftBracket final : Error {
    const char* what() const override { return "expected '('"; }
};
struct ExpectedRightBracket final : Error {
    const char* what() const override { return "expected ')'"; }
};
struct ExpectedLeftSquareBracket final : Error {
    const char* what() const override { return "expected '['"; }
};
struct ExpectedRightSquareBracket final : Error {
    const char* what() const override { return "expected ']'"; }
};
struct FunctionParamsError final : Error {
    const char* what() const override { return "cannot evaluate given function's parameters"; }
};
struct ExpectedEndFunc final : Error {
    const char* what() const override { return "expected end function"; }
};
struct ExpectedVarExpr final : Error {
    const char* what() const override { return "expected variable expression"; }
};

} // end ParserErrors

namespace OperatorErrors {

struct OperatorUnaryError final : Error {
    OperatorUnaryError(const std::string& op, const std::string& type, size_t lineno, size_t column) : Error(lineno, column) {
        using namespace std::string_literals;
        error = "unknow unary operation '"s + op + "' and "s + type;
    }
    const char* what() const override {
        return error.c_str();
    }
};

struct OperatorBinaryError final : Error {
    OperatorBinaryError(const std::string& op, const std::string& left, const std::string& right, size_t lineno, size_t column) : Error(lineno, column) {
        using namespace std::string_literals;
        error = "unknow binary operation '"s + op + "' between "s + left + " and "s + right; 
    }
    const char* what() const override { return error.c_str(); }
};

} // end OperatorErrors

namespace MemoryErrors {

struct NotFound final : Error {
    const char* what() const override { return "variable is not declared"; }
};

} // end MemoryErrors

namespace TypeErrors {

struct TypeErrorNum final : Error {
    const char* what() const override { return "expression should be a number"; }
};
struct NonPositiveNumber final : Error {
    const char* what() const override { return "number should be positive"; }
};
struct IndexNotInteger final : Error {
    const char* what() const override { return "index must be an integer"; }
};

struct TypeErrorString final : Error {
    const char* what() const override { return "expression should be a string"; }
};

struct TypeErrorList final : Error {
    const char* what() const override { return "expression should be a list"; }
};

struct TypeErrorFunc final : Error {
    const char* what() const override { return "expression should be a func"; }
};

struct TypeErrorStringOrList final : Error {
    const char* what() const override { return "variable must be a string or list"; }
};

} // end TypeErrors

namespace RunTime {

struct OutOfRange final : Error {
    const char* what() const override { return "out of range"; }
};
struct ExpectedZeroArgs final : Error {
    const char* what() const override { return "expected no args for call"; }
};
struct ExpectedOneArg final : Error {
    const char* what() const override { return "expected one arg for call"; }
};
struct ExpectedTwoArgs final : Error {
    const char* what() const override { return "expected two args for call"; }
};
struct ExpectedThreeArgs final : Error {
    const char* what() const override { return "expected three args for call"; }
};
struct ExpectedAtLeastOneArg final : Error {
    const char* what() const override { return "expected at lease one arg for call"; }
};
struct ExpectedFromOneOrTwoArgs final : Error {
    const char* what() const override { return "expected one or two args for call"; }
};
struct ExpectedFromOneToThreeArgs final : Error {
    const char* what() const override { return "expected from one to three args for call"; }
};
struct ZeroStep final : Error {
    const char* what() const override { return "cannot have 0 as step"; }
};
struct WrongArgumentCount final : Error {
    const char* what() const override { return "arguments count on calling function does not match"; }
};
struct AssignLiteral final : Error {
    const char* what() const override { return "cannot assign to literal"; }
};
struct NotEvaluatedSequence final : Error {
    const char* what() const override { return "the evaluated expression must be a sequence"; }
};

}

namespace InternalErrors {

struct InternalError final : Error {
    const char* what() const override { return "internal operation error occured"; }
};
struct NotImplemented final : Error {
    const char* what() const override { return "not implemented yet"; }
};

} // end InternalErrors

inline void PrintError(std::string header, Errors::Error* error, size_t pos, size_t line) {
    std::cerr << Patterns::RED << header << Patterns::WHITE << ": "
              << error->what() << " at " << Patterns::WHITEBOLD
              << "line " << line << ", col " << pos << Patterns::WHITE << std::endl;
}

} // end Errors

namespace Closures {

struct Closure {
    Closure(std::any t) : token(std::move(t)) {}
    virtual const char* what() const { return closure.c_str(); }
    virtual ~Closure() = 0;

    std::string closure = "uncaughted ? closure";
    std::any token;
};

struct Return final : Closure {
    Return(std::any hp, std::any t) : Closure(std::move(t)), holder_pack(std::move(hp)) {}
    const char* what() const override { return "uncaught return closure"; }
    std::any holder_pack;
};

struct Break final : Closure {
    Break(std::any t) : Closure(std::move(t)) {}
    const char* what() const override { return "uncaught break closure"; }
};

struct Continue final : Closure {
    Continue(std::any t) : Closure(std::move(t)) {}
    const char* what() const override { return "uncaught continue closure"; }
};

} // end Closures

using ParserError = Errors::ParserErrors::Panic;
using InternalError = Errors::InternalErrors::InternalError;
using OutOfRange = Errors::RunTime::OutOfRange;

using Closure = Closures::Closure;

namespace Utils {

template<typename Comp>
std::vector<std::string> Split(const std::string& string, const Comp& comp);

} // end Utils
