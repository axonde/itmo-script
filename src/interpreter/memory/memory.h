#pragma once
#include <any>
#include <functional>
#include <memory>
#include <string_view>
#include <sstream>
#include <optional>
#include <variant>
#include <vector>

#include "utils.h"

enum TYPES : uint16_t {
    NUM_TYPE,
    STRING_TYPE,
    NIL_TYPE,
    BOOL_TYPE,
    LIST_TYPE,
    FUNC_TYPE,
    NOT_SET_TYPE
};

namespace Memory {

class StackFrame;

struct ListHolder;
struct FuncHolder;

using ListHolderPtr = std::unique_ptr<ListHolder>;
using FuncHolderPtr = std::unique_ptr<FuncHolder>;

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

struct RawHolderPack {
    RawHolderPack() = default;
    RawHolderPack(TYPES t) : type(t) {}
    RawHolderPack(Holder&& h) : holder(std::move(h)) {}
    RawHolderPack(Holder&& h, TYPES t) : holder(std::move(h)), type(t) {}
    Holder holder;
    TYPES type = TYPES::NOT_SET_TYPE;
};

using HolderPack = std::shared_ptr<RawHolderPack>;

using BuiltInFunction = std::function<HolderPack(std::vector<HolderPack>&&)>;
using Function = std::variant<
    BuiltInFunction,            // built in function
    std::any                    // user set function (Parser::NodePtr)
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
HolderPack MakeHolderPack(Args... args) {
    return std::make_shared<RawHolderPack>(std::forward<Args>(args)...);
}
template<typename... Args>
ListHolderPtr MakeList(Args... args) {
    return std::make_unique<ListHolder>(std::forward<Args>(args)...);
}

class StackFrame {
public:
    StackFrame() = default;
    StackFrame(StackFrame&&) = default;
    StackFrame(std::unordered_map<std::string, HolderPack> e, std::string&& n) : environment(std::move(e)), name(std::move(n)) {}
    StackFrame(std::unique_ptr<StackFrame>&& ptr, std::string&& n) : parent(std::move(ptr)), name(std::move(n)) {}

    HolderPack Lookup(std::string_view);  // throw an error otherwise

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
private:
    std::string name;
    std::unique_ptr<StackFrame> parent;
    std::unordered_map<std::string, HolderPack> environment;

    HolderPack search(std::string_view);
};

} // namespace Memory