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
    struct UnaryOpTableValue {
        Parser::Types type;
        std::function<Value(Value&&)> func;
    };

    struct BinaryOpTableKey {
        Lexer::Tokens op;
        Parser::Types left_arg_type;
        Parser::Types right_arg_type;
        bool operator==(const BinaryOpTableKey& other) const = default;
    };
    struct BinaryOpTableValue {
        Parser::Types type;
        std::function<Value(Value&&, Value&&)> func;
    };

    extern std::unordered_map<UnaryOpTableKey, UnaryOpTableValue> UNARY_OP_TABLE;
    extern std::unordered_map<BinaryOpTableKey, BinaryOpTableValue> BINARY_OP_TABLE;

    void RegisterUnaryNumOperators() noexcept;
    void RegisterUnaryStringOperators() noexcept;
    void RegisterUnaryBoolOperators() noexcept;
    void RegisterUnaryNilOperators() noexcept;
    void RegisterUnaryOperators() noexcept;

    void RegisterBinaryNumOperators() noexcept;
    void RegisterBinaryStringOperators() noexcept;
    void RegisterBinaryBoolOperators() noexcept;
    void RegisterBinaryNilOperators() noexcept;

    void RegisterBinaryOperators() noexcept;

    [[nodiscard]] Expected ExecUnaryOperation(Parser::UnaryOp* node, Value&& computed);
    [[nodiscard]] Expected ExecBinaryOperation(Parser::BinaryOp* node, Value&& computed_left, Value&& computed_right);
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
