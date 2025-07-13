#pragma once
#include <cctype>
#include <iostream>
#include <string>
#include <unordered_map>
#include "memory.h"

extern std::unordered_map<std::string, Memory::HolderPack> BUILT_IN_FUNCTIONS;

namespace BuiltIn {

using HolderPack = Memory::HolderPack;
using BuiltInFunction = Memory::BuiltInFunction;
using Memory::MakeFuncHolder;
using Memory::MakeListHolder;

using ListHolderPtr = Memory::ListHolderPtr;
using FuncHolderPtr = Memory::FuncHolderPtr;

void InitializeBuilInFunctions();

// MATH FUNCTIONS
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
extern HolderPack copy;

// UNIVERSAL FUNCTIONS
extern HolderPack len;

// SYSTEM FUNCTIONS
extern HolderPack print;
extern HolderPack println;
extern HolderPack read;
extern HolderPack stacktrace;
extern HolderPack exit;

} // end BuiltIn

// forward declaration for work in/out stream system function of built in functions.
extern std::istream* in;
extern std::ostream* out;

