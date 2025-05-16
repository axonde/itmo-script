#pragma once
#include <any>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <optional>
// #include <regex>
#include <string>

#include "utils.h"

namespace Lexer {

enum Tokens : uint16_t {
    T_EOF,                          // END OF LINE
    T_VAR,                          // `var`, `Var_Var`, `var__0`, `_var_`
    T_NUMBER,                       // 12, -123, 1.2e-12
    T_STRING,                       // "string"
    T_NULL,                         // `nil`
    T_COMMENT,                      // `//{line}`, `/*{block}*/`

    // SYNTAX SYMBOLS
    T_COLON,                        // `:`
    T_QUOTES,                       // `""`
    T_LEFT_BRACKET,                 // `(`
    T_RIGHT_BRACKET,                // `)`
    T_LEFT_SQUARE_BRACKET,          // `[`
    T_RIGHT_SQUARE_BRACKET,         // `]`

    // STATEMENTS
    T_THEN,                         // `then`
    T_IN,                           // `in`
    T_IF,                           // `if`
    T_ELSE_IF,                      // `else if`
    T_ELSE,                         // `else`
    T_END_IF,                       // `end if`
    T_WHILE,                        // `while`
    T_END_WHILE,                    // `end while`
    T_FOR,                          // `for`
    T_END_FOR,                      // `end for`
    T_BREAK,                        // `break`
    T_CONTINUE,                     // `continue`

    // FUNCTIONS
    T_FUNC,                         // `function`
    T_RETURN,                       // `return`
    T_END_FUNC,                     // `end function`

    // OPERATORS
    T_COMMA,                        // `,`
    T_PLUS,                         // `+`
    T_MINUS,                        // `-`
    T_DIV,                          // `/`
    T_MULT,                         // `*`
    T_MOD,                          // `%`
    T_XOR,                          // `^`

    // EQUALS
    T_EQUAL,                        // `=`
    T_EQUAL_PLUS,                   // `+=`
    T_EQUAL_MINUS,                  // `-=`
    T_EQUAL_MULT,                   // `*=`
    T_EQUAL_DIV,                    // `/=`
    T_EQUAL_MOD,                    // `%=`
    T_EQUAL_XOR,                    // `^=`

    // COMPARATORS
    T_COMP_EQUAL,                   // `==`
    T_COMP_NON_EQUAL,               // `!=`
    T_COMP_SMALLER,                 // `<`
    T_COMP_GREATER,                 // `>`
    T_COMP_SMALLER_OR_EQ,           // `<=`
    T_COMP_GREATER_OR_EQ,           // `>=`

    // LOGICS
    T_AND,                          // `and`
    T_OR,                           // `or`
    T_NOT,                          // `not`
    T_BOOL_FALSE,                   // `false`
    T_BOOL_TRUE,                    // `true`

    // // ERROR
    // T_BAD                           // ill formed token
};

template<uint16_t TToken>
struct Token {
    Token() = default;
};

template<>
struct Token<Lexer::Tokens::T_VAR> {
    Token() = default;
    Token(const std::optional<std::string>& v) {
        if (v) value = *v;
        else throw;
    }
    Token(std::optional<std::string>&& v) {
        if (v) value = std::move(*v);
        else throw;
    }
    std::string value;
};

template<>
struct Token<Lexer::Tokens::T_NUMBER> {
    Token() = default;
    Token(const std::optional<double>& v) {
        if (v) value = *v;
        else throw;
    }
    Token(std::optional<double>&& v) {
        if (v) value = std::move(*v);
        else throw;
    }
    double value;
};

template<>
struct Token<Lexer::Tokens::T_STRING> {
    Token() = default;
    Token(const std::optional<std::string>& v) {
        if (v) value = *v;
        else throw;
    }
    Token(std::optional<std::string>&& v) {
        if (v) value = std::move(*v);
        else throw;
    }
    std::string value;
};


// Lexical Analyzer
class Tokenizer {
public:
    Tokenizer(const std::string& str) : line(str) {}

    void Error();

    template<uint16_t TToken>
    Tokenizer& operator>>(Lexer::Token<TToken>& token);
    Tokenizer& operator>>(std::any& token);

private:
    size_t pos = 0;
    std::string line;

    std::any Advance();

    void SkipWhiteSpaces();
    std::optional<std::string> GetVar();
    std::optional<double> GetNumber();
    std::optional<std::string> GetString();
};


}
