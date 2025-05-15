#include "lexer.h"

void Lexer::Tokenizer::Error() {
    Utils::Errors::SyntaxError();
}