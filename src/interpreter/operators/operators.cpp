#include "operators.h"

namespace Operators {
    std::unordered_map<UnaryOpTableKey, UnaryFunction> UNARY_OP_TABLE;
    std::unordered_map<BinaryOpTableKey, BinaryFunction> BINARY_OP_TABLE;

    /// Unary operations
    void RegisterUnaryNumOperators() noexcept {
        // + NUM
        UNARY_OP_TABLE[{Lexer::Tokens::T_PLUS, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg) noexcept -> HolderPack {
                return Memory::MakeHolderPack(
                    std::get<double>(arg->holder),
                    TYPES::NUM_TYPE
                );
            }
        };
        // - NUM
        UNARY_OP_TABLE[{Lexer::Tokens::T_MINUS, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg) noexcept -> HolderPack {
                return Memory::MakeHolderPack(
                    -std::get<double>(arg->holder),
                    TYPES::NUM_TYPE
                );
            }
        };
    }

    void RegisterUnaryStringOperators() noexcept {
        // + STRING
        UNARY_OP_TABLE[{Lexer::Tokens::T_PLUS, TYPES::STRING_TYPE}] = {
            [](HolderPack&& arg) noexcept -> HolderPack {
                return { std::move(arg) };
            }
        };
    }
    void RegisterUnaryBoolOperators() noexcept {
        // not NUM
        UNARY_OP_TABLE[{Lexer::Tokens::T_NOT, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg) noexcept -> HolderPack {
                return Memory::MakeHolderPack(
                    !(std::get<double>(arg->holder) != 0),
                    TYPES::BOOL_TYPE
                );
            }
        };
        // not STRING
        UNARY_OP_TABLE[{Lexer::Tokens::T_NOT, TYPES::STRING_TYPE}] = {
            [](HolderPack&& arg) noexcept -> HolderPack {
                return Memory::MakeHolderPack(
                    std::get<std::string>(arg->holder).size() == 0,
                    TYPES::BOOL_TYPE
                );
            }
        };
        // not BOOL
        UNARY_OP_TABLE[{Lexer::Tokens::T_NOT, TYPES::BOOL_TYPE}] = {
            [](HolderPack&& arg) noexcept -> HolderPack {
                return Memory::MakeHolderPack(
                    !(std::get<bool>(arg->holder)), TYPES::BOOL_TYPE
                );
            }
        };
        // not NIL
        UNARY_OP_TABLE[{Lexer::Tokens::T_NOT, TYPES::NIL_TYPE}] = {
            [](HolderPack&& arg) noexcept -> HolderPack {
                return Memory::MakeHolderPack(TYPES::NIL_TYPE);
            }
        };
    }
    void RegisterUnaryNilOperators() noexcept {
        // + NIL
        UNARY_OP_TABLE[{Lexer::Tokens::T_PLUS, TYPES::NIL_TYPE}] = {
            [](HolderPack&& arg) noexcept -> HolderPack {
                return std::move(arg);
            }
        };
        // - NIL
        UNARY_OP_TABLE[{Lexer::Tokens::T_MINUS, TYPES::NIL_TYPE}] = {
            [](HolderPack&& arg) noexcept -> HolderPack {
                return std::move(arg);
            }
        };
        // not NIL
        UNARY_OP_TABLE[{Lexer::Tokens::T_NOT, TYPES::NIL_TYPE}] = {
            [](HolderPack&& arg) noexcept -> HolderPack {
                return std::move(arg);
            }
        };
    }

    void RegisterUnaryOperators() noexcept {
        RegisterUnaryNumOperators();
        RegisterUnaryStringOperators();
        RegisterUnaryBoolOperators();
        RegisterUnaryNilOperators();
    }

    /// Binary operations
    void RegisterBinaryNumOperators() noexcept {
        // (not set type) = NUM
        BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
                arg_left->holder = std::move(arg_right->holder);
                arg_left->type = TYPES::NUM_TYPE;
                return arg_left;
            }
        };
        // NUM (setted) = NUM
        BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
                if (arg_left.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
                arg_left->holder = std::move(arg_right->holder);
                arg_left->type = TYPES::NUM_TYPE;
                return arg_left;
            }
        };
        // NUM + NUM
        BINARY_OP_TABLE[{Lexer::Tokens::T_PLUS, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
                return Memory::MakeHolderPack(
                    std::get<double>(arg_left->holder) + std::get<double>(arg_right->holder),
                    TYPES::NUM_TYPE
                );
            }
        };
        // NUM - NUM
        BINARY_OP_TABLE[{Lexer::Tokens::T_MINUS, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
                return Memory::MakeHolderPack(
                    std::get<double>(arg_left->holder) - std::get<double>(arg_right->holder),
                    TYPES::NUM_TYPE
                );
            }
        };
        // NUM * NUM
        BINARY_OP_TABLE[{Lexer::Tokens::T_MULT, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
                return Memory::MakeHolderPack(
                    std::get<double>(arg_left->holder) * std::get<double>(arg_right->holder),
                    TYPES::NUM_TYPE
                );
            }
        };
        // NUM / NUM
        BINARY_OP_TABLE[{Lexer::Tokens::T_DIV, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
                return Memory::MakeHolderPack(
                    std::get<double>(arg_left->holder) / std::get<double>(arg_right->holder),
                    TYPES::NUM_TYPE
                );
            }
        };
        // NUM % NUM
        BINARY_OP_TABLE[{Lexer::Tokens::T_MOD, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
                return Memory::MakeHolderPack(
                    static_cast<double>(
                        static_cast<int>(std::get<double>(arg_left->holder)) % static_cast<int>(std::get<double>(arg_right->holder))
                    ),
                    TYPES::NUM_TYPE
                );
            }
        };
        // NUM ^ NUM
        BINARY_OP_TABLE[{Lexer::Tokens::T_XOR, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
                return Memory::MakeHolderPack(
                    std::pow(std::get<double>(arg_left->holder), std::get<double>(arg_right->holder)),
                    TYPES::NUM_TYPE
                );
            }
        };
    }
    void RegisterBinaryStringOperators() noexcept {
        // (not set type) = STRING
        BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::STRING_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
                arg_left->holder = std::move(arg_right->holder);
                arg_left->type = TYPES::STRING_TYPE;
                return arg_left;
            }
        };
        // STRING (setted) = STRING
        BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::STRING_TYPE, TYPES::STRING_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
                if (arg_left.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
                arg_left->holder = std::move(arg_right->holder);
                arg_left->type = TYPES::STRING_TYPE;
                return arg_left;
            }
        };
        // STRING + STRING
        BINARY_OP_TABLE[{Lexer::Tokens::T_PLUS, TYPES::STRING_TYPE, TYPES::STRING_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
                return Memory::MakeHolderPack(
                    std::get<std::string>(arg_left->holder) + std::get<std::string>(arg_right->holder),
                    TYPES::STRING_TYPE
                );
            }
        };
        // STRING - STRING
        BINARY_OP_TABLE[{Lexer::Tokens::T_MINUS, TYPES::STRING_TYPE, TYPES::STRING_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
                std::string& left_str = std::get<std::string>(arg_left->holder);
                std::string& right_str = std::get<std::string>(arg_right->holder);
                if (size_t i = left_str.rfind(right_str); i != std::string::npos) {
                    return Memory::MakeHolderPack(
                        left_str.substr(0, i), TYPES::STRING_TYPE
                    );
                }
                return arg_left;
            }
        };
        // STRING * NUM
        BINARY_OP_TABLE[{Lexer::Tokens::T_MULT, TYPES::STRING_TYPE, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
                std::string& left_str = std::get<std::string>(arg_left->holder);
                double count = std::get<double>(arg_right->holder);
                std::string result;
                for (double i = 0; i < left_str.size() * count; ++i) {
                    result += left_str[static_cast<size_t>(i) % left_str.size()];
                }
                return Memory::MakeHolderPack(result, TYPES::STRING_TYPE);
            }
        };
        // NUM * STRING
        BINARY_OP_TABLE[{Lexer::Tokens::T_MULT, TYPES::NUM_TYPE, TYPES::STRING_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
                return BINARY_OP_TABLE[{Lexer::Tokens::T_MULT, TYPES::STRING_TYPE, TYPES::NUM_TYPE}](std::move(arg_right), std::move(arg_left));
            }
        };
    }
    void RegisterBinaryBoolOperators() noexcept {
    /// EQUALS
        // NUM == NUM
        BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
                return Memory::MakeHolderPack(
                    std::get<double>(arg_left->holder) == std::get<double>(arg_right->holder),
                    TYPES::BOOL_TYPE
                );
            }
        };
        // NUM == BOOL
        BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NUM_TYPE, TYPES::BOOL_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
                return Memory::MakeHolderPack(
                    (std::get<double>(arg_left->holder) != 0) == std::get<bool>(arg_right->holder),
                    TYPES::BOOL_TYPE
                );
            }
        };
        // BOOL == NUM
        BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::BOOL_TYPE, TYPES::NUM_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
                return BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NUM_TYPE, TYPES::BOOL_TYPE}](
                    std::move(arg_right), std::move(arg_left));
            }
        };

        // STRING == STRING
        BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::STRING_TYPE, TYPES::STRING_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
                return Memory::MakeHolderPack(
                    std::get<std::string>(arg_left->holder) == std::get<std::string>(arg_right->holder),
                    TYPES::BOOL_TYPE
                );
            }
        };
        // STRING == BOOL
        BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::STRING_TYPE, TYPES::BOOL_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
                return Memory::MakeHolderPack(
                    (std::get<std::string>(arg_left->holder).size() != 0) == std::get<bool>(arg_right->holder),
                    TYPES::BOOL_TYPE
                );
            }
        };
        // BOOL == STRING
        BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::BOOL_TYPE, TYPES::STRING_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
                return BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::STRING_TYPE, TYPES::BOOL_TYPE}](
                    std::move(arg_right), std::move(arg_left));
            }
        };

        // NIL == NIL
        BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NIL_TYPE, TYPES::NIL_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
                return Memory::MakeHolderPack(true, TYPES::BOOL_TYPE);
            }
        };
        // NIL == BOOL
        BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NIL_TYPE, TYPES::BOOL_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
                return Memory::MakeHolderPack(false == (std::get<bool>(arg_right->holder)), TYPES::BOOL_TYPE);
            }
        };
        // BOOL == NIL
        BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::BOOL_TYPE, TYPES::NIL_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
                return BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NIL_TYPE, TYPES::BOOL_TYPE}](
                    std::move(arg_right), std::move(arg_left));
            }
        };

        // BOOL == BOOL
        BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::BOOL_TYPE, TYPES::BOOL_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
                return Memory::MakeHolderPack(
                    std::get<bool>(arg_left->holder) == std::get<bool>(arg_right->holder),
                    TYPES::BOOL_TYPE
                );
            }
        };

        // LIST == LIST
        BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::LIST_TYPE, TYPES::LIST_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
                std::vector<HolderPack>& ls_left = std::get<Memory::ListHolderPtr>(arg_left->holder)->data;
                std::vector<HolderPack>& ls_right = std::get<Memory::ListHolderPtr>(arg_right->holder)->data;

                auto exit_false = Memory::MakeHolderPack(false, TYPES::BOOL_TYPE);
                auto exit_true = Memory::MakeHolderPack(true, TYPES::BOOL_TYPE);

                if (ls_left.size() != ls_right.size()) { return std::move(exit_false); }
                
                for (size_t i = 0; i != ls_left.size(); ++i) {
                    if (ls_left[i]->type != ls_right[i]->type) { return std::move(exit_false); }
                    BinaryOpTableKey key = {Lexer::Tokens::T_COMP_EQUAL, ls_left[i]->type, ls_right[i]->type};
                    auto iter = BINARY_OP_TABLE.find(std::move(key));
                    if (iter == BINARY_OP_TABLE.end()) { return std::move(exit_false); }
                    if (!iter->second(std::move(ls_left[i]), std::move(ls_right[i]))) {
                        return std::move(exit_false);
                    }
                }
                return std::move(exit_true);
            }
        };

    /// GREATER
    }
    void RegisterBinaryNilOperators() noexcept {}

    void RegisterBinaryListOperators() noexcept {
        // (not set type) = LIST
        BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::LIST_TYPE}] = {
            [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
                arg_left->holder = std::move(arg_right->holder);
                arg_left->type = TYPES::LIST_TYPE;
                return arg_left;
            }
        };
    }

    void RegisterBinaryOperators() noexcept {
        RegisterBinaryNumOperators();
        RegisterBinaryBoolOperators();
        RegisterBinaryNilOperators();
        RegisterBinaryStringOperators();
        RegisterBinaryListOperators();
    }

    [[nodiscard]] Expected ExecUnaryOperation(Parser::UnaryOp* node, HolderPack&& computed) {
        UnaryOpTableKey key = {node->op, computed->type};
        auto iter = UNARY_OP_TABLE.find(std::move(key));
        if (iter == UNARY_OP_TABLE.end()) {
            return std::unexpected(Lexer::Token{
                Errors::OperatorErrors::OperatorUnaryError(Lexer::TOKENS_TO_STR[node->op], TYPE_TO_STR[computed->type]), 
                node->token});
        }
        return iter->second(std::move(computed));
    }

    [[nodiscard]] Expected ExecBinaryOperation(Parser::BinaryOp* node, HolderPack&& computed_left, HolderPack&& computed_right) {
        BinaryOpTableKey key = {node->op, computed_left->type, computed_right->type};
        auto iter = BINARY_OP_TABLE.find(std::move(key));
        if (iter == BINARY_OP_TABLE.end()) {
            return std::unexpected(Lexer::Token{
                Errors::OperatorErrors::OperatorBinaryError(Lexer::TOKENS_TO_STR[node->op], TYPE_TO_STR[computed_left->type], TYPE_TO_STR[computed_right->type]),
                node->token});
        }
        try {
            return iter->second(std::move(computed_left), std::move(computed_right));
        } catch (Error& e) {
            return std::unexpected(Lexer::Token{
                Error(e.what()),
                node->token});
        }
    }
    [[nodiscard]] Expected ExecBinaryOperation(Lexer::Tokens op, Parser::NodePtr& node, HolderPack&& computed_left, HolderPack&& computed_right) {
        BinaryOpTableKey key = {op, computed_left->type, computed_right->type};
        auto iter = BINARY_OP_TABLE.find(std::move(key));
        if (iter == BINARY_OP_TABLE.end()) {
            return std::unexpected(Lexer::Token{
                Errors::OperatorErrors::OperatorBinaryError(Lexer::TOKENS_TO_STR[op], TYPE_TO_STR[computed_left->type], TYPE_TO_STR[computed_right->type]),
                node->token});
        }
        try {
            return iter->second(std::move(computed_left), std::move(computed_right));
        } catch (Error& e) {
            return std::unexpected(Lexer::Token{
                Error(e.what()),
                node->token});
        }
    }
}