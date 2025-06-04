#pragma once
#include <any>
#include <functional>
#include <span>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <iostream>

#include "memory.h"

extern std::unordered_map<std::string, Memory::HolderPack> BUILT_IN_FUNCTIONS;
extern std::unordered_map<TYPES, std::string> TYPE_TO_STR;

namespace BuiltIn {

using HolderPack = Memory::HolderPack;
using BuiltInFunction = Memory::BuiltInFunction;
using Memory::MakeFuncHolder;

using ListHolderPtr = Memory::ListHolderPtr;
using FuncHolderPtr = Memory::FuncHolderPtr;

void InitializeBuilInFunctions();

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

namespace Interpreter {
    extern std::unique_ptr<Memory::StackFrame> stack_frame;
    extern std::istream* in;
    extern std::ostream* out;
} // Interpreter