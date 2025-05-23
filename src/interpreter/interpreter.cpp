#include "interpreter.h"

bool Interpreter::Interpret(std::istream& input, std::ostream& output) {
    std::string line;
    do {
        std::cout << Patterns::CMD;
        std::getline(input, line);
    } while (line.size() == 0);

    Lexer::Tokenizer tokenizer(line);

    Lexer::Token token;
    tokenizer >> token;
    while (token.token != Lexer::Tokens::T_EOF) {
        std::cout << token.token << " ; ";
        if (token.token == Lexer::Tokens::T_BAD) {
            SyntaxError(token);
            return false;
        }
        tokenizer >> token;
    }
    std::cout << '\n';

    return true;
}
