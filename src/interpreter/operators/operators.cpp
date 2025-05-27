#include "operators.h"

namespace Operators {
    std::unordered_map<UnaryOpTableKey, UnaryOpTableValue> UNARY_OP_TABLE;
    std::unordered_map<BinaryOpTableKey, BinaryOpTableValue> BINARY_OP_TABLE;

    std::unordered_map<Lexer::Tokens, std::string> OP_TO_STR {
        {Lexer::Tokens::T_PLUS, "+"},
        {Lexer::Tokens::T_MINUS, "-"},
        {Lexer::Tokens::T_DIV, "/"},
        {Lexer::Tokens::T_MULT, "*"},
        {Lexer::Tokens::T_MOD, "%"},
        {Lexer::Tokens::T_XOR, "^"}
    };
    std::unordered_map<Parser::Types, std::string> TYPE_TO_STR {
        {Parser::Types::NUM_TYPE, "number"},
        {Parser::Types::STRING_TYPE, "string"},
        {Parser::Types::NIL_TYPE, "nil"},
        {Parser::Types::BOOL_TYPE, "bool"},
        {Parser::Types::LIST_TYPE, "list"},
        {Parser::Types::FUNC_TYPE, "func"},
        {Parser::Types::NOT_SET_TYPE, "(not set type)"}
    };

    /// Unary operations
    void RegisterUnaryNumOperators() noexcept {
        UNARY_OP_TABLE[{Lexer::Tokens::T_PLUS, Parser::Types::NUM_TYPE}] = {
            Parser::Types::NUM_TYPE,
            [](Value&& value) -> Value { 
                return std::get<double>(value); }
        };

        UNARY_OP_TABLE[{Lexer::Tokens::T_MINUS, Parser::Types::NUM_TYPE}] = {
            Parser::Types::NUM_TYPE,
            [](Value&& value) -> Value {
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

    [[nodiscard]] Expected ExecUnaryOperation(Parser::UnaryOp* node, Value&& computed) {
        UnaryOpTableKey key = {node->op, node->child->type};
        auto iter = UNARY_OP_TABLE.find(std::move(key));
        if (iter == UNARY_OP_TABLE.end()) {
            return std::unexpected(Lexer::Token{
                Errors::OperatorErrors::OperatorUnaryError(OP_TO_STR[node->op], TYPE_TO_STR[node->child->type]), 
                node->token});
        }
        try {
            node->type = iter->second.type;
            Value result = iter->second.func(std::move(computed));
            return result;
        } catch (const std::bad_variant_access&) {
            return std::unexpected(Lexer::Token{Errors::InternalErrors::InternalOperationError(), node->token});
        }
    }

    [[nodiscard]] Expected ExecBinaryOperation(Parser::BinaryOp* node, Value&& computed_left, Value&& computed_right) {
        BinaryOpTableKey key = {node->op, node->left->type, node->right->type};
        auto iter = BINARY_OP_TABLE.find(std::move(key));
        if (iter == BINARY_OP_TABLE.end()) {
            return std::unexpected(Lexer::Token{
                Errors::OperatorErrors::OperatorBinaryError(OP_TO_STR[node->op], TYPE_TO_STR[node->left->type], TYPE_TO_STR[node->right->type]),
                node->token});
        }

        try {
            node->type = iter->second.type;
            Value result = iter->second.func(std::move(computed_left), std::move(computed_right));
            return result;
        } catch (const std::bad_variant_access&) {
            return std::unexpected(Lexer::Token{Errors::InternalErrors::InternalOperationError(), node->token});
        }
    }
}