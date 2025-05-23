#pragma once
#include <iostream>
#include <string>

#include "lexer/lexer.cpp"

namespace Interpreter {
    bool Interpret(std::istream& input, std::ostream& output);


    // ERRORS
    inline void SyntaxError(const Lexer::Token& token) {
        using ErrorType = std::shared_ptr<Errors::Error>;
        Errors::PrintError("Syntax error", std::get<ErrorType>(token.value).get(), token.pos);
    }
}