#include "lexer.h"

/// ERRORS
void Lexer::Tokenizer::Error(size_t pos) {
    Utils::Errors::SyntaxError();
}

/// TOKEN
template<typename T>
Lexer::Token::Token(Lexer::Tokens t, const std::optional<T>& opt) {
    token = t;
    if (opt == std::nullopt) { throw; }
    value = *opt;
}

auto Lexer::Token::Get() const { return value; }

/// SKIPPERS
void Lexer::Tokenizer::SkipWhiteSpaces() {
    while (pos < line.size() && std::isblank(line[pos])) { ++pos; }
}
void Lexer::Tokenizer::SkipComment() {
    if (pos < line.size() - 1 && line[pos] == '/' && line[pos + 1] == '/') {
        pos = line.size();
    }
}

// GETTERS
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
std::optional<std::string> Lexer::Tokenizer::GetWord() {
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

/// TRYERS

// lexems
std::optional<Lexer::Token> Lexer::Tokenizer::TryLiterals() {
    using namespace Lexer;
    if (std::isdigit(line[pos])) return Token(Tokens::T_NUMBER, GetNumber());
    if (line[pos] == '"') return Token(Tokens::T_STRING, GetString());
    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TryComparators() {
    using namespace Lexer;
    if (line[pos] == '<') { ++pos; return Token(Tokens::T_COMP_SMALLER); }
    if (line[pos] == '>') { ++pos; return Token(Tokens::T_COMP_GREATER); }

    if (pos + 1 == line.size()) return {};

    if (line[pos] == '=' && line[pos + 1] == '=') { pos += 2; return Token(Tokens::T_COMP_EQUAL); }
    if (line[pos] == '!' && line[pos + 1] == '=') { pos += 2; return Token(Tokens::T_COMP_NON_EQUAL); }
    if (line[pos] == '<' && line[pos + 1] == '=') { pos += 2; return Token(Tokens::T_COMP_SMALLER_OR_EQ); }
    if (line[pos] == '>' && line[pos + 1] == '=') { pos += 2; return Token(Tokens::T_COMP_GREATER_OR_EQ); }

    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TryEquals() {
    using namespace Lexer;
    if (line[pos] == '=') { ++pos; return Token(Tokens::T_EQUAL); }

    if (pos + 1 == line.size()) return {};

    if (line[pos] == '+' && line[pos + 1] == '=') { pos += 2; return Token(Tokens::T_EQUAL_PLUS); }
    if (line[pos] == '-' && line[pos + 1] == '=') { pos += 2; return Token(Tokens::T_EQUAL_MINUS); }
    if (line[pos] == '*' && line[pos + 1] == '=') { pos += 2; return Token(Tokens::T_EQUAL_MULT); }
    if (line[pos] == '/' && line[pos + 1] == '=') { pos += 2; return Token(Tokens::T_EQUAL_DIV); }
    if (line[pos] == '%' && line[pos + 1] == '=') { pos += 2; return Token(Tokens::T_EQUAL_MOD); }
    if (line[pos] == '^' && line[pos + 1] == '=') { pos += 2; return Token(Tokens::T_EQUAL_XOR); }

    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TryOperators() {
    using namespace Lexer;
    if (line[pos] == '+') { ++pos; return Token(Tokens::T_PLUS); }
    if (line[pos] == '-') { ++pos; return Token(Tokens::T_MINUS); }
    if (line[pos] == '/') { ++pos; return Token(Tokens::T_DIV); }
    if (line[pos] == '*') { ++pos; return Token(Tokens::T_MULT); }
    if (line[pos] == '%') { ++pos; return Token(Tokens::T_MOD); }
    if (line[pos] == '^') { ++pos; return Token(Tokens::T_XOR); }

    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TrySyntaxes() {
    using namespace Lexer;
    if (line[pos] == ',') { ++pos; return Token(Tokens::T_COMMA); }
    if (line[pos] == ':') { ++pos; return Token(Tokens::T_COLON); }
    if (line[pos] == '(') { ++pos; return Token(Tokens::T_LEFT_BRACKET); }
    if (line[pos] == ')') { ++pos; return Token(Tokens::T_RIGHT_BRACKET); }
    if (line[pos] == '[') { ++pos; return Token(Tokens::T_LEFT_SQUARE_BRACKET); }
    if (line[pos] == ']') { ++pos; return Token(Tokens::T_RIGHT_SQUARE_BRACKET); }

    return {};
}

// words
std::optional<Lexer::Token> Lexer::Tokenizer::TryKeyWords(std::string str) {
    using namespace Lexer;
    using namespace std::literals::string_literals;

    if (auto iter = key_words_.find(str); iter == key_words_.end()) {
        return {};
    }
    /// not finished here.
    size_t pos_save = pos;
    Token token = Advance();
    if (token.token == Lexer::Tokens::T_IF) str += " if"s;
    if (token.token == Lexer::Tokens::T_WHILE) str += " while"s;
    if (token.token == Lexer::Tokens::T_FUNC) str += " func"s;

    if (auto iter = key_words_.find(str); iter == key_words_.end()) {
        pos = pos_save;
        return {};
    } else {
        return Token(static_cast<Tokens>(iter->second));
    }
}


// GENERELIZING TRYERS
std::optional<Lexer::Token> Lexer::Tokenizer::TryLexems() {
    if (auto token = TryLiterals(); token) return token;
    if (auto token = TryComparators(); token) return token;
    if (auto token = TryEquals(); token) return token;
    if (auto token = TryOperators(); token) return token;
    if (auto token = TrySyntaxes(); token) return token;
    return {};
}

std::optional<Lexer::Token> Lexer::Tokenizer::TryWords() {
    using namespace Lexer;
    std::string word;
    if (auto w = GetWord(); w) word = std::move(*w);
    else return {};

    if (auto token = TryKeyWords(word); token) return token;
    return Token(Tokens::T_VAR, std::move(word));
}

Lexer::Token Lexer::Tokenizer::Advance() {
    using namespace Lexer;

    SkipWhiteSpaces();
    SkipComment();

    if (pos >= line.size()) return Token(Tokens::T_EOF);

    try {
        if (auto token = TryLexems(); token) return std::move(*token);
        if (auto token = TryWords(); token) return std::move(*token);
    } catch (...) {
        return Token(Tokens::T_BAD);
    }

    return Token(Tokens::T_BAD);
}

Lexer::Tokenizer& Lexer::Tokenizer::operator>>(Lexer::Token& token) {
    token = Advance();
    ++pos;
    return *this;
}
