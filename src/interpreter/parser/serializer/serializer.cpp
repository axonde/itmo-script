#include "serializer.h"
#include <iostream>
#include <filesystem>
#include <fstream>

int main(int argc, char** argv) {
    if (argc != 2) return 1;

    std::filesystem::path path = argv[1];
    std::fstream file{path, std::ios::in | std::ios::ate};

    if (!file.is_open()) return 2;

    size_t size = file.tellg();
    file.seekg(0);
    std::string program(size, '\0');
    file.read(&program[0], size);

    Lexer::Tokenizer tokenizer;
    try {
        tokenizer << program;
        Serializer serializer(Parser(std::move(tokenizer)));
        std::cout << serializer.tree.dump(2) << '\n';
        return 0;
    } catch (Closure& c) {
        std::cerr << "Closure error occured: " << c.closure << '\n';
    } catch (Error& e) {
        std::cerr << "Error occured: " << e.what() << '\n';
    }
    return 1;
}
