#include "memory.h"

Memory::HolderPack Memory::StackFrame::Lookup(std::string_view key) {
    try {
        return std::move(search(key));
    } catch (const Errors::MemoryErrors::NotFound&) {
        environment[key] = {{}, TYPES::NOT_SET_TYPE};
        return environment[key];
    }
}

Memory::HolderPack Memory::StackFrame::search(std::string_view key) {
    if (auto iter = environment.find(key); iter != environment.end()) {
        return iter->second;
    }
    if (parent != nullptr) { return parent->Lookup(key); }
    throw Errors::MemoryErrors::NotFound();
}