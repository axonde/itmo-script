#pragma once
#include <iostream>
#include <string>

#include "lexer.h"
#include "parser.h"

namespace Interpreter {
    bool Interpret(std::istream&, std::ostream&);
    bool Interpret(std::string&, std::ostream&);

    // ERRORS
    inline void SyntaxError(const Lexer::Token& token) {
        using ErrorType = std::shared_ptr<Errors::Error>;
        Errors::PrintError("Syntax error", std::get<ErrorType>(token.value).get(), token.pos, token.line);
    }
}