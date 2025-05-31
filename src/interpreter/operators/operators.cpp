#include "operators.h"

namespace Operators {
    std::unordered_map<UnaryOpTableKey, UnaryFunction> UNARY_OP_TABLE;
    std::unordered_map<BinaryOpTableKey, BinaryFunction> BINARY_OP_TABLE;

    /// Unary operations
    void RegisterUnaryNumOperators() noexcept {
        // + NUM
        UNARY_OP_TABLE[{Lexer::Tokens::T_PLUS, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg) -> HolderPack {
                return { 
                    std::make_shared<HolderTypes>(std::get<double>(*(arg.holder))),
                    TYPES::NUM_TYPE
                };
            }
        };

        // - NUM
        UNARY_OP_TABLE[{Lexer::Tokens::T_MINUS, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg) -> HolderPack {
                return {
                    std::make_shared<HolderTypes>(-std::get<double>(*(arg.holder))),
                    TYPES::NUM_TYPE
                };
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

    [[nodiscard]] Expected ExecUnaryOperation(Parser::UnaryOp* node, HolderPack&& computed) {
        UnaryOpTableKey key = {node->op, computed.type};
        auto iter = UNARY_OP_TABLE.find(std::move(key));
        if (iter == UNARY_OP_TABLE.end()) {
            return std::unexpected(Lexer::Token{
                Errors::OperatorErrors::OperatorUnaryError(Lexer::TOKENS_TO_STR[node->op], TYPE_TO_STR[computed.type]), 
                node->token});
        }
        try {
            return iter->second(std::move(computed));
        } catch (...) {
            return std::unexpected(Lexer::Token{InternalError(), node->token});
        }
    }

    [[nodiscard]] Expected ExecBinaryOperation(Parser::BinaryOp* node, HolderPack&& computed_left, HolderPack&& computed_right) {
        BinaryOpTableKey key = {node->op, node->left->type, node->right->type};
        auto iter = BINARY_OP_TABLE.find(std::move(key));
        if (iter == BINARY_OP_TABLE.end()) {
            return std::unexpected(Lexer::Token{
                Errors::OperatorErrors::OperatorBinaryError(Lexer::TOKENS_TO_STR[node->op], TYPE_TO_STR[node->left->type], TYPE_TO_STR[node->right->type]),
                node->token});
        }
        try {
            return iter->second(std::move(computed_left), std::move(computed_right));
        } catch (...) {
            return std::unexpected(Lexer::Token{InternalError(), node->token});
        }
    }
}