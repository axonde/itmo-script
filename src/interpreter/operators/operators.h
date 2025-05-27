#pragma once
#include <expected>
#include <functional>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "parser.h"

namespace Operators {
    using Value = std::variant<std::monostate, double, std::string, bool>;
    using Expected = std::expected<Value, Lexer::Token>;

    extern std::unordered_map<Lexer::Tokens, std::string> OP_TO_STR;
    extern std::unordered_map<Parser::Types, std::string> TYPE_TO_STR;

    struct UnaryOpTableKey {
        Lexer::Tokens op;
        Parser::Types arg_type;
        bool operator==(const UnaryOpTableKey& other) const = default;
    };
    using UnaryOpTableValue = std::function<Value(Value&&)>;

    struct BinaryOpTableKey {
        Lexer::Tokens op;
        Parser::Types left_arg_type;
        Parser::Types right_arg_type;
        bool operator==(const BinaryOpTableKey& other) const = default;
    };
    using BinaryOpTableValue = std::function<Value(Value&&, Value&&)>;

    extern std::unordered_map<UnaryOpTableKey, UnaryOpTableValue> UNARY_OP_TABLE;
    extern std::unordered_map<BinaryOpTableKey, BinaryOpTableValue> BINARY_OP_TABLE;

    void RegisterUnaryNumOperators();
    void RegisterUnaryStringOperators();
    void RegisterUnaryBoolOperators();
    void RegisterUnaryNilOperators();
    void RegisterUnaryOperators();

    void RegisterBinaryNumOperators();
    void RegisterBinaryStringOperators();
    void RegisterBinaryBoolOperators();
    void RegisterBinaryNilOperators();

    void RegisterBinaryOperators();

    Expected ExecUnaryOperation(Parser::UnaryOp* node, Value&& computed);
    Expected ExecBinaryOperation(Parser::BinaryOp* node, Value&& computed_left, Value&& computed_right);
}

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
