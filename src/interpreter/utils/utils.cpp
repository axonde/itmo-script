#include "utils.h"
#include <algorithm>

namespace Errors {

void PrintError(std::string header, const Error& error) {
    std::cerr << Patterns::RED << header << Patterns::WHITE << ": "
              << error.what() << " at " << Patterns::WHITEBOLD
              << "line " << error.lineno << ", col " << error.column << Patterns::WHITE << std::endl;
}

void PrintSyntaxError(const Error& error) { PrintError("Syntax error", error); }
void PrintOperatorError(const Error& error) { PrintError("Operator error", error); };
void PrintTypeError(const Error& error) { PrintError("Type error", error); }
void PrintRunTimeError(const Error& error) { PrintError("RunTime error", error); }
void PrintPanic(const Error& error) { Errors::PrintError("Panic!", error); }

void PrintProgramSnippet(std::vector<std::string>& program, size_t lineno, size_t column) {
    size_t gap = 10;
    size_t left = std::max(0ul, column - 1 - gap);
    size_t right = std::min(program[lineno - 1].size(), column - 1 + gap);
    std::cout << '\t' << program[lineno - 1] << std::endl;
    for (size_t i = 0; i != left; ++i) { std::cout << ' '; }
    std::cout << Patterns::RED << '\t';
    for (size_t i = left; left != right; ++i) {
        std::cout << "=";
    }
    std::cout << Patterns::WHITE << std::endl;
}

} // end Errors

namespace Closures {

void PrintClosureError(const Closure& c) {
    Errors::PrintError("Closure Error", Error(c.closure, c.lineno, c.column));
}

} // end Closures

