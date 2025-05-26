#include "operators.h"

namespace Operators {
    std::unordered_map<UnaryOpTableKey, UnaryOpTableValue> UNARY_OP_TABLE;
    std::unordered_map<BinaryOpTableKey, BinaryOpTableValue> BINARY_OP_TABLE;

    /// Unary operations
    void RegisterUnaryNumOperators() {
        UNARY_OP_TABLE[{Lexer::Tokens::T_PLUS, Parser::Types::NUM_TYPE}] = {
            [](Value&& value) -> Value { 
                return std::get<double>(value); }
        };

        UNARY_OP_TABLE[{Lexer::Tokens::T_MINUS, Parser::Types::NUM_TYPE}] = {
            [](Value&& value) -> Value {
                return -std::get<double>(value);
            }
        };
    }

    void RegisterUnaryStringOperators() {}
    void RegisterUnaryBoolOperators() {}
    void RegisterUnaryNilOperators() {}

    void RegisterUnaryOperators() {
        RegisterUnaryNumOperators();
        RegisterUnaryStringOperators();
        RegisterUnaryBoolOperators();
        RegisterUnaryNilOperators();
    }

    /// Binary operations
    void RegisterBinaryNumOperators() {}
    void RegisterBinaryStringOperators() {}
    void RegisterBinaryBoolOperators() {}
    void RegisterBinaryNilOperators() {}

    void RegisterBinaryOperators() {
        RegisterBinaryNumOperators();
        RegisterBinaryStringOperators();
        RegisterBinaryBoolOperators();
        RegisterBinaryNilOperators();
    }

    Expected ExecUnaryOperation(Parser::UnaryOp* node, Value&& computed) {
        UnaryOpTableKey key = {node->op, node->child->type};
        auto iter = UNARY_OP_TABLE.find(std::move(key));
        if (iter == UNARY_OP_TABLE.end()) return std::unexpected(node->token);

        try {
            Value result = iter->second(std::move(computed));
            return result;
        } catch (const std::bad_variant_access&) {
            return std::unexpected(node->token);
        }
    }

    Expected ExecBinaryOperation(Parser::BinaryOp* node, Value&& computed_left, Value&& computed_right) {
        BinaryOpTableKey key = {node->op, node->left->type, node->right->type};
        auto iter = BINARY_OP_TABLE.find(std::move(key));
        if (iter == BINARY_OP_TABLE.end()) return std::unexpected(node->token);

        try {
            Value result = iter->second(std::move(computed_left), std::move(computed_right));
            return result;
        } catch (const std::bad_variant_access&) {
            return std::unexpected(node->token);
        }
    }
}