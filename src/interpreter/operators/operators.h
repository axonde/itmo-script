#pragma once
#include <expected>
#include <functional>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "parser.h"

namespace Operators {

using RawHolderPack = Memory::RawHolderPack;
using Holder = Memory::Holder;
using HolderPack = Memory::HolderPack;
using UnaryFunction = std::function<HolderPack(HolderPack&&)>;
using BinaryFunction = std::function<HolderPack(HolderPack&&, HolderPack&&)>;
using Expected = std::expected<HolderPack, Lexer::Token>;

struct UnaryOpTableKey {
    Lexer::Tokens op;
    TYPES arg_type;
    bool operator==(const UnaryOpTableKey& other) const = default;
};

struct BinaryOpTableKey {
    Lexer::Tokens op;
    TYPES left_arg_type;
    TYPES right_arg_type;
    bool operator==(const BinaryOpTableKey& other) const = default;
};

extern std::unordered_map<UnaryOpTableKey, UnaryFunction> UNARY_OP_TABLE;
extern std::unordered_map<BinaryOpTableKey, BinaryFunction> BINARY_OP_TABLE;

void RegisterUnaryNumOperators() noexcept;
void RegisterUnaryStringOperators() noexcept;
void RegisterUnaryBoolOperators() noexcept;
void RegisterUnaryNilOperators() noexcept;
void RegisterUnaryOperators() noexcept;

void RegisterBinaryNumOperators() noexcept;
void RegisterBinaryBoolOperators() noexcept;
void RegisterBinaryNilOperators() noexcept;
void RegisterBinaryStringOperators() noexcept;
void RegisterBinaryListOperators() noexcept;
void RegisterBinaryFuncOperators() noexcept;

void RegisterBinaryOperators() noexcept;

[[nodiscard]] Expected ExecUnaryOperation(Parser::UnaryOp*, HolderPack&&);
[[nodiscard]] Expected ExecBinaryOperation(Parser::BinaryOp*, HolderPack&&, HolderPack&&);
[[nodiscard]] Expected ExecBinaryOperation(Lexer::Tokens, Parser::NodePtr&, HolderPack&&, HolderPack&&);

} // end Operators

namespace std {
    template<>
    struct hash<Operators::UnaryOpTableKey> {
        size_t operator()(const Operators::UnaryOpTableKey& key) const {
            return hash<int>()(static_cast<int>(key.op)) ^
                   hash<int>()(static_cast<int>(key.arg_type));
        }
    };

    template<>
    struct hash<Operators::BinaryOpTableKey> {
        size_t operator()(const Operators::BinaryOpTableKey& key) const {
            return hash<int>()(static_cast<int>(key.op)) ^
                   hash<int>()(static_cast<int>(key.left_arg_type)) ^
                   hash<int>()(static_cast<int>(key.right_arg_type));
        }
    };
}
