#pragma once
#include <iostream>
#include <string>
#include <string_view>

namespace Patterns {
    static std::string RED = "\e[1;31m";
    static std::string WHITE = "\e[0m";
    static std::string BLUE = "\e[1;34m";

    static std::string WHITEBOLD = "\e[1;37m";

    static std::string CMD = "\e[3;34mitmo> \e[0m";

    inline void Welcome() {
        std::cout << "Welcome to " + BLUE + "ITMO SCRIPT" + WHITE + "!" << std::endl;
    }
}

namespace Errors {
    inline void ErrorOpenFile() {
        std::cout << Patterns::RED << "Error" << Patterns::WHITE << ": could not read the file." << std::endl;
    }

// переделать на строковые литералы const char*
    struct Error {
        Error(const std::string& e = "error") : error(e) {}
        virtual const char* what() const { return error.c_str(); }
        virtual ~Error() = default;

        std::string error;
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
                error = "unknow unary operation '"s + op + "' between "s + left + " and "s + right; 
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
        struct TypeErrorInt : Error {
            const char* what() const override { return "expression must to be a number"; }
        };
        struct IndexNotInteger : Error {
            const char* what() const override { return "index must be an integer"; }
        };
        struct TypeErrorStringOrList : Error {
            const char* what() const override { return "variable must be a string or list"; }
        };
    }

    namespace RunTime {
        struct OutOfRange : Error {
            const char* what() const override { return "out of range"; }
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
    std::cout << Patterns::RED << header << Patterns::WHITE << ": "
                << error->what() << " at " << Patterns::WHITEBOLD 
                << "line " << line << ", col " << pos << Patterns::WHITE << std::endl;
}

} // end Errors

using Error = Errors::Error;
using ParserError = Errors::ParserErrors::Panic;
using InternalError = Errors::InternalErrors::InternalError;
using OutOfRange = Errors::RunTime::OutOfRange;
