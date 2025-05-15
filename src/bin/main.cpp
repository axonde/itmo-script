#include <iostream>
#include <interpreter>

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "Welcome to \e[1;34mITMO SCRIPT\e[0m!" << std::endl;
        std::cout << '\n';

        while (true) {
            std::cout << "\e[3;34mitmo> \e[0m";
            if (!interpret(std::cin, std::cout)) {
                return 1;
            }
            std::cout << std::endl;
        }
        return 0;
    }

    std::cout << "File reading is not implement yet." << std::endl;
    
    return 0;
}