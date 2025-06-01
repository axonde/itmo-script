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

struct ListHolder;
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
    ListHolder(std::vector<HolderPack>&& d) : data(std::move(d)) {}
    std::vector<HolderPack> data;
};
struct FuncHolder {
    Function function;
};

} // end Memory namespace

extern std::unordered_map<std::string_view, Memory::HolderPack> BUILT_IN_FUNCTIONS;
extern std::unordered_map<TYPES, std::string> TYPE_TO_STR;

namespace BuiltIn {

using HolderPack = Memory::HolderPack;
using Function = Memory::Function;

extern HolderPack abs;
extern HolderPack ceil;
extern HolderPack floor;
extern HolderPack round;
extern HolderPack sqrt;
extern HolderPack rnd;
extern HolderPack parse_num;
extern HolderPack to_string;

// STRING AWARE FUNCTIONS
extern HolderPack lower;
extern HolderPack upper;
extern HolderPack split;
extern HolderPack join;
extern HolderPack replace;

// LIST AWARE FUNCTIONS
extern HolderPack range;
extern HolderPack push;
extern HolderPack pop;
extern HolderPack insert;
extern HolderPack remove;
extern HolderPack sort;

// UNIVERSAL FUNCTIONS
extern HolderPack copy;
extern HolderPack len;

// SYSTEM FUNCTIONS
extern HolderPack print;
extern HolderPack println;
extern HolderPack read;
extern HolderPack stacktrace;

} // BuiltIn