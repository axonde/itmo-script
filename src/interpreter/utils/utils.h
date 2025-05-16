#pragma once
#include <iostream>

namespace Utils {

namespace Errors {
    static char RED[] = "\e[1;31m";
    static char WHITE[] = "\e[0m";

    void SyntaxError();
}

namespace Patterns {
    static char CMD[] = "\e[3;34mitmo> \e[0m";
}

}