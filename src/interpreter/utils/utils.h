#pragma once
#include <iostream>
#include <string>

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
