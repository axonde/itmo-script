#include "lexer.h"

void Lexer::Tokenizer::Error() {
    Utils::Errors::SyntaxError();
}

void Lexer::Tokenizer::SkipWhiteSpaces() {
    while (pos < line.size() && std::isblank(line[pos])) { ++pos; }
}


// GETTERS

std::optional<std::string> Lexer::Tokenizer::GetVar() {
    std::regex re(R"([[:alpha:]_][[:alnum:]_]*)", std::regex_constants::extended);
    std::smatch match;

    std::string remaining = line.substr(pos);
    if (std::regex_search(remaining, match, re) && match.position() == 0) {
        std::string str = match.str();
        pos += str.size();
        return str;
    }
    return std::nullopt;
}

std::optional<double> Lexer::Tokenizer::GetNumber() {
    auto get = [&]() { return line[pos++] - '0'; };

    double number = 0;
    while (pos < line.size() && std::isdigit(line[pos])) number = number * 10 + get();

    if (pos < line.size() && line[pos] == '.') {
        ++pos;
        if (!std::isdigit(line[pos])) { return std::nullopt; }

        double power_of_ten = 0.1;
        while (pos < line.size() && std::isdigit(line[pos])) {
            number = number + get() * power_of_ten;
            power_of_ten *= 0.1;
        }
    }

    if (pos < line.size() && line[pos] == 'e') {
        ++pos;
        if (!std::isdigit(line[pos]) && line[pos] != '+' && line[pos] != '-') { return std::nullopt; }

        int power = 0;
        if (line[pos] == '-') {
            ++pos;
            if (!std::isdigit(line[pos])) { return std::nullopt; }
            while (pos < line.size() && std::isdigit(line[pos])) power = power * 10 + get();
            power = -power;
        } else {
            if (line[pos] == '+') ++pos;
            if (!std::isdigit(line[pos])) { return std::nullopt; }

            while (pos < line.size() && std::isdigit(line[pos])) power = power * 10 + get();
        }
        number = number * std::pow(10, power);
    }

    return number;
}

std::optional<std::string> Lexer::Tokenizer::GetString() {
    std::string str;
    bool escape = false;
    ++pos;

    while (pos < line.size() && ((line[pos] != '"') | escape)) {
        if (escape) {
            escape = false;
            if (line[pos] != '\\' && line[pos] != '"') str += '\\';
            str += line[pos];
        } else if (line[pos] == '\\') {
            escape = true;
        } else {
            str += line[pos];
        }
        ++pos;
    }

    if (pos == line.size()) return std::nullopt;
    else { ++pos; }
    return str;
}

std::any Lexer::Tokenizer::Advance() {
    using namespace Lexer;

    SkipWhiteSpaces();

    if (pos >= line.size()) return Token<Tokens::T_EOF>();

    if (std::isalpha(line[pos]) || line[pos] == '_') return Token<Tokens::T_VAR>(GetVar());

    if (std::isdigit(line[pos])) return Token<Tokens::T_NUMBER>(GetNumber());

    if (line[pos] == '"') return Token<Tokens::T_STRING>(GetString());

    // if ()
    return Token<Tokens::T_EOF>();
}

template<uint16_t TToken>
Lexer::Tokenizer& Lexer::Tokenizer::operator>>(Lexer::Token<TToken>& token) {
    token = std::any_cast<Lexer::Token<TToken>>(Advance());
    ++pos;
    return *this;
}

Lexer::Tokenizer& Lexer::Tokenizer::operator>>(std::any& token) {
    token = Advance();
    ++pos;
    return *this;
}
