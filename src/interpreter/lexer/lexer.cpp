#include "lexer.h"
#include "utils.h"
#include <regex>

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

    {Lexer::Tokens::T_EOL, "end of line"},
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
Lexer::Token::Token(Lexer::Tokens t, const std::optional<T>& opt, size_t l, size_t c)
: lineno(l), column(c) {
    token = t;
    if (opt == std::nullopt) {
        if (t == Lexer::Tokens::T_NUMBER)
            throw Errors::LexerErrors::LexerNumberError{lineno, column};
        else
            throw Errors::LexerErrors::LexerStringError{lineno, column};
    }
    value = *opt;
    int i;
}

/// TOKENIZER INTERFACE
Lexer::Tokenizer::Tokenizer(Lexer::Tokenizer&& other) {
    tokens = std::move(other.tokens);
    closures = std::move(other.closures);
    lineno = other.lineno;
    column = other.column;
    text = other.text;
    pos = other.pos;

    other.lineno = 1;
    other.column = 1;
    other.text = nullptr;
    other.pos = 0;
}
Lexer::Tokenizer& Lexer::Tokenizer::operator=(Lexer::Tokenizer&& other) {
    tokens = std::move(other.tokens);
    closures = std::move(other.closures);
    lineno = other.lineno;
    column = other.column;
    text = other.text;
    pos = other.pos;

    other.lineno = 1;
    other.column = 1;
    other.text = nullptr;
    other.pos = 0;

    return *this;
}

/// SKIPPERS
void Lexer::Tokenizer::SkipWhiteSpaces() {
    while (pos < text->size() && std::isblank(text->at(pos))) { Inc(); }
}
void Lexer::Tokenizer::SkipComment() {
    if (!text->empty() && pos < text->size() - 1 && text->at(pos) == '/' && text->at(pos + 1) == '/') {
        while (pos < text->size() && text->at(pos) != '\n') { Inc(); }
    }
}

// GETTERS
std::optional<double> Lexer::Tokenizer::GetNumber() {
    auto get = [&]() { Inc(); return text->at(pos - 1) - '0'; };

    double number = 0;
    while (pos < text->size() && std::isdigit(text->at(pos))) number = number * 10 + get();

    if (pos < text->size() && text->at(pos) == '.') {
        Inc();
        if (!std::isdigit(text->at(pos))) { return std::nullopt; }

        double power_of_ten = 0.1;
        while (pos < text->size() && std::isdigit(text->at(pos))) {
            number = number + get() * power_of_ten;
            power_of_ten *= 0.1;
        }
    }

    if (pos < text->size() && text->at(pos) == 'e') {
        Inc();
        if (!std::isdigit(text->at(pos)) && text->at(pos) != '+' && text->at(pos) != '-') {
            return std::nullopt;
        }

        int power = 0;
        if (text->at(pos) == '-') {
            Inc();
            if (!std::isdigit(text->at(pos))) { return std::nullopt; }
            while (pos < text->size() && std::isdigit(text->at(pos))) power = power * 10 + get();
            power = -power;
        } else {
            if (text->at(pos) == '+') Inc();
            if (!std::isdigit(text->at(pos))) { return std::nullopt; }

            while (pos < text->size() && std::isdigit(text->at(pos))) power = power * 10 + get();
        }
        number = number * std::pow(10, power);
    }

    return number;
}
std::optional<std::string> Lexer::Tokenizer::GetString() {
    std::string str;
    Inc();

    while (pos < text->size() && text->at(pos) != '\n') {
        if (text->at(pos) == '"' && text->at(pos - 1) != '\\') {
            Inc(); return str;
        }
        str += text->at(pos);
        Inc();
    }
    return std::nullopt;
}
std::optional<std::string> Lexer::Tokenizer::GetWord() {
    std::regex re(R"([[:alpha:]_][[:alnum:]_]*)", std::regex_constants::extended);
    std::smatch match;

    std::string remaining = text->substr(pos);
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
    if (std::isdigit(text->at(pos))) return Token(Tokens::T_NUMBER, GetNumber(), lineno, column);
    if (text->at(pos) == '"') return Token(Tokens::T_STRING, GetString(), lineno, column);
    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TryComparators() {
    using namespace Lexer;
    if (pos + 1 < text->size()) {
        if (text->at(pos) == '=' && text->at(pos + 1) == '=') { DoubleInc(); return Token(Tokens::T_COMP_EQUAL, lineno, column); }
        if (text->at(pos) == '!' && text->at(pos + 1) == '=') { DoubleInc(); return Token(Tokens::T_COMP_NON_EQUAL, lineno, column); }
        if (text->at(pos) == '<' && text->at(pos + 1) == '=') { DoubleInc(); return Token(Tokens::T_COMP_SMALLER_OR_EQ, lineno, column); }
        if (text->at(pos) == '>' && text->at(pos + 1) == '=') { DoubleInc(); return Token(Tokens::T_COMP_GREATER_OR_EQ, lineno, column); }
    }

    if (text->at(pos) == '<') { Inc(); return Token(Tokens::T_COMP_SMALLER, lineno, column); }
    if (text->at(pos) == '>') { Inc(); return Token(Tokens::T_COMP_GREATER, lineno, column); }

    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TryEquals() {
    using namespace Lexer;

    if (pos + 1 < text->size()) {
        if (text->at(pos) == '+' && text->at(pos + 1) == '=') { DoubleInc(); return Token(Tokens::T_EQUAL_PLUS, lineno, column); }
        if (text->at(pos) == '-' && text->at(pos + 1) == '=') { DoubleInc(); return Token(Tokens::T_EQUAL_MINUS, lineno, column); }
        if (text->at(pos) == '*' && text->at(pos + 1) == '=') { DoubleInc(); return Token(Tokens::T_EQUAL_MULT, lineno, column); }
        if (text->at(pos) == '/' && text->at(pos + 1) == '=') { DoubleInc(); return Token(Tokens::T_EQUAL_DIV, lineno, column); }
        if (text->at(pos) == '%' && text->at(pos + 1) == '=') { DoubleInc(); return Token(Tokens::T_EQUAL_MOD, lineno, column); }
        if (text->at(pos) == '^' && text->at(pos + 1) == '=') { DoubleInc(); return Token(Tokens::T_EQUAL_XOR, lineno, column); }
    }

    if (text->at(pos) == '=') { Inc(); return Token(Tokens::T_EQUAL, lineno, column); }

    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TryOperators() {
    using namespace Lexer;
    if (text->at(pos) == '+') { Inc(); return Token(Tokens::T_PLUS, lineno, column); }
    if (text->at(pos) == '-') { Inc(); return Token(Tokens::T_MINUS, lineno, column); }
    if (text->at(pos) == '/') { Inc(); return Token(Tokens::T_DIV, lineno, column); }
    if (text->at(pos) == '*') { Inc(); return Token(Tokens::T_MULT, lineno, column); }
    if (text->at(pos) == '%') { Inc(); return Token(Tokens::T_MOD, lineno, column); }
    if (text->at(pos) == '^') { Inc(); return Token(Tokens::T_XOR, lineno, column); }

    return {};
}
std::optional<Lexer::Token> Lexer::Tokenizer::TrySyntaxes() {
    using namespace Lexer;
    if (text->at(pos) == '\n') { ++pos; ++lineno, column = 1; return Token(Tokens::T_EOL, lineno, column); }
    if (text->at(pos) == ',') { Inc(); return Token(Tokens::T_COMMA, lineno, column); }
    if (text->at(pos) == ':') { Inc(); return Token(Tokens::T_COLON, lineno, column); }
    if (text->at(pos) == '(') { Inc(); return Token(Tokens::T_LEFT_BRACKET, lineno, column); }
    if (text->at(pos) == ')') { Inc(); return Token(Tokens::T_RIGHT_BRACKET, lineno, column); }
    if (text->at(pos) == '[') { Inc(); return Token(Tokens::T_LEFT_SQUARE_BRACKET, lineno, column); }
    if (text->at(pos) == ']') { Inc(); return Token(Tokens::T_RIGHT_SQUARE_BRACKET, lineno, column); }

    return {};
}

// words
std::optional<Lexer::Token> Lexer::Tokenizer::TryKeyWords(std::string str) {
    using namespace Lexer;
    using namespace std::literals::string_literals;

    if (auto iter = key_words_.find(str); str != "end" && iter == key_words_.end()) {
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
        throw Errors::LexerErrors::LexerKeyWordError(lineno, column);
    } else {
        return Token(static_cast<Tokens>(iter->second), lineno, column);
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
    return Token(Tokens::T_VAR, std::move(word), lineno, column);
}

Lexer::Token Lexer::Tokenizer::Advance() {
    using namespace Lexer;

    SkipWhiteSpaces();
    SkipComment();

    if (pos >= text->size()) { return Token(Tokens::T_EOF, lineno, column); }

    if (auto token = TryLexems(); token) { return std::move(*token); }
    if (auto token = TryWords(); token) { return std::move(*token); }

    throw Errors::LexerErrors::LexerUnrecognizable{lineno, column};
}

Lexer::Token Lexer::Tokenizer::Peek() {
    using namespace Lexer;

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

// TOKENIZER INTERFIECE
Lexer::Tokenizer& Lexer::Tokenizer::operator<<(const std::string& str) {
    pos = 0;
    text = &str;
    Token token;

    if (tokens.size() != 0) { tokens.pop_back(); }

    auto update_closures = [this, &token]() {
        if (token.token == Tokens::T_IF
         || token.token == Tokens::T_WHILE
         || token.token == Tokens::T_FOR
         || token.token == Tokens::T_FUNC) {
            closures.push(token);
        }
        if (token.token == Tokens::T_END_IF
         || token.token == Tokens::T_END_WHILE
         || token.token == Tokens::T_END_FOR
         || token.token == Tokens::T_END_FUNC) {
            if (closures.empty()) {
                throw Closures::NonExistantClosure(token.lineno, token.column);
            }
            closures.pop();
        }
    };

    do {
        token = Advance();
        update_closures();
        tokens.push_back(token);
    } while(token.token != Tokens::T_EOF);
    if (closures.size() != 0) {
        throw Closures::UncaughtClosure(
            TOKENS_TO_STR[closures.top().token],
            closures.top().lineno,
            closures.top().column
        );
    }
    return *this;
}
Lexer::Tokenizer& Lexer::Tokenizer::operator>>(Lexer::Token& token) {
    if (tokens.empty()) {
        token = Token(Tokens::T_EOF, lineno, column);
        return *this;
    }
    token = tokens.front(); tokens.pop_front();
    return *this;
}
size_t Lexer::Tokenizer::GetClosuresSize() const {
    return closures.size();
}
