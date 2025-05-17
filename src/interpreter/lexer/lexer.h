#pragma once
#include <cctype>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>

#include "utils.h"

namespace Lexer {

enum Tokens : uint16_t {
    // BASE
    T_EOF,                          // END OF LINE
    T_VAR,                          // `var`, `Var_Var`, `var__0`, `_var_`
    T_NUMBER,                       // 12, -123, 1.2e-12
    T_STRING,                       // "string"
    T_NULL,                         // `nil`

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

    // LOGICS
    T_AND,                          // `and`
    T_OR,                           // `or`
    T_NOT,                          // `not`
    T_BOOL_FALSE,                   // `false`
    T_BOOL_TRUE,                    // `true`

    // COMPARATORS
    T_COMP_EQUAL,                   // `==`
    T_COMP_NON_EQUAL,               // `!=`
    T_COMP_SMALLER,                 // `<`
    T_COMP_GREATER,                 // `>`
    T_COMP_SMALLER_OR_EQ,           // `<=`
    T_COMP_GREATER_OR_EQ,           // `>=`

    // EQUALS
    T_EQUAL,                        // `=`
    T_EQUAL_PLUS,                   // `+=`
    T_EQUAL_MINUS,                  // `-=`
    T_EQUAL_MULT,                   // `*=`
    T_EQUAL_DIV,                    // `/=`
    T_EQUAL_MOD,                    // `%=`
    T_EQUAL_XOR,                    // `^=`

    // OPERATORS
    T_PLUS,                         // `+`
    T_MINUS,                        // `-`
    T_DIV,                          // `/`
    T_MULT,                         // `*`
    T_MOD,                          // `%`
    T_XOR,                          // `^`

    // SYNTAX SYMBOLS
    T_COMMA,                        // `,`
    T_COLON,                        // `:`
    T_LEFT_BRACKET,                 // `(`
    T_RIGHT_BRACKET,                // `)`
    T_LEFT_SQUARE_BRACKET,          // `[`
    T_RIGHT_SQUARE_BRACKET,         // `]`

    // ERROR
    T_BAD                           // ill formed token
};

class Token {
public:
    Token() : token(Tokens::T_BAD), value(std::monostate{}) {}

    Token(Tokens t, double v)
    : token(t), value(v) {}

    Token(Tokens t, const std::string& v)
    : token(t), value(v) {}
    Token(Tokens t, std::string&& v)
    : token(t), value(v) {}

    // Comfy function, that give you a possibility to pass an optional object and get a throw if that is empty.
    template<typename T>
    Token(Tokens, const std::optional<T>&);

    Token(Tokens t) : token(t), value(std::monostate{}) {}

    auto Get() const;

    Tokens token;
private:
    std::variant<std::monostate, double, std::string> value;
};


// Lexical Analyzer
class Tokenizer {
public:
    Tokenizer(const std::string& str) : line(str) {}

    void Error();

    Tokenizer& operator>>(Lexer::Token& token);

private:
    size_t pos = 0;
    std::string line;

    Lexer::Token Advance();

    /// SKIPPERS
    void SkipWhiteSpaces();
    void SkipComment();

    /// TRYERS
    std::optional<Lexer::Token> TryLexems(); // - num literal
                                             // - str literal
                                             // - comparators
                                             // - equals
                                             // - operators
                                             // - syntaxes

    std::optional<Lexer::Token> TryWords(); // - functions
                                            // - statements
                                            // - logics
                                            // - variables

    // lexems
    std::optional<Lexer::Token> TryLiterals();
    std::optional<Lexer::Token> TryComparators();
    std::optional<Lexer::Token> TryEquals();
    std::optional<Lexer::Token> TryOperators();
    std::optional<Lexer::Token> TrySyntaxes();

    // words
    std::optional<Lexer::Token> TryKeyWords(std::string);

    /// GETERS
    std::optional<double> GetNumber();
    std::optional<std::string> GetString();
    std::optional<std::string> GetWord();


    std::unordered_map<std::string, uint16_t> key_words_ = {
    // statements
        {"then", Tokens::T_THEN},
        {"in", Tokens::T_IN},
        {"if", Tokens::T_IF},
        {"else if", Tokens::T_ELSE_IF},
        {"else", Tokens::T_ELSE},
        {"end if", Tokens::T_END_IF},
        {"while", Tokens::T_WHILE},
        {"end while", Tokens::T_END_WHILE},
        {"for", Tokens::T_FOR},
        {"end for", Tokens::T_END_FOR},
        {"break", Tokens::T_BREAK},
        {"continue", Tokens::T_CONTINUE},

    // functions
        {"function", Tokens::T_FUNC},
        {"return", Tokens::T_RETURN},
        {"end function", Tokens::T_END_FUNC},

    // logics
        {"and", Tokens::T_AND},
        {"or", Tokens::T_OR},
        {"not", Tokens::T_NOT},
        {"false", Tokens::T_BOOL_FALSE},
        {"true", Tokens::T_BOOL_TRUE}
    };
};


}
