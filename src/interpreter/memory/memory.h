#pragma once
#include <any>
#include <functional>
#include <memory>
#include <string_view>
#include <sstream>
#include <variant>
#include <vector>

#include "lexer.h"

enum TYPES : uint16_t {
    NUM_TYPE,
    STRING_TYPE,
    NIL_TYPE,
    BOOL_TYPE,
    LIST_TYPE,
    FUNC_TYPE,
    NOT_SET_TYPE
};
extern std::unordered_map<TYPES, std::string> TYPE_TO_STR;

namespace Memory {

class StackFrame;

struct ListHolder;
struct FuncHolder;

using ListHolderPtr = std::unique_ptr<ListHolder>;
using FuncHolderPtr = std::unique_ptr<FuncHolder>;

/// Holder -> HolderData -> RawHolderPack -> HolderPack

/// HOLDER
/// - std::monostate    > nil type / (not set type)
/// - double            > number type
/// - bool              > bool type
/// - std::string       > string type
/// - ListHolder        > list type
/// - FuncHolder        > func type

using Holder = std::variant<
    std::monostate,
    double,
    bool,
    std::string,
    ListHolderPtr,
    FuncHolderPtr
>;

struct HolderData {
    HolderData() = default;
    HolderData(TYPES t) : type(t) {}
    HolderData(Holder&& h) : holder(std::move(h)) {}
    HolderData(Holder&& h, TYPES t) : holder(std::move(h)), type(t) {}
    Holder holder;
    TYPES type = TYPES::NOT_SET_TYPE;
};

class HolderPack;

} // end Memory

namespace Operators {

// need to forward declare to avoid compare between HolderPack
[[nodiscard]] Memory::HolderPack ExecBinaryOperation(Lexer::Tokens, Memory::HolderPack&&, Memory::HolderPack&&);

} // end Operators

namespace Memory {

using RawHolderPack = std::shared_ptr<HolderData>;
struct HolderPack {
    template<typename... Args>
    HolderPack(Args... args) {
        pack = std::make_shared<RawHolderPack>(std::make_shared<HolderData>(std::forward<Args>(args)...));
    }
    bool operator<(const HolderPack&) const;
    HolderData* operator->() { return (*pack).get(); }
    std::shared_ptr<RawHolderPack> pack;
};

using BuiltInFunction = std::function<HolderPack(std::vector<HolderPack>&&)>;
using Function = std::variant<
    BuiltInFunction,            // built in function
    std::any                    // user set function (Parser::Node*) TODO think how fix this problem by forwarding declarations ?
>;

struct ListHolder {
    ListHolder(std::vector<HolderPack>&& d) : data(std::move(d)) {}
    std::vector<HolderPack> data;
};
struct FuncHolder {
    FuncHolder(Function&& f) : function(std::move(f)) {}
    Function function;
};

template<typename... Args>
ListHolderPtr MakeListHolder(Args... args) {
    return std::make_unique<ListHolder>(std::forward<Args>(args)...);
}
template<typename... Args>
FuncHolderPtr MakeFuncHolder(Args... args) {
    return std::make_unique<FuncHolder>(std::forward<Args>(args)...);
}

class StackFrame {
public:
    StackFrame() = default;
    StackFrame(StackFrame&&) = default;
    StackFrame(std::unordered_map<std::string, HolderPack> e, std::string&& n) : environment(std::move(e)), name(std::move(n)) {}
    StackFrame(std::unique_ptr<StackFrame>&& ptr, std::string&& n) : parent(std::move(ptr)), name(std::move(n)) {}

    HolderPack Lookup(std::string_view);  // return a (not set type) if not found variable

    inline static std::string PrintStack(StackFrame& stack) {
        std::stringstream formatted;
        formatted << Patterns::STACK_TRACE_HEADER;
        size_t count = 0;
        formatted << "#" << count << ' ' << stack.name << '\n';
        StackFrame* ptr = stack.parent.get();
        while (ptr) {
            formatted << "#" << ++count << ' ' << ptr->name << '\n';
            ptr = ptr->parent.get();
        }
        return formatted.str();
    }
    std::unique_ptr<StackFrame> parent;
private:
    std::string name;
    std::unordered_map<std::string, HolderPack> environment;

    HolderPack search(std::string_view);
};

} // namespace Memory
