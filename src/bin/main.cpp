#include <iostream>
#include <filesystem>
#include <fstream>
#include <interpreter>

int main(int argc, char** argv) {

    if (argc == 1) {
        Interpreter interpreter;
        Patterns::Welcome();

        while (true) {
            if (!interpreter.Interpret(std::cin, true)) {
                return 1;
            }
        }
        return 0;
    }

    std::filesystem::path path(argv[1]);
    std::fstream file(path, std::ios::in);

    Interpreter interpreter;

    if (!file.is_open()) {
        Errors::GeneralErrors::FailedOpenFile();
        return 1;
    }

    if (!interpreter.Interpret(file, false)) {
        return 1;
    }

    return 0;
}
