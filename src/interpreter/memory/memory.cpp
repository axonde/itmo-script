#include "memory.h"

Memory::HolderPack Memory::StackFrame::Lookup(std::string_view key) {
    try {
        return search(key);
    } catch (const Errors::MemoryErrors::NotFound&) {
        environment[std::string(key)] = std::make_shared<RawHolderPack>();
        return environment[std::string(key)];
    }
}

Memory::HolderPack Memory::StackFrame::search(std::string_view key) {
    std::cout << "смотрю!\n";
    std::cout << "вот пример " << (environment.find("print") != environment.end()) << '\n';
    if (auto iter = environment.find(std::string(key)); iter != environment.end()) {
        return iter->second;
    }
    if (parent != nullptr) { return parent->search(key); }
    throw Errors::MemoryErrors::NotFound();
}
