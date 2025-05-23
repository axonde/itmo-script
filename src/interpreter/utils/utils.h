#pragma once
#include <iostream>
#include <string>

namespace Patterns {
    static std::string RED = "\e[1;31m";
    static std::string WHITE = "\e[0m";

    static std::string WHITEBOLD = "\e[1;37m";

    static std::string CMD = "\e[3;34mitmo> \e[0m";
}

namespace Errors {
    struct Error {
        virtual const char* what() { return "error"; }
        virtual ~Error() = default;
    };

    namespace LexerErrors {
        struct LexerNumberError : Error {
            const char* what() override { return "wrong number literal"; }
        };
        struct LexerStringError : Error {
            const char* what() override { return "unclosed string literal"; }
        };
        struct LexerKeyWordError : Error {
            const char* what() override { return "non-existant key word"; }
        };
        struct LexerUnrecognizable : Error {
            const char* what() override { return "unrecognizable symbols"; }
        };
    }

    inline void PrintError(std::string header, Errors::Error* error, size_t pos) {
        std::cout << '\n'<< Patterns::RED << header << Patterns::WHITE << ": "
                  << error->what() << " at " << Patterns::WHITEBOLD << pos << Patterns::WHITE << std::endl;
    }
}
