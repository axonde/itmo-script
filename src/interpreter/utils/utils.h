#pragma once
#include <any>
#include <iostream>
#include <string>
#include <string_view>
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
    inline void ErrorOpenFile() {
        std::cerr << Patterns::RED << "Error" << Patterns::WHITE << ": could not read the file." << std::endl;
    }

    struct Error {
        Error() = default;
        Error(const std::string& e) : error(e) {}
        virtual const char* what() const { return error.c_str(); }
        virtual ~Error() = default;

        std::string error = "error";
    };

    namespace LexerErrors {
        struct LexerNumberError : Error {
            const char* what() const override { return "wrong number literal"; }
        };
        struct LexerStringError : Error {
            const char* what() const override { return "unclosed string literal"; }
        };
        struct LexerKeyWordError : Error {
            const char* what() const override { return "non-existant key word"; }
        };
        struct LexerUnrecognizable : Error {
            const char* what() const override { return "unrecognizable symbols"; }
        };
    }

    namespace ParserErrors {
        struct Panic : Error {
            const char* what() const override { return "parser panic"; }
        };
        struct FactorError : Error {
            const char* what() const override { return "cannot correctly evaluate expression"; }
        };
        struct ExpectedAssignment : Error {
            const char* what() const override { return "expected an assignment operator"; }
        };
        struct ExpectedEmpty : Error {
            const char* what() const override { return "expected end of line"; }
        };
        struct ExpectedThen : Error {
            const char* what() const override { return "expected then"; }
        };
        struct ExpectedEndIf : Error {
            const char* what() const override { return "expected end if"; }
        };
        struct ExpectedIn : Error {
            const char* what() const override { return "expected in"; }
        };
        struct ExpectedEndFor : Error {
            const char* what() const override { return "expected end for"; }
        };
        struct ExpectedEndWhile : Error {
            const char* what() const override { return "expected end while"; }
        };
        struct ExpectedFuntionName : Error {
            const char* what() const override { return "expected function name"; }
        };
        struct ExpectedLeftBracket : Error {
            const char* what() const override { return "expected '('"; }
        };
        struct ExpectedRightBracket : Error {
            const char* what() const override { return "expected ')'"; }
        };
        struct ExpectedLeftSquareBracket : Error {
            const char* what() const override { return "expected '['"; }
        };
        struct ExpectedRightSquareBracket : Error {
            const char* what() const override { return "expected ']'"; }
        };
        struct FunctionParamsError : Error {
            const char* what() const override { return "cannot evaluate given function's parameters"; }
        };
        struct ExpectedEndFunc : Error {
            const char* what() const override { return "expected end function"; }
        };
        struct ExpectedVarExpr : Error {
            const char* what() const override { return "expected variable expression"; }
        };
    }

    namespace OperatorErrors {
        struct OperatorUnaryError : Error {
            OperatorUnaryError(const std::string& op, const std::string& type) {
                using namespace std::string_literals;
                error = "unknow unary operation '"s + op + "' and "s + type;
            }
            const char* what() const override {
                return error.c_str();
            }
            std::string error;
        };

        struct OperatorBinaryError : Error {
            OperatorBinaryError(const std::string& op, const std::string& left, const std::string& right) {
                using namespace std::string_literals;
                error = "unknow binary operation '"s + op + "' between "s + left + " and "s + right; 
            }
            const char* what() const override { return error.c_str(); }
            std::string error;
        };
    }

    namespace MemoryErrors {
        struct NotFound : Error {
            const char* what() const override { return "variable is not declared"; }
        };
    }

    namespace TypeErrors {
        struct TypeErrorNum : Error {
            const char* what() const override { return "expression should be a number"; }
        };
        struct NonPositiveNumber : Error {
            const char* what() const override { return "number should be positive"; }
        };
        struct IndexNotInteger : Error {
            const char* what() const override { return "index must be an integer"; }
        };

        struct TypeErrorString : Error {
            const char* what() const override { return "expression should be a string"; }
        };

        struct TypeErrorList : Error {
            const char* what() const override { return "expression should be a list"; }
        };

        struct TypeErrorFunc : Error {
            const char* what() const override { return "expression should be a func"; }
        };

        struct TypeErrorStringOrList : Error {
            const char* what() const override { return "variable must be a string or list"; }
        };
    }

    namespace RunTime {
        struct OutOfRange : Error {
            const char* what() const override { return "out of range"; }
        };
        struct ExpectedZeroArgs : Error {
            const char* what() const override { return "expected no args for call"; }
        };
        struct ExpectedOneArg : Error {
            const char* what() const override { return "expected one arg for call"; }
        };
        struct ExpectedTwoArgs : Error {
            const char* what() const override { return "expected two args for call"; }
        };
        struct ExpectedThreeArgs : Error {
            const char* what() const override { return "expected three args for call"; }
        };
        struct ExpectedAtLeastOneArg : Error {
            const char* what() const override { return "expected at lease one arg for call"; }
        };
        struct ExpectedFromOneOrTwoArgs : Error {
            const char* what() const override { return "expected one or two args for call"; }
        };
        struct ExpectedFromOneToThreeArgs : Error {
            const char* what() const override { return "expected from one to three args for call"; }
        };
        struct ZeroStep : Error {
            const char* what() const override { return "cannot have 0 as step"; }
        };
        struct WrongArgumentCount : Error {
            const char* what() const override { return "arguments count on calling function does not match"; }
        };
        struct AssignLiteral : Error {
            const char* what() const override { return "cannot assign to literal"; }
        };
        struct NotEvaluatedSequence : Error {
            const char* what() const override { return "the evaluated expression must be a sequence"; }
        };
    }

    namespace InternalErrors {
        struct InternalError : Error {
            const char* what() const override { return "internal operation error occured"; }
        };
        struct NotImplemented : Error {
            const char* what() const override { return "not implemented yet"; }
        };
    }

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
    virtual ~Closure() = default;

    std::string closure = "uncaughted ? closure";
    std::any token;
};

struct Return : Closure {
    Return(std::any hp, std::any t) : Closure(std::move(t)), holder_pack(std::move(hp)) {}
    const char* what() const override { return "uncaught return closure"; }
    std::any holder_pack;
};

struct Break : Closure {
    Break(std::any t) : Closure(std::move(t)) {}
    const char* what() const override { return "uncaught break closure"; }
};

struct Continue : Closure {
    Continue(std::any t) : Closure(std::move(t)) {}
    const char* what() const override { return "uncaught continue closure"; }
};

} // end Closures

using Error = Errors::Error;
using ParserError = Errors::ParserErrors::Panic;
using InternalError = Errors::InternalErrors::InternalError;
using OutOfRange = Errors::RunTime::OutOfRange;

using Closure = Closures::Closure;

namespace Utils {

template<typename Comp>
std::vector<std::string> Split(const std::string& string, const Comp& comp) {
    std::vector<std::string> splitted_string;
    int l = 0;
    for (int r = 0; r != string.size() + 1; r++) {
        if (r == string.size() || comp(string[r])) {
            splitted_string.push_back(string.substr(l, r - l));
            l = r + 1;
        }
    }
    return splitted_string;
}

}  // end Utils