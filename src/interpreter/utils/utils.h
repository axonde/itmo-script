#pragma once
#include <iostream>
#include <string>

namespace Utils {

namespace Errors {
    static char RED[] = "\e[1;31m";
    static char WHITE[] = "\e[0m";

    void SyntaxError();
    void AbnormalError();

    struct Error {
        virtual const char* what() = 0;
    };
}

namespace Patterns {
    static char CMD[] = "\e[3;34mitmo> \e[0m";
}

}