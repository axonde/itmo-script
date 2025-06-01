#include "built_in.h"

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
using HolderTypes = Memory::HolderTypes;
using FunctionType = std::function<HolderPack(std::vector<HolderPack>&&)>;

HolderPack print = {
    std::make_shared<HolderTypes>(Memory::FuncHolder(FunctionType([](std::vector<HolderPack>&& params) -> HolderPack {
        for (auto& holderpack : params) {
            switch (holderpack.type) {
                case TYPES::NUM_TYPE:
                    std::cout << std::get<double>(*holderpack.holder); break;
                case TYPES::STRING_TYPE:
                    std::cout << '"' << std::get<std::string>(*holderpack.holder) << '"'; break;
                case TYPES::BOOL_TYPE:
                    if (std::get<bool>(*holderpack.holder)) { std::cout << "true"; }
                    else { std::cout << "false"; }
                    break;
                case TYPES::NIL_TYPE:
                    std::cout << "nil"; break;
                case TYPES::FUNC_TYPE:
                    std::cout << "<function>"; break;
                case TYPES::LIST_TYPE:
                  { std::cout << '['; bool not_first = false;
                    for (auto hp : std::get<Memory::ListHolder>(*holderpack.holder).data) {
                        if (not_first) { std::cout << ", "; } not_first = true;
                        std::get<FunctionType>(
                            std::get<Memory::FuncHolder>(*(print.holder)).function
                        )({std::move(hp)});
                    } std::cout << ']'; }
                    break;
                case TYPES::NOT_SET_TYPE:
                    std::cout << "(not set type)"; break;
            }
        }
        return {std::shared_ptr<HolderTypes>(), TYPES::NIL_TYPE};
    }))),
    TYPES::FUNC_TYPE
};
HolderPack println;
HolderPack read;
HolderPack stacktrace;

}

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