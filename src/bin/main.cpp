#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <interpreter>

int main(int argc, char** argv) {
    if (argc == 1) {
        Patterns::Welcome();

        while (true) {
            if (!Interpreter::Interpret(std::cin, std::cout)) {
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

    size_t size = file.tellg();
    file.seekg(0);
    std::string program(size, '\0');
    file.read(&program[0], size);

    if (!Interpreter::Interpret(program, std::cout)) {
        return 1;
    }

    return 0;
}