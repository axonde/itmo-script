#include "utils.h"
#include <algorithm>

namespace Errors {

namespace GeneralErrors {

void FailedOpenFile(std::string&& path) {
    std::cerr << Patterns::RED << "Error" << Patterns::WHITE << " Could not read the file: " << path << std::endl;
}
void WrongLaunch() {
    std::cerr << Patterns::RED << "Wrong launch" << Patterns::WHITE << " Please, retry :)" << std::endl;
}

} // end GeneralErrors

void PrintError(std::string header, const Error& error) {
    *err << Patterns::RED << header << Patterns::WHITE << ' '
         << error.what() << " at " << Patterns::WHITEBOLD
         << "line " << error.lineno << ", col " << error.column << Patterns::WHITE << std::endl;
}

void PrintSyntaxError(const Error& error) { PrintError("Syntax error", error); }
void PrintOperatorError(const Error& error) { PrintError("Operator error", error); };
void PrintTypeError(const Error& error) { PrintError("Type error", error); }
void PrintRunTimeError(const Error& error) { PrintError("RunTime error", error); }
void PrintPanic(const Error& error) { Errors::PrintError("Panic!", error); }

void PrintProgramSnippet(std::vector<std::string>& program, size_t lineno, size_t column) {
    if (program.empty()) { return; }
    if (program.size() < lineno) { return; }

    const size_t gap = 2;
    size_t left = 0;
    size_t right = std::min(program[lineno - 1].size(), column - 1 + gap);
    if (column > 1 + gap) { left = column - 1 - gap; }

    *err << '\t' << program[lineno - 1] << std::endl << '\t';
    for (size_t i = 0; i != left; ++i) { *err << ' '; }
    *err << Patterns::RED;
    for (size_t i = left; i < right; ++i) { *err << "="; }
    *err << Patterns::WHITE << std::endl;
}

} // end Errors

namespace Closures {

void PrintClosureError(const Closure& c) {
    Errors::PrintError("Closure Error", Error(c.closure, c.lineno, c.column));
}

} // end Closures

