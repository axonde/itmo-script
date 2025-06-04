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

            // IsRepl is not the Interpret deal.
            // if (IsRepl) {
            //     std::string line;
            //     do {
            //         std::cout << Patterns::CMD;
            //         std::getline(input, line);
            //     } while (line.size() == 0);

            //     Lexer::Tokenizer tokenizer(line);

            //     Lexer::Token token;
            //     tokenizer >> token;
            //     while (token.token != Lexer::Tokens::T_EOF) {
            //         std::cout << token.token << " ; ";
            //         if (token.token == Lexer::Tokens::T_BAD) {
            //             SyntaxError(token);
            //             return false;
            //         }
            //         tokenizer >> token;
            //     }
            //     std::cout << '\n';

            //     return true;
            // }

        }
        return 0;
    }

    std::filesystem::path path(argv[1]);
    std::fstream file(path, std::ios::in | std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        Errors::ErrorOpenFile();
        return 1;
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);
    std::string program(size, '\0');
    file.read(&program[0], size);

    if (!Interpreter::Interpret(program, std::cin, std::cout)) {
        return 1;
    }

    return 0;
}