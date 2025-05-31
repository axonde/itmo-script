#pragma once
#include <functional>
#include <memory>
#include <string_view>
#include <optional>
#include <variant>
#include <vector>

#include "parser.h"

namespace Memory {

/// HOLDER
/// - std::monostate    > nil type
/// - double            > number type
/// - bool              > bool type
/// - std::string       > string type
/// - ListHolder        > list type
/// - FuncHolder        > func type

class StackFrame {
public:
    StackFrame(const std::unordered_map<std::string_view, HolderPack>& e) : environment(e) {}
    StackFrame(std::unique_ptr<StackFrame>&& ptr) : parent(std::move(ptr)) {}

    HolderPack Lookup(std::string_view);  // throw an error otherwise

    HolderPack AddNil();
    HolderPack AddBool(bool);
    HolderPack AddNumber(double);
    HolderPack AddList();
    HolderPack AddFunc();

private:
    std::unique_ptr<StackFrame> parent = nullptr;
    std::unordered_map<std::string_view, HolderPack> environment;
};

} // namespace Memory