#include "utils.h"

void Errors::SyntaxError() {
    std::cerr << Errors::RED << "Syntax error." << Errors::WHITE;
}

void Errors::AbnormalError() {
    std::cerr << Errors::RED << "Abnormal error." << Errors::WHITE;
}