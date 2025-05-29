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

    struct Error {
        virtual std::string what() { return "error"; }
        virtual ~Error() = default;
    };

    namespace LexerErrors {
        struct LexerNumberError : Error {
            std::string what() override { return "wrong number literal"; }
        };
        struct LexerStringError : Error {
            std::string what() override { return "unclosed string literal"; }
        };
        struct LexerKeyWordError : Error {
            std::string what() override { return "non-existant key word"; }
        };
        struct LexerUnrecognizable : Error {
            std::string what() override { return "unrecognizable symbols"; }
        };
    }

    namespace ParserErrors {
        struct Panic : Error {
            std::string what() override { return "parser panic"; }
        };
        struct FactorError : Error {
            std::string what() override { return "cannot correctly evaluate expression"; }
        };
        struct ExpectedAssignment : Error {
            std::string what() override { return "expected an assignment operator"; }
        };
        struct ExpectedEmpty : Error {
            std::string what() override { return "expected end of line"; }
        };
        struct ExpectedThen : Error {
            std::string what() override { return "expected then"; }
        };
        struct ExpectedEndIf : Error {
            std::string what() override { return "expected end if"; }
        };
        struct ExpectedIn : Error {
            std::string what() override { return "expected in"; }
        };
        struct ExpectedEndFor : Error {
            std::string what() override { return "expected end for"; }
        };
        struct ExpectedEndWhile : Error {
            std::string what() override { return "expected end while"; }
        };
        struct ExpectedFuntionName : Error {
            std::string what() override { return "expected function name"; }
        };
        struct ExpectedLeftBracket : Error {
            std::string what() override { return "expected '('"; }
        };
        struct FunctionParamsError : Error {
            std::string what() override { return "cannot evaluate given function's parameters"; }
        };
        struct ExpectedEndFunc : Error {
            std::string what() override { return "expected end function"; }
        };
    }

    namespace OperatorErrors {
        struct OperatorUnaryError : Error {
            OperatorUnaryError(const std::string& o, const std::string& t) : op(o), type(t) {}
            std::string what() override {
                using namespace std::string_literals;
                return "unknow unary operation '"s + op + "' and "s + type; 
            }
            std::string op;
            std::string type;
        };

        struct OperatorBinaryError : Error {
            OperatorBinaryError(const std::string& o, const std::string& l, const std::string& r)
            : op(o), left(l), right(r) {}
            std::string what() override {
                using namespace std::string_literals;
                return "unknow unary operation '"s + op + "' between "s + left + " and "s + right; 
            }
            std::string op;
            std::string left;
            std::string right;
        };
    }

    namespace InternalErrors {
        struct InternalOperationError : Error {
            std::string what() override { return "internal operation error occured: wrong argument (or arg / return types) for operator"; }
        };
        struct NotImplemented : Error {
            std::string what() override { return "not implemented yet"; }
        };
    }

    inline void PrintError(std::string header, Errors::Error* error, size_t pos, size_t line) {
        std::cout << '\n' << Patterns::RED << header << Patterns::WHITE << ": "
                  << error->what() << " at " << Patterns::WHITEBOLD 
                  << "line " << line << ", col " << pos << Patterns::WHITE << std::endl;
    }
}

using Error = Errors::Error;
