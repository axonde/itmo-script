#include "built_in.h"

std::unordered_map<std::string, Memory::Holder> BUILT_IN_FUNCTIONS = {
    // NUMBER AWARE FUNCTIONS
    {"abs", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::abs)},
    {"ceil", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::ceil)},
    {"floor", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::floor)},
    {"round", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::round)},
    {"sqrt", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::sqrt)},
    {"rnd", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::rnd)},
    {"parse_num", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::parse_num)},
    {"to_string", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::to_string)},

    // STRING AWARE FUNCTIONS
    {"len", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::len)},
    {"lower", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::lower)},
    {"upper", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::upper)},
    {"split", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::split)},
    {"join", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::join)},
    {"replace", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::replace)},

    // LIST AWARE FUNCTIONS
    {"range", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::range)},
    {"len", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::len)},
    {"push", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::push)},
    {"pop", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::pop)},
    {"insert", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::insert)},
    {"remove", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::remove)},
    {"sort", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::sort)},

    // UNIVERSAL FUNCTIONS
    {"copy", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::copy)},

    // SYSTEM FUNCTIONS
    {"print", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::print)},
    {"println", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::println)},
    {"read", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::read)},
    {"stacktrace", std::make_shared<Memory::BuiltInFuncHolder>(&BuiltIn::stacktrace)}
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