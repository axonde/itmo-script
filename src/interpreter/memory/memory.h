#pragma once
#include <functional>
#include <memory>
#include <span>
#include <string_view>
#include <optional>
#include <variant>

#include "parser.h"

namespace Memory {

/// HOLDER
/// - std::monostate    > nil type
/// - double            > number type
/// - bool              > bool type
/// - StringHolder      > string type
/// - ListHolder        > list type
/// - FuncHolder        > func type

struct StringHolder {
    StringHolder(std::string&& v) : value(std::move(v)) {}
    std::string value;
};
struct ListHolder {
    template<typename T>
    ListHolder(T&& container) : data(std::forward<T>(container)) {}
    std::span<Holder> data;
};

class StackFrame {
public:
    StackFrame(const std::unordered_map<std::string_view, HolderPack>& e) : environment(e) {}
    StackFrame(std::unique_ptr<StackFrame>&& ptr) : parent(std::move(ptr)) {}

    HolderPack& Lookup(std::string_view);  // throw an error otherwise

    HolderPack& AddNil();
    HolderPack& AddNumber(double);
    HolderPack& AddBool(bool);
    HolderPack& AddList();
    HolderPack& AddFunc();

private:
    std::unique_ptr<StackFrame> parent = nullptr;
    std::unordered_map<std::string_view, HolderPack> environment;
};

} // namespace Memory