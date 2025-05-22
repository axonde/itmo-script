#include "utils.h"

void Utils::Errors::SyntaxError() {
    std::cerr << Utils::Errors::RED << "Syntax error." << Utils::Errors::WHITE;
}

void Utils::Errors::AbnormalError() {
    std::cerr << Utils::Errors::RED << "Abnormal error." << Utils::Errors::WHITE;
}