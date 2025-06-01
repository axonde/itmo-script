#include "built_in.h"

std::unordered_map<std::string_view, Memory::HolderPack> BUILT_IN_FUNCTIONS = {
    // NUMBER AWARE FUNCTIONS
    {"abs", BuiltIn::abs},
    {"ceil", BuiltIn::ceil},
    {"floor", BuiltIn::floor},
    {"round", BuiltIn::round},
    {"sqrt", BuiltIn::sqrt},
    {"rnd", BuiltIn::rnd},
    {"parse_num", BuiltIn::parse_num},
    {"to_string", BuiltIn::to_string},

    // STRING AWARE FUNCTIONS
    {"len", BuiltIn::len},
    {"lower", BuiltIn::lower},
    {"upper", BuiltIn::upper},
    {"split", BuiltIn::split},
    {"join", BuiltIn::join},
    {"replace", BuiltIn::replace},

    // LIST AWARE FUNCTIONS
    {"range", BuiltIn::range},
    {"len", BuiltIn::len},
    {"push", BuiltIn::push},
    {"pop", BuiltIn::pop},
    {"insert", BuiltIn::insert},
    {"remove", BuiltIn::remove},
    {"sort", BuiltIn::sort},

    // UNIVERSAL FUNCTIONS
    {"copy", BuiltIn::copy},

    // SYSTEM FUNCTIONS
    {"print", BuiltIn::print},
    {"println", BuiltIn::println},
    {"read", BuiltIn::read},
    {"stacktrace", BuiltIn::stacktrace}
};

std::unordered_map<TYPES, std::string> TYPE_TO_STR = {
    {TYPES::NUM_TYPE, "number"},
    {TYPES::STRING_TYPE, "string"},
    {TYPES::NIL_TYPE, "nil"},
    {TYPES::BOOL_TYPE, "bool"},
    {TYPES::LIST_TYPE, "list"},
    {TYPES::FUNC_TYPE, "func"},
    {TYPES::NOT_SET_TYPE, "(not set type)"}
};


namespace BuiltIn {

// NUMBER AWARE FUNCTIONS
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

}