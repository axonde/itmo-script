#include "lexer.h"

void Lexer::Tokenizer::Error() {
    Utils::Errors::SyntaxError();
}

void Lexer::Tokenizer::SkipWhiteSpaces() {
    while (pos < line.size() && std::isblank(line[pos])) { ++pos; }
}

double Lexer::Tokenizer::GetNumber() {
    double number = 0;
    while (pos < line.size() && std::isdigit(line[pos])) number = number * 10 + line[pos++] - '0';
    return number;
}

std::any Lexer::Tokenizer::Advance() {
    using namespace Lexer;

    SkipWhiteSpaces();

    if (pos >= line.size()) return Token<void>(Tokens::T_EOF);

    if (std::isdigit(line[pos])) return Token<double>(Tokens::T_NUMBER, GetNumber());

    // if ()
    return Token<void>(Tokens::T_EOF);
}

template<typename T>
Lexer::Tokenizer& Lexer::Tokenizer::operator>>(Lexer::Token<T>& token) {
    token = std::any_cast<Lexer::Token<T>>(Advance());
    ++pos;
    return *this;
}