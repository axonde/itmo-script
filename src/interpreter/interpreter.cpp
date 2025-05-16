#include "interpreter.h"

bool interpret(std::istream& input, std::ostream& output) {
    std::string line;
    do {
        std::cout << Utils::Patterns::CMD;
        std::getline(input, line);
    } while (line.size() == 0);

    std::cout << line << '\n' << line.size() << '\n';

    Lexer::Tokenizer tokenizer(line);

    Lexer::Token<Lexer::Tokens::T_NUMBER> token;

    tokenizer >> token;
    std::cout << token.value;
    Utils::Errors::SyntaxError();
    return false;
}