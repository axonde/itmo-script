#pragma once
#include <iostream>

namespace Utils {

namespace Errors {
    static char RED[8] = "\e[1;31m";
    static char WHITE[5] = "\e[0m";

    void SyntaxError();
}

}