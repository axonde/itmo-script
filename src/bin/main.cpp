#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <interpreter>

int main(int argc, char** argv) {
    if (argc == 1) {
        Patterns::Welcome();

        while (true) {
            if (!Interpreter::Interpret(std::cin, std::cout, true)) {
                return 1;
            }
        }
        return 0;
    }

    std::filesystem::path path(argv[1]);
    std::fstream file(path, std::ios::in | std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        Errors::ErrorOpenFile();
        return 1;
    }

    if (!Interpreter::Interpret(file, std::cout)) {
        return 1;
    }

    return 0;
}