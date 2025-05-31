#pragma once
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

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

struct ListHolder;      // defined in memory.h
struct FuncHolder;

using HolderTypes = std::variant<
    std::monostate,     // nil type
    double,             // num type
    bool,               // bool type
    std::string,        // string type
    ListHolder,         // list type
    FuncHolder          // functions type
>;

using Holder = std::shared_ptr<HolderTypes>;

struct HolderPack {
    Holder holder;
    TYPES type;
};

using Function = std::function<HolderPack(HolderPack&&)>;

struct ListHolder {
    template<typename T>
    ListHolder(T&& container) : data(std::forward<T>(container)) {}
    std::vector<Holder> data;
};
struct FuncHolder {
    FuncHolder(Function f) : function(std::move(f)) {}
    Function function;
};

} // end Memory namespace

extern std::unordered_map<std::string_view, Memory::HolderPack> BUILT_IN_FUNCTIONS;
extern std::unordered_map<TYPES, std::string> TYPE_TO_STR;

namespace BuiltIn {

using HolderPack = Memory::HolderPack;
using Function = Memory::Function;

HolderPack abs;
HolderPack ceil;
HolderPack floor;
HolderPack round;
HolderPack sqrt;
HolderPack rnd;
HolderPack parse_num;
HolderPack to_string;

// STRING AWARE FUNCTIONS
HolderPack lower;
HolderPack upper;
HolderPack split;
HolderPack join;
HolderPack replace;

// LIST AWARE FUNCTIONS
HolderPack range;
HolderPack push;
HolderPack pop;
HolderPack insert;
HolderPack remove;
HolderPack sort;

// UNIVERSAL FUNCTIONS
HolderPack copy;
HolderPack len;

// SYSTEM FUNCTIONS
HolderPack print;
HolderPack println;
HolderPack read;
HolderPack stacktrace;

} // BuiltIn