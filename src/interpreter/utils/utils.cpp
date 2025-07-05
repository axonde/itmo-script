#include "utils.h"

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
