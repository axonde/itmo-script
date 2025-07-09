#include "memory.h"
#include "parser.h"

std::unordered_map<TYPES, std::string> TYPE_TO_STR = {
    {TYPES::NUM_TYPE, "number"},
    {TYPES::STRING_TYPE, "string"},
    {TYPES::NIL_TYPE, "nil"},
    {TYPES::BOOL_TYPE, "bool"},
    {TYPES::LIST_TYPE, "list"},
    {TYPES::FUNC_TYPE, "func"},
    {TYPES::NOT_SET_TYPE, "(not set type)"}
};

namespace Memory {

struct NodeHolder::Impl {
    Impl(void* n) {
        Parser::Node* source = static_cast<Parser::Node*>(n);

        node = std::make_unique<Parser::Func>(
            std::move(*static_cast<Parser::Func*>(source))
        );
    }
    Parser::NodePtr node;
};
NodeHolder::NodeHolder(void* node) {
    pimpl = std::make_unique<NodeHolder::Impl>(node);
}
NodeHolder::NodeHolder(NodeHolder&&) noexcept = default;
NodeHolder& NodeHolder::operator=(NodeHolder&&) noexcept = default;
NodeHolder::~NodeHolder() = default;
void* NodeHolder::get() {
    return static_cast<void*>(pimpl->node.get());
}

StackFrame* stack_frame;

bool HolderPack::operator<(const HolderPack& other) const {
    return std::get<bool>(
        Operators::RawExecBinaryOperation(Lexer::T_COMP_SMALLER, HolderPack(*this), HolderPack(other))->holder
    );
}

HolderPack StackFrame::Lookup(std::string_view key) {
    try {
        return search(key);
    } catch (const Errors::MemoryErrors::NotFound&) {
        environment[std::string(key)] = HolderPack();
        return environment[std::string(key)];
    }
}

HolderPack StackFrame::search(std::string_view key) {
    if (auto iter = environment.find(std::string(key)); iter != environment.end()) {
        return iter->second;
    }
    if (parent != nullptr) { return parent->search(key); }
    throw Errors::MemoryErrors::NotFound();
}

} // end Memory
