#include "utils.h"

void Utils::Errors::SyntaxError() {
    std::cerr << Utils::Errors::RED << "Syntax error." << Utils::Errors::WHITE;
}