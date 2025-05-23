#include "interpreter.h"

bool interpret(std::istream& input, std::ostream& output) {
    std::string line;
    do {
        std::cout << Patterns::CMD;
        std::getline(input, line);
    } while (line.size() == 0);

    Lexer::Tokenizer tokenizer(line);

    Lexer::Token token;
    while (token.token != Lexer::Tokens::T_EOF) {
        tokenizer >> token;
        std::cout << token.token << " ; ";
        if (token.token == Lexer::Tokens::T_BAD) {
            Errors::SyntaxError();
            return false;
        }
    }
    std::cout << '\n';

    return true;
}