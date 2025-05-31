#include "memory.h"

Memory::Holder& Memory::StackFrame::Lookup(const std::string& key) {
    if (auto iter = environment.find(key); iter != environment.end()) {
        return iter->second;
    }
    if (parent != nullptr) { return parent->Lookup(key); }
    throw Errors::MemoryErrors::NotFound{};
}
