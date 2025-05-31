#pragma once
#include <functional>
#include <memory>
#include <span>
#include <optional>
#include <variant>

#include "parser.h"

namespace Memory {

struct StringHolder {
    StringHolder(std::string&& str) : value(std::move(str)) {}
    std::string value;
};
struct ListHolder {
    template<typename T>
    ListHolder(T&& container) : data(std::forward<T>(container)) {}
    std::span<Holder> data;
};
struct BuiltInFuncHolder;  // declarations in built-in
struct UserSetFuncHolder {
    Parser::Node* node;
};

class StackFrame {
public:
    StackFrame(const std::unordered_map<std::string, Holder>& e) : environment(e) {}
    StackFrame(std::unique_ptr<StackFrame>&& ptr) : parent(std::move(ptr)) {}

    Holder& Lookup(const std::string&);

    Holder& AddNil();
    Holder& AddNumber(double);
    Holder& AddBool(bool);
    Holder& AddList();
    Holder& AddFunc();

private:
    std::unique_ptr<StackFrame> parent = nullptr;
    std::unordered_map<std::string, Holder> environment;
};

} // namespace Memory