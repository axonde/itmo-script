#include "operators.h"

namespace Operators {
    std::unordered_map<UnaryOpTableKey, UnaryOpTableValue> UNARY_OP_TABLE;
    std::unordered_map<BinaryOpTableKey, BinaryOpTableValue> BINARY_OP_TABLE;

    /// Unary operations
    void RegisterUnaryNumOperators() noexcept {
        // + NUM
        UNARY_OP_TABLE[{Lexer::Tokens::T_PLUS, TYPES::NUM_TYPE}] = {
            TYPES::NUM_TYPE,
            [](Holder&& value) -> Holder { 
                return std::get<double>(value); }
        };

        // - NUM
        UNARY_OP_TABLE[{Lexer::Tokens::T_MINUS, TYPES::NUM_TYPE}] = {
            TYPES::NUM_TYPE,
            [](Holder&& value) -> Holder {
                return -std::get<double>(value);
            }
        };
    }

    void RegisterUnaryStringOperators() noexcept {}
    void RegisterUnaryBoolOperators() noexcept {}
    void RegisterUnaryNilOperators() noexcept {}

    void RegisterUnaryOperators() noexcept {
        RegisterUnaryNumOperators();
        RegisterUnaryStringOperators();
        RegisterUnaryBoolOperators();
        RegisterUnaryNilOperators();
    }

    /// Binary operations
    void RegisterBinaryNumOperators() noexcept {}
    void RegisterBinaryStringOperators() noexcept {}
    void RegisterBinaryBoolOperators() noexcept {}
    void RegisterBinaryNilOperators() noexcept {}

    void RegisterBinaryOperators() noexcept {
        RegisterBinaryNumOperators();
        RegisterBinaryStringOperators();
        RegisterBinaryBoolOperators();
        RegisterBinaryNilOperators();
    }

    [[nodiscard]] Expected ExecUnaryOperation(Parser::UnaryOp* node, Holder&& computed) {
        UnaryOpTableKey key = {node->op, node->operand->type};
        auto iter = UNARY_OP_TABLE.find(std::move(key));
        if (iter == UNARY_OP_TABLE.end()) {
            return std::unexpected(Lexer::Token{
                Errors::OperatorErrors::OperatorUnaryError(Lexer::TOKENS_TO_STR[node->op], TYPE_TO_STR[node->operand->type]), 
                node->token});
        }
        try {
            node->type = iter->second.type;
            Holder result = iter->second.func(std::move(computed));
            return result;
        } catch (...) {
            return std::unexpected(Lexer::Token{InternalError(), node->token});
        }
    }

    [[nodiscard]] Expected ExecBinaryOperation(Parser::BinaryOp* node, Holder&& computed_left, Holder&& computed_right) {
        BinaryOpTableKey key = {node->op, node->left->type, node->right->type};
        auto iter = BINARY_OP_TABLE.find(std::move(key));
        if (iter == BINARY_OP_TABLE.end()) {
            return std::unexpected(Lexer::Token{
                Errors::OperatorErrors::OperatorBinaryError(Lexer::TOKENS_TO_STR[node->op], TYPE_TO_STR[node->left->type], TYPE_TO_STR[node->right->type]),
                node->token});
        }
        try {
            node->type = iter->second.type;
            Holder result = iter->second.func(std::move(computed_left), std::move(computed_right));
            return result;
        } catch (...) {
            return std::unexpected(Lexer::Token{InternalError(), node->token});
        }
    }
}