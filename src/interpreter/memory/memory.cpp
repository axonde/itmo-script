#include "memory.h"

std::unordered_map<TYPES, std::string> TYPE_TO_STR = {
    {TYPES::NUM_TYPE, "number"},
    {TYPES::STRING_TYPE, "string"},
    {TYPES::NIL_TYPE, "nil"},
    {TYPES::BOOL_TYPE, "bool"},
    {TYPES::LIST_TYPE, "list"},
    {TYPES::FUNC_TYPE, "func"},
    {TYPES::NOT_SET_TYPE, "(not set type)"}
};

bool Memory::HolderPack::operator<(const Memory::HolderPack& other) const {
    return std::get<bool>(
        Operators::ExecBinaryOperation(Lexer::T_COMP_SMALLER, HolderPack(*this), HolderPack(other))->holder
    );
}

Memory::HolderPack Memory::StackFrame::Lookup(std::string_view key) {
    try {
        return search(key);
    } catch (const Errors::MemoryErrors::NotFound&) {
        environment[std::string(key)] = HolderPack();
        return environment[std::string(key)];
    }
}

Memory::HolderPack Memory::StackFrame::search(std::string_view key) {
    if (auto iter = environment.find(std::string(key)); iter != environment.end()) {
        return iter->second;
    }
    if (parent != nullptr) { return parent->search(key); }
    throw Errors::MemoryErrors::NotFound();
}
