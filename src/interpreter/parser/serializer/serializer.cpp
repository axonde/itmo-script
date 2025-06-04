#include "serializer.h"

int main(int argc, char** argv) {
    if (argc != 2) return 1;

    std::filesystem::path path = argv[1];
    std::fstream file{path, std::ios::in | std::ios::ate};

    if (!file.is_open()) return 2;

    size_t size = file.tellg();
    file.seekg(0);
    std::string program(size, '\0');
    file.read(&program[0], size);

    Serializer serializer(Parser(Lexer::Tokenizer(std::move(program))));
    std::cout << serializer.tree.dump(2) << '\n';
}