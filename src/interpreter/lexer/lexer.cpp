#include "lexer.h"

namespace Lexer {
std::unordered_map<Lexer::Tokens, std::string> TOKENS_TO_STR = {
    {Lexer::Tokens::T_EOF, "END OF FILE"},
    {Lexer::Tokens::T_VAR, "var"},
    {Lexer::Tokens::T_NUMBER, "number"},
    {Lexer::Tokens::T_STRING, "string"},
    {Lexer::Tokens::T_NIL, "nil"},

    {Lexer::Tokens::T_THEN, "then"},
    {Lexer::Tokens::T_IN, "in"},
    {Lexer::Tokens::T_IF, "if"},
    {Lexer::Tokens::T_ELSE_IF, "else if"},
    {Lexer::Tokens::T_ELSE, "else"},
    {Lexer::Tokens::T_END_IF, "end if"},
    {Lexer::Tokens::T_WHILE, "while"},
    {Lexer::Tokens::T_END_WHILE, "end while"},
    {Lexer::Tokens::T_FOR, "for"},
    {Lexer::Tokens::T_END_FOR, "end for"},
    {Lexer::Tokens::T_BREAK, "break"},
    {Lexer::Tokens::T_CONTINUE, "continue"},

    {Lexer::Tokens::T_FUNC, "function"},
    {Lexer::Tokens::T_RETURN, "return"},
    {Lexer::Tokens::T_END_FUNC, "end function"},

    {Lexer::Tokens::T_AND, "and"},
    {Lexer::Tokens::T_OR, "or"},
    {Lexer::Tokens::T_NOT, "not"},
    {Lexer::Tokens::T_FALSE, "false"},
    {Lexer::Tokens::T_TRUE, "true"},

    {Lexer::Tokens::T_COMP_EQUAL, "=="},
    {Lexer::Tokens::T_COMP_NON_EQUAL, "!="},
    {Lexer::Tokens::T_COMP_SMALLER, "<"},
    {Lexer::Tokens::T_COMP_GREATER, ">"},
    {Lexer::Tokens::T_COMP_SMALLER_OR_EQ, "<="},
    {Lexer::Tokens::T_COMP_GREATER_OR_EQ, ">="},

    {Lexer::Tokens::T_EQUAL, "="},
    {Lexer::Tokens::T_EQUAL_PLUS, "+="},
    {Lexer::Tokens::T_EQUAL_MINUS, "-="},
    {Lexer::Tokens::T_EQUAL_MULT, "*="},
    {Lexer::Tokens::T_EQUAL_DIV, "/="},
    {Lexer::Tokens::T_EQUAL_MOD, "%="},
    {Lexer::Tokens::T_EQUAL_XOR, "^="},

    {Lexer::Tokens::T_PLUS, "+"},
    {Lexer::Tokens::T_MINUS, "-"},
    {Lexer::Tokens::T_DIV, "/"},
    {Lexer::Tokens::T_MULT, "*"},
    {Lexer::Tokens::T_MOD, "%"},
    {Lexer::Tokens::T_XOR, "^"},

    {Lexer::Tokens::T_EOL, "end of lineno"},
    {Lexer::Tokens::T_COMMA, ","},
    {Lexer::Tokens::T_COLON, ":"},
    {Lexer::Tokens::T_LEFT_BRACKET, "("},
    {Lexer::Tokens::T_RIGHT_BRACKET, ")"},
    {Lexer::Tokens::T_LEFT_SQUARE_BRACKET, "["},
    {Lexer::Tokens::T_RIGHT_SQUARE_BRACKET, "]"}
};
}

/// TOKEN
template<typename T>
Lexer::Token::Token(Lexer::Tokens t, const std::optional<T>& opt, size_t c, size_t l)
: column(c), lineno(l) {
    token = t;
    if (opt == std::nullopt) {
        if (t == Lexer::Tokens::T_NUMBER)
            *this = Token(Errors::LexerErrors::LexerNumberError{}, column, lineno);
        else
            *this = Token(Errors::LexerErrors::LexerStringError{}, column, lineno);
        return;
    }
    value = *opt;
}

/// SKIPPERS
void Lexer::Tokenizer::SkipWhiteSpaces() {
    while (pos < text.size() && std::isblank(text[pos])) { Inc(); }
}
void Lexer::Tokenizer::SkipComment() {
    if (pos < text.size() - 1 && text[pos] == '/' && text[pos + 1] == '/') {
        while (pos < text.size() && text[pos] != '\n') { Inc(); }
    }
}

// GETTERS
std::optional<double> Lexer::Tokenizer::GetNumber() {
    auto get = [&]() { Inc(); return text[pos - 1] - '0'; };

    double number = 0;
    while (pos < text.size() && std::isdigit(text[pos])) number = number * 10 + get();

    if (pos < text.size() && text[pos] == '.') {
        Inc();
        if (!std::isdigit(text[pos])) { return std::nullopt; }

        double power_of_ten = 0.1;
        while (pos < text.size() && std::isdigit(text[pos])) {
            number = number + get() * power_of_ten;
            power_of_ten *= 0.1;
        }
    }

    if (pos < text.size() && text[pos] == 'e') {
        Inc();
        if (!std::isdigit(text[pos]) && text[pos] != '+' && text[pos] != '-') { return std::nullopt; }

        int power = 0;
        if (text[pos] == '-') {
            Inc();
            if (!std::isdigit(text[pos])) { return std::nullopt; }
            while (pos < text.size() && std::isdigit(text[pos])) power = power * 10 + get();
            power = -power;
        } else {
            if (text[pos] == '+') Inc();
            if (!std::isdigit(text[pos])) { return std::nullopt; }

            while (pos < text.size() && std::isdigit(text[pos])) power = power * 10 + get();
        }
        number = number * std::pow(10, power);
    }

    return number;
}
std::optional<std::string> Lexer::Tokenizer::GetString() {
    std::string str;
    Inc();

    while (pos < text.size() && text[pos] != '\n') {
        if (text[pos] == '"' && text[pos - 1] != '\\') {
            Inc(); return str;
        }
        str += text[pos];
        Inc();
    }
    return std::nullopt;
}
std::optional<std::string> Lexer::Tokenizer::GetWord() {
    std::regex re(R"([[:alpha:]_][[:alnum:]_]*)", std::regex_constants::extended);
    std::smatch match;

    std::string remaining = text.substr(pos);
    if (std::regex_search(remaining, match, re) && match.position() == 0) {
        std::string str = match.str();
        pos += str.size();
        column += str.size();
        return str;
    }
    return std::nullopt;
}

/// TRYERS

// lexems
std::optional<Lexer::Token> Lexer::Tokenizer::TryLiterals() {
    using namespace Lexer;
    if (std::isdigit(text[pos])) return Token(Tokens::T_NUMBER, GetNumber(), column, lineno);
    if (text[pos] == '"') return Token(Tokens::T_STRING, GetString(), column, lineno);
    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TryComparators() {
    using namespace Lexer;
    if (pos + 1 < text.size()) {
        if (text[pos] == '=' && text[pos + 1] == '=') { DoubleInc(); return Token(Tokens::T_COMP_EQUAL, column, lineno); }
        if (text[pos] == '!' && text[pos + 1] == '=') { DoubleInc(); return Token(Tokens::T_COMP_NON_EQUAL, column, lineno); }
        if (text[pos] == '<' && text[pos + 1] == '=') { DoubleInc(); return Token(Tokens::T_COMP_SMALLER_OR_EQ, column, lineno); }
        if (text[pos] == '>' && text[pos + 1] == '=') { DoubleInc(); return Token(Tokens::T_COMP_GREATER_OR_EQ, column, lineno); }
    }

    if (text[pos] == '<') { Inc(); return Token(Tokens::T_COMP_SMALLER, column, lineno); }
    if (text[pos] == '>') { Inc(); return Token(Tokens::T_COMP_GREATER, column, lineno); }

    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TryEquals() {
    using namespace Lexer;
    if (text[pos] == '=') { Inc(); return Token(Tokens::T_EQUAL, column, lineno); }

    if (pos + 1 == text.size()) return {};

    if (text[pos] == '+' && text[pos + 1] == '=') { DoubleInc(); return Token(Tokens::T_EQUAL_PLUS, column, lineno); }
    if (text[pos] == '-' && text[pos + 1] == '=') { DoubleInc(); return Token(Tokens::T_EQUAL_MINUS, column, lineno); }
    if (text[pos] == '*' && text[pos + 1] == '=') { DoubleInc(); return Token(Tokens::T_EQUAL_MULT, column, lineno); }
    if (text[pos] == '/' && text[pos + 1] == '=') { DoubleInc(); return Token(Tokens::T_EQUAL_DIV, column, lineno); }
    if (text[pos] == '%' && text[pos + 1] == '=') { DoubleInc(); return Token(Tokens::T_EQUAL_MOD, column, lineno); }
    if (text[pos] == '^' && text[pos + 1] == '=') { DoubleInc(); return Token(Tokens::T_EQUAL_XOR, column, lineno); }

    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TryOperators() {
    using namespace Lexer;
    if (text[pos] == '+') { Inc(); return Token(Tokens::T_PLUS, column, lineno); }
    if (text[pos] == '-') { Inc(); return Token(Tokens::T_MINUS, column, lineno); }
    if (text[pos] == '/') { Inc(); return Token(Tokens::T_DIV, column, lineno); }
    if (text[pos] == '*') { Inc(); return Token(Tokens::T_MULT, column, lineno); }
    if (text[pos] == '%') { Inc(); return Token(Tokens::T_MOD, column, lineno); }
    if (text[pos] == '^') { Inc(); return Token(Tokens::T_XOR, column, lineno); }

    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TrySyntaxes() {
    using namespace Lexer;
    if (text[pos] == '\n') { ++pos; ++lineno, column = 1; return Token(Tokens::T_EOL, column, lineno); }
    if (text[pos] == ',') { Inc(); return Token(Tokens::T_COMMA, column, lineno); }
    if (text[pos] == ':') { Inc(); return Token(Tokens::T_COLON, column, lineno); }
    if (text[pos] == '(') { Inc(); return Token(Tokens::T_LEFT_BRACKET, column, lineno); }
    if (text[pos] == ')') { Inc(); return Token(Tokens::T_RIGHT_BRACKET, column, lineno); }
    if (text[pos] == '[') { Inc(); return Token(Tokens::T_LEFT_SQUARE_BRACKET, column, lineno); }
    if (text[pos] == ']') { Inc(); return Token(Tokens::T_RIGHT_SQUARE_BRACKET, column, lineno); }

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

    if (auto iter = key_words_.find(str); str == "end" || iter == key_words_.end()) {
        return Token(Errors::LexerErrors::LexerKeyWordError(), column, lineno);
    } else {
        return Token(static_cast<Tokens>(iter->second), column, lineno);
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
    if (auto w = GetWord(); w) { word = std::move(*w); }
    else return {};

    if (auto token = TryKeyWords(word); token) { return token; }
    return Token(Tokens::T_VAR, std::move(word), column, lineno);
}

Lexer::Token Lexer::Tokenizer::Advance() {
    using namespace Lexer;

    SkipWhiteSpaces();
    SkipComment();

    if (pos >= text.size()) { return Token(Tokens::T_EOF, column, lineno); }

    if (auto token = TryLexems(); token) { return std::move(*token); }
    if (auto token = TryWords(); token) { return std::move(*token); }

    return Token(Errors::LexerErrors::LexerUnrecognizable{}, column, lineno);
}

Lexer::Token Lexer::Tokenizer::Peek() {
    using namespace Lexer;

    std::string str;
    size_t pos_save = pos;
    auto column_save = column;
    size_t lineno_save = lineno;
    Token token = Advance();
    pos = pos_save;
    column = column_save;
    lineno = lineno_save;
    return token;
}

void Lexer::Tokenizer::Inc() { ++pos; ++column; }
void Lexer::Tokenizer::DoubleInc() { pos += 2; column += 2; }

Lexer::Tokenizer& Lexer::Tokenizer::operator>>(Lexer::Token& token) {
    token = Advance();
    return *this;
}
