#include <cstdint>
#include <iostream>

namespace Lexer {

enum Tokens : uint16_t {
    T_EOF,                          // END OF LINE
    T_NUMBER,                       // 12, -123, 1.2e-12
    T_STRING,                       // string, string_0, string0, string0string, ...
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
    T_BOOL_TRUE                     // `true`
};


}
