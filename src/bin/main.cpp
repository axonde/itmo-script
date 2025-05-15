#include <iostream>
#include <interpreter>

int main(int argc, char** argv) {
    std::cout << "Hello, world!" << std::endl;

    std::cout << Lexer::Tokens::T_EOF << '\n';
    return 0;
}