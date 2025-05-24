#include "lexer.h"

/// TOKEN
template<typename T>
Lexer::Token::Token(Lexer::Tokens t, const std::optional<T>& opt, size_t p, size_t l)
: pos(p), line(l) {
    token = t;
    if (opt == std::nullopt) {
        if (t == Lexer::Tokens::T_NUMBER)
            *this = Token(Errors::LexerErrors::LexerNumberError{}, pos, line);
        else
            *this = Token(Errors::LexerErrors::LexerStringError{}, pos, line);
        return;
    }
    value = *opt;
}

/// SKIPPERS
void Lexer::Tokenizer::SkipWhiteSpaces() {
    while (pos < text.size() && std::isblank(text[pos])) { ++pos; }
}
void Lexer::Tokenizer::SkipComment() {
    if (pos < text.size() - 1 && text[pos] == '/' && text[pos + 1] == '/') {
        pos = text.size();
    }
}

// GETTERS
std::optional<double> Lexer::Tokenizer::GetNumber() {
    auto get = [&]() { return text[pos++] - '0'; };

    double number = 0;
    while (pos < text.size() && std::isdigit(text[pos])) number = number * 10 + get();

    if (pos < text.size() && text[pos] == '.') {
        ++pos;
        if (!std::isdigit(text[pos])) { return std::nullopt; }

        double power_of_ten = 0.1;
        while (pos < text.size() && std::isdigit(text[pos])) {
            number = number + get() * power_of_ten;
            power_of_ten *= 0.1;
        }
    }

    if (pos < text.size() && text[pos] == 'e') {
        ++pos;
        if (!std::isdigit(text[pos]) && text[pos] != '+' && text[pos] != '-') { return std::nullopt; }

        int power = 0;
        if (text[pos] == '-') {
            ++pos;
            if (!std::isdigit(text[pos])) { return std::nullopt; }
            while (pos < text.size() && std::isdigit(text[pos])) power = power * 10 + get();
            power = -power;
        } else {
            if (text[pos] == '+') ++pos;
            if (!std::isdigit(text[pos])) { return std::nullopt; }

            while (pos < text.size() && std::isdigit(text[pos])) power = power * 10 + get();
        }
        number = number * std::pow(10, power);
    }

    return number;
}
std::optional<std::string> Lexer::Tokenizer::GetString() {
    std::string str;
    bool escape = false;
    ++pos;

    while (pos < text.size() && ((text[pos] != '"') | escape)) {
        if (escape) {
            escape = false;
            if (text[pos] != '\\' && text[pos] != '"') str += '\\';
            str += text[pos];
        } else if (text[pos] == '\\') {
            escape = true;
        } else {
            str += text[pos];
        }
        ++pos;
    }

    if (pos == text.size()) return std::nullopt;
    else { ++pos; }
    return str;
}
std::optional<std::string> Lexer::Tokenizer::GetWord() {
    std::regex re(R"([[:alpha:]_][[:alnum:]_]*)", std::regex_constants::extended);
    std::smatch match;

    std::string remaining = text.substr(pos);
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
    if (std::isdigit(text[pos])) return Token(Tokens::T_NUMBER, GetNumber(), DebugPos(), line);
    if (text[pos] == '"') return Token(Tokens::T_STRING, GetString(), DebugPos(), line);
    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TryComparators() {
    using namespace Lexer;
    if (text[pos] == '<') { ++pos; return Token(Tokens::T_COMP_SMALLER, DebugPos(), line); }
    if (text[pos] == '>') { ++pos; return Token(Tokens::T_COMP_GREATER, DebugPos(), line); }

    if (pos + 1 == text.size()) return {};

    if (text[pos] == '=' && text[pos + 1] == '=') { pos += 2; return Token(Tokens::T_COMP_EQUAL, DebugPos(), line); }
    if (text[pos] == '!' && text[pos + 1] == '=') { pos += 2; return Token(Tokens::T_COMP_NON_EQUAL, DebugPos(), line); }
    if (text[pos] == '<' && text[pos + 1] == '=') { pos += 2; return Token(Tokens::T_COMP_SMALLER_OR_EQ, DebugPos(), line); }
    if (text[pos] == '>' && text[pos + 1] == '=') { pos += 2; return Token(Tokens::T_COMP_GREATER_OR_EQ, DebugPos(), line); }

    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TryEquals() {
    using namespace Lexer;
    if (text[pos] == '=') { ++pos; return Token(Tokens::T_EQUAL, DebugPos(), line); }

    if (pos + 1 == text.size()) return {};

    if (text[pos] == '+' && text[pos + 1] == '=') { pos += 2; return Token(Tokens::T_EQUAL_PLUS, DebugPos(), line); }
    if (text[pos] == '-' && text[pos + 1] == '=') { pos += 2; return Token(Tokens::T_EQUAL_MINUS, DebugPos(), line); }
    if (text[pos] == '*' && text[pos + 1] == '=') { pos += 2; return Token(Tokens::T_EQUAL_MULT, DebugPos(), line); }
    if (text[pos] == '/' && text[pos + 1] == '=') { pos += 2; return Token(Tokens::T_EQUAL_DIV, DebugPos(), line); }
    if (text[pos] == '%' && text[pos + 1] == '=') { pos += 2; return Token(Tokens::T_EQUAL_MOD, DebugPos(), line); }
    if (text[pos] == '^' && text[pos + 1] == '=') { pos += 2; return Token(Tokens::T_EQUAL_XOR, DebugPos(), line); }

    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TryOperators() {
    using namespace Lexer;
    if (text[pos] == '+') { ++pos; return Token(Tokens::T_PLUS, DebugPos(), line); }
    if (text[pos] == '-') { ++pos; return Token(Tokens::T_MINUS, DebugPos(), line); }
    if (text[pos] == '/') { ++pos; return Token(Tokens::T_DIV, DebugPos(), line); }
    if (text[pos] == '*') { ++pos; return Token(Tokens::T_MULT, DebugPos(), line); }
    if (text[pos] == '%') { ++pos; return Token(Tokens::T_MOD, DebugPos(), line); }
    if (text[pos] == '^') { ++pos; return Token(Tokens::T_XOR, DebugPos(), line); }

    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TrySyntaxes() {
    using namespace Lexer;
    if (text[pos] == '\n') { ++pos; archived_pos = pos; return Token(Tokens::T_EOL, DebugPos(), ++line); }
    if (text[pos] == ',') { ++pos; return Token(Tokens::T_COMMA, DebugPos(), line); }
    if (text[pos] == ':') { ++pos; return Token(Tokens::T_COLON, DebugPos(), line); }
    if (text[pos] == '(') { ++pos; return Token(Tokens::T_LEFT_BRACKET, DebugPos(), line); }
    if (text[pos] == ')') { ++pos; return Token(Tokens::T_RIGHT_BRACKET, DebugPos(), line); }
    if (text[pos] == '[') { ++pos; return Token(Tokens::T_LEFT_SQUARE_BRACKET, DebugPos(), line); }
    if (text[pos] == ']') { ++pos; return Token(Tokens::T_RIGHT_SQUARE_BRACKET, DebugPos(), line); }

    return {};
}

// words
std::optional<Lexer::Token> Lexer::Tokenizer::TryKeyWords(std::string str) {
    using namespace Lexer;
    using namespace std::literals::string_literals;

    if (auto iter = key_words_.find(str); iter == key_words_.end()) {
        return {};
    }

    Token next_token = Peek();
    if (str == "end") {
        if (next_token.token == Tokens::T_IF) { Advance(); str += " if"s; }
        if (next_token.token == Tokens::T_WHILE) { Advance(); str += " while"s; }
        if (next_token.token == Tokens::T_FOR) { Advance(); str += " for"s; }
        if (next_token.token == Tokens::T_FUNC) { Advance(); str += " function"s; }
    }
    if (str == "else") {
        if (next_token.token == Tokens::T_IF) { Advance(); str += " if"s; }
    }

    if (auto iter = key_words_.find(str); iter == key_words_.end()) {
        return Token(Errors::LexerErrors::LexerKeyWordError(), pos, line);
    } else {
        return Token(static_cast<Tokens>(iter->second), pos, line);
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
    return Token(Tokens::T_VAR, std::move(word), DebugPos(), line);
}

Lexer::Token Lexer::Tokenizer::Advance() {
    using namespace Lexer;

    SkipWhiteSpaces();
    SkipComment();

    if (pos >= text.size()) { ++pos; return Token(Tokens::T_EOF, DebugPos(), line); }

    if (auto token = TryLexems(); token) return std::move(*token);
    if (auto token = TryWords(); token) return std::move(*token);

    return Token(Errors::LexerErrors::LexerUnrecognizable{}, DebugPos(), line);
}

Lexer::Token Lexer::Tokenizer::Peek() {
    using namespace Lexer;

    std::string str;
    size_t pos_save = pos;
    Token token = Advance();
    pos = pos_save;
    return token;
}

Lexer::Tokenizer& Lexer::Tokenizer::operator>>(Lexer::Token& token) {
    token = Advance();
    return *this;
}
