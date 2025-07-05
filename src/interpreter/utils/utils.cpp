#include "utils.h"
#include <algorithm>

namespace Errors {

void PrintError(std::string header, const Error& error) {
    std::cerr << Patterns::RED << header << Patterns::WHITE << ": "
              << error.what() << " at " << Patterns::WHITEBOLD
              << "line " << error.lineno << ", col " << error.column << Patterns::WHITE << std::endl;
}

void PrintSyntaxError(const Error& error) {
    Errors::PrintError("Syntax error", error);
}
void PrintRunTimeError(const Error& error) {
    Errors::PrintError("RunTime error", error);
}
void PrintPanic(const Error& error) {
    Errors::PrintError("Panic!", error);
}

void PrintProgramSnipper(std::vector<std::string>& program, size_t lineno, size_t column) {
    size_t gap = 10;
    size_t left = std::max(0ul, column - 1 - gap);
    size_t right = std::min(program[lineno - 1].size(), column - 1 + gap);
    std::cout << '\t' << program[lineno - 1] << std::endl;
    for (size_t i = 0; i != left; ++i) { std::cout << ' '; }
    std::cout << Patterns::RED << '\t';
    for (size_t i = left; left != right; ++i) { std::cout << "="; }
    std::cout << Patterns::WHITE << std::endl;
}

} // end Errors

namespace Closures {

void PrintClosureError(const Closure& c) {
    Errors::PrintError("Closure Error", Error(c.closure, c.lineno, c.column));
}

} // end Closures

namespace Utils {

template<typename Comp>
std::vector<std::string> Split(const std::string& string, const Comp& comp) {
    std::vector<std::string> splitted_string;
    int l = 0;
    for (int r = 0; r != string.size() + 1; r++) {
        if (r == string.size() || comp(string[r])) {
            splitted_string.push_back(string.substr(l, r - l));
            l = r + 1;
        }
    }
    return splitted_string;
}

} // end Utils
