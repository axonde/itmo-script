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

using RawHolderPack = Memory::RawHolderPack;
using BuiltInFunction = Memory::BuiltInFunction;

using ListHolder = Memory::ListHolder;
using FuncHolder = Memory::FuncHolder;

using ListHolderPtr = Memory::ListHolderPtr;
using FuncHolderPtr = Memory::FuncHolderPtr;

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
HolderPack len = Memory::MakeHolderPack(
    std::make_unique<FuncHolder>(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() != 1) { throw Errors::RunTime::ExpectedOneArg(); }
            if (params[0]->type == TYPES::STRING_TYPE) {
                return Memory::MakeHolderPack(
                    static_cast<double>(std::get<std::string>(params[0]->holder).size()),
                    TYPES::NUM_TYPE);
            }
            if (params[0]->type == TYPES::LIST_TYPE) {
                return Memory::MakeHolderPack(
                    static_cast<double>(std::get<Memory::ListHolderPtr>(params[0]->holder)->data.size()),
                    TYPES::NUM_TYPE);
            }
            throw Errors::TypeErrors::TypeErrorStringOrList();
        }
    )),
    TYPES::FUNC_TYPE
);

// SYSTEM FUNCTIONS
HolderPack print = Memory::MakeHolderPack(
    std::make_unique<FuncHolder>(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            bool need_space = false;
            for (auto& holderpack : params) {
                if (need_space) { *Interpreter::out << ' '; }
                switch (holderpack->type) {
                    case TYPES::NUM_TYPE:
                        *Interpreter::out << std::get<double>(holderpack->holder); break;
                    case TYPES::STRING_TYPE:
                        *Interpreter::out << '"' << std::get<std::string>(holderpack->holder) << '"'; break;
                    case TYPES::BOOL_TYPE:
                        if (std::get<bool>(holderpack->holder)) { *Interpreter::out << "true"; }
                        else { *Interpreter::out << "false"; }
                        break;
                    case TYPES::NIL_TYPE:
                        *Interpreter::out << "nil"; break;
                    case TYPES::FUNC_TYPE:
                        *Interpreter::out << "<function>"; break;
                    case TYPES::LIST_TYPE:
                    { *Interpreter::out << '['; bool not_first = false;
                        for (auto hp : std::get<ListHolderPtr>(holderpack->holder)->data) {
                            if (not_first) { *Interpreter::out << ", "; } not_first = true;
                            std::get<BuiltInFunction>(
                                std::get<FuncHolderPtr>(print->holder)->function
                            )({hp});
                        } *Interpreter::out << ']'; }
                        break;
                    case TYPES::NOT_SET_TYPE:
                        *Interpreter::out << "(not set type)"; break;
                } need_space = true;
            }
            return std::make_shared<RawHolderPack>(TYPES::NIL_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);
HolderPack println = std::make_shared<RawHolderPack>(
    std::make_unique<FuncHolder>(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            for (auto& holderpack : params) {
                std::get<BuiltInFunction>(
                    std::get<FuncHolderPtr>(print->holder)->function
                )({std::move(holderpack)});
                *Interpreter::out << std::endl;
            }
            return std::make_shared<RawHolderPack>(TYPES::NIL_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);
HolderPack read = std::make_shared<RawHolderPack>(
    std::make_unique<FuncHolder>(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            if (params.size() > 0) { throw Errors::RunTime::ExpectedZeroArgs(); }
            std::string str;
            std::getline(*Interpreter::in, str);
            return std::make_shared<RawHolderPack>(
                std::move(str), TYPES::STRING_TYPE
            );
        })
    ),
    TYPES::FUNC_TYPE
);
HolderPack stacktrace = std::make_shared<RawHolderPack>(
    std::make_unique<FuncHolder>(BuiltInFunction(
        [](std::vector<HolderPack>&& params) -> HolderPack {
            *Interpreter::out << Memory::StackFrame::PrintStack(*Interpreter::stack_frame);
            return std::make_shared<RawHolderPack>(TYPES::NIL_TYPE);
        })
    ),
    TYPES::FUNC_TYPE
);

}

std::unordered_map<std::string, Memory::HolderPack> BUILT_IN_FUNCTIONS;

void BuiltIn::InitializeBuilInFunctions() {
    BUILT_IN_FUNCTIONS = {
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
}