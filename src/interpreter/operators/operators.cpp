#include "operators.h"
#include <memory>
#include <string>
#include <vector>

namespace Operators {

std::unordered_map<UnaryOpTableKey, UnaryFunction> UNARY_OP_TABLE;
std::unordered_map<BinaryOpTableKey, BinaryFunction> BINARY_OP_TABLE;

/// Unary operations
void RegisterUnaryNumOperators() noexcept {
    // + NUM
    UNARY_OP_TABLE[{Lexer::Tokens::T_PLUS, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg) noexcept -> HolderPack {
            return {
                std::get<double>(arg->holder),
                TYPES::NUM_TYPE
            };
        }
    };
    // - NUM
    UNARY_OP_TABLE[{Lexer::Tokens::T_MINUS, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg) noexcept -> HolderPack {
            return {
                -std::get<double>(arg->holder),
                TYPES::NUM_TYPE
            };
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
            return {
                !(std::get<double>(arg->holder) != 0),
                TYPES::BOOL_TYPE
            };
        }
    };
    // not STRING
    UNARY_OP_TABLE[{Lexer::Tokens::T_NOT, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg) noexcept -> HolderPack {
            return {
                std::get<std::string>(arg->holder).size() == 0,
                TYPES::BOOL_TYPE
            };
        }
    };
    // not BOOL
    UNARY_OP_TABLE[{Lexer::Tokens::T_NOT, TYPES::BOOL_TYPE}] = {
        [](HolderPack&& arg) noexcept -> HolderPack {
            return {!(std::get<bool>(arg->holder)), TYPES::BOOL_TYPE};
        }
    };
    // not NIL
    UNARY_OP_TABLE[{Lexer::Tokens::T_NOT, TYPES::NIL_TYPE}] = {
        [](HolderPack&& arg) noexcept -> HolderPack {
            return {TYPES::NIL_TYPE};
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
            arg_left->holder = std::get<double>(arg_right->holder);  // (copy by value)
            arg_left->type = TYPES::NUM_TYPE;
            return arg_left;
        }
    };

    // NUM (setted) = NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::NUM_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // STRING (setted) = NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::STRING_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::NUM_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // BOOL (setted) = NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::BOOL_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::NUM_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // NIL (setted) = NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NIL_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::NUM_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // LIST (setted) = NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::LIST_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::NUM_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // FUNC (setted) = NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::FUNC_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::NUM_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };

    // NUM + NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_PLUS, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
            return {
                std::get<double>(arg_left->holder) + std::get<double>(arg_right->holder),
                TYPES::NUM_TYPE
            };
        }
    };
    // NUM - NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_MINUS, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
            return {
                std::get<double>(arg_left->holder) - std::get<double>(arg_right->holder),
                TYPES::NUM_TYPE
            };
        }
    };
    // NUM * NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_MULT, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
            return {
                std::get<double>(arg_left->holder) * std::get<double>(arg_right->holder),
                TYPES::NUM_TYPE
            };
        }
    };
    // NUM / NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_DIV, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
            return {
                std::get<double>(arg_left->holder) / std::get<double>(arg_right->holder),
                TYPES::NUM_TYPE
            };
        }
    };
    // NUM % NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_MOD, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
            return {
                static_cast<double>(
                    static_cast<int>(std::get<double>(arg_left->holder)) % static_cast<int>(std::get<double>(arg_right->holder))
                ),
                TYPES::NUM_TYPE
            };
        }
    };
    // NUM ^ NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_XOR, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
            return {
                std::pow(std::get<double>(arg_left->holder), std::get<double>(arg_right->holder)),
                TYPES::NUM_TYPE
            };
        }
    };
    // NUM == NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                std::get<double>(arg_left->holder) == std::get<double>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
    // NUM != NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_NON_EQUAL, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                std::get<double>(arg_left->holder) != std::get<double>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
    // NUM < NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_SMALLER, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                std::get<double>(arg_left->holder) < std::get<double>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
    // NUM <= NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_SMALLER_OR_EQ, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                std::get<double>(arg_left->holder) <= std::get<double>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
    // NUM > NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_GREATER, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                std::get<double>(arg_left->holder) > std::get<double>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
    // NUM >= NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_GREATER_OR_EQ, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                std::get<double>(arg_left->holder) >= std::get<double>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
    // NUM += NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL_PLUS, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            std::get<double>(arg_left->holder) += std::get<double>(arg_right->holder);
            return std::move(arg_left);
        }
    };
    // NUM -= NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL_MINUS, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            std::get<double>(arg_left->holder) -= std::get<double>(arg_right->holder);
            return std::move(arg_left);
        }
    };
    // NUM *= NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL_MULT, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            std::get<double>(arg_left->holder) *= std::get<double>(arg_right->holder);
            return std::move(arg_left);
        }
    };
    // NUM /= NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL_DIV, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            std::get<double>(arg_left->holder) /= std::get<double>(arg_right->holder);
            return std::move(arg_left);
        }
    };
    // NUM %= NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL_MOD, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            arg_left->holder = std::get<double>(RawExecBinaryOperation(
                Lexer::Tokens::T_MOD, HolderPack(arg_left), std::move(arg_right)
            )->holder);
            return std::move(arg_left);
        }
    };
    // NUM ^= NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL_XOR, TYPES::NUM_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            std::get<double>(arg_left->holder) = std::pow(std::get<double>(arg_left->holder), std::get<double>(arg_right->holder));
            return std::move(arg_left);
        }
    };
}
void RegisterBinaryStringOperators() noexcept {
    // (not set type) = STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            *arg_left.pack = *arg_right.pack;
            return arg_left;
        }
    };

    // NUM (setted) = STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NUM_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::STRING_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // STRING (setted) = STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::STRING_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::STRING_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // BOOL (setted) = STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::BOOL_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::STRING_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // NIL (setted) = STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NIL_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::STRING_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // LIST (setted) = STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::LIST_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::STRING_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // FUNC (setted) = STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::FUNC_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::STRING_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };

    // STRING + STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_PLUS, TYPES::STRING_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
            return {
                std::get<std::string>(arg_left->holder) + std::get<std::string>(arg_right->holder),
                TYPES::STRING_TYPE
            };
        }
    };
    // STRING - STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_MINUS, TYPES::STRING_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
            std::string& left_str = std::get<std::string>(arg_left->holder);
            std::string& right_str = std::get<std::string>(arg_right->holder);
            if (size_t i = left_str.rfind(right_str); i != std::string::npos) {
                return {left_str.substr(0, i), TYPES::STRING_TYPE};
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
            return {std::move(result), TYPES::STRING_TYPE};
        }
    };
    // NUM * STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_MULT, TYPES::NUM_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
            return BINARY_OP_TABLE[{Lexer::Tokens::T_MULT, TYPES::STRING_TYPE, TYPES::NUM_TYPE}](std::move(arg_right), std::move(arg_left));
        }
    };
    // STRING == STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::STRING_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                std::get<std::string>(arg_left->holder) == std::get<std::string>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
    // STRING != STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_NON_EQUAL, TYPES::STRING_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                std::get<std::string>(arg_left->holder) != std::get<std::string>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
    // STRING < STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_SMALLER, TYPES::STRING_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                std::get<std::string>(arg_left->holder) < std::get<std::string>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
    // STRING <= STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_SMALLER_OR_EQ, TYPES::STRING_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                std::get<std::string>(arg_left->holder) <= std::get<std::string>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
    // STRING > STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_GREATER, TYPES::STRING_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                std::get<std::string>(arg_left->holder) > std::get<std::string>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
    // STRING >= STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_GREATER_OR_EQ, TYPES::STRING_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                std::get<std::string>(arg_left->holder) >= std::get<std::string>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
    // STRING += STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL_PLUS, TYPES::STRING_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            std::get<std::string>(arg_left->holder) = std::get<std::string>(RawExecBinaryOperation(
                Lexer::Tokens::T_PLUS,
                HolderPack(arg_left),
                std::move(arg_right)
            )->holder);
            return std::move(arg_left);
        }
    };
    // STRING -= STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL_MINUS, TYPES::STRING_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            std::get<std::string>(arg_left->holder) = std::get<std::string>(RawExecBinaryOperation(
                Lexer::Tokens::T_MINUS,
                HolderPack(arg_left),
                std::move(arg_right)
            )->holder);
            return std::move(arg_left);
        }
    };
    // STRING *= NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL_MULT, TYPES::STRING_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            std::get<std::string>(arg_left->holder) = std::get<std::string>(RawExecBinaryOperation(
                Lexer::Tokens::T_MULT,
                HolderPack(arg_left),
                std::move(arg_right)
            )->holder);
            return std::move(arg_left);
        }
    };
}
void RegisterBinaryBoolOperators() noexcept {
    // (not set type) = BOOL
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::BOOL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            arg_left->holder = std::get<bool>(arg_right->holder);
            arg_left->type = TYPES::BOOL_TYPE;
            return arg_left;
        }
    };

    // NUM (setted) = BOOL
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NUM_TYPE, TYPES::BOOL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::BOOL_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // STRING (setted) = BOOL
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::STRING_TYPE, TYPES::BOOL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::BOOL_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // BOOL (setted) = BOOL
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::BOOL_TYPE, TYPES::BOOL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::BOOL_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // NIL (setted) = BOOL
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NIL_TYPE, TYPES::BOOL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::BOOL_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // LIST (setted) = BOOL
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::LIST_TYPE, TYPES::BOOL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::BOOL_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // FUNC (setted) = BOOL
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::FUNC_TYPE, TYPES::BOOL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::BOOL_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };

    // BOOL == BOOL
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::BOOL_TYPE, TYPES::BOOL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                std::get<bool>(arg_left->holder) == std::get<bool>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };

    // NUM == BOOL
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NUM_TYPE, TYPES::BOOL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                (std::get<double>(arg_left->holder) != 0) == std::get<bool>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
    // BOOL == NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::BOOL_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NUM_TYPE, TYPES::BOOL_TYPE}](
                std::move(arg_right), std::move(arg_left));
        }
    };

    // STRING == BOOL
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::STRING_TYPE, TYPES::BOOL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                (std::get<std::string>(arg_left->holder).size() != 0) == std::get<bool>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
    // BOOL == STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::BOOL_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::STRING_TYPE, TYPES::BOOL_TYPE}](
                std::move(arg_right), std::move(arg_left));
        }
    };

    // BOOL and BOOL
    BINARY_OP_TABLE[{Lexer::Tokens::T_AND, TYPES::BOOL_TYPE, TYPES::BOOL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                std::get<bool>(arg_left->holder) && std::get<bool>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
    // BOOL or BOOL
    BINARY_OP_TABLE[{Lexer::Tokens::T_OR, TYPES::BOOL_TYPE, TYPES::BOOL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {
                std::get<bool>(arg_left->holder) || std::get<bool>(arg_right->holder),
                TYPES::BOOL_TYPE
            };
        }
    };
}
void RegisterBinaryNilOperators() noexcept {
    // (not set type) = NIL
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::NIL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {TYPES::NIL_TYPE};
        }
    };
    
    // NUM (setted) = NIL
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NUM_TYPE, TYPES::NIL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::NIL_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // STRING (setted) = NIL
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::STRING_TYPE, TYPES::NIL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::NIL_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // BOOL (setted) = NIL
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::BOOL_TYPE, TYPES::NIL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::NIL_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // NIL (setted) = NIL
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NIL_TYPE, TYPES::NIL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::NIL_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // LIST (setted) = NIL
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::LIST_TYPE, TYPES::NIL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::NIL_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // FUNC (setted) = NIL
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::FUNC_TYPE, TYPES::NIL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::NIL_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };

    // NIL == NIL
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NIL_TYPE, TYPES::NIL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {true, TYPES::BOOL_TYPE};
        }
    };
    // NIL == NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NIL_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {false, TYPES::BOOL_TYPE};
        }
    };
    // NUM == NIL
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NUM_TYPE, TYPES::NIL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NIL_TYPE, TYPES::NUM_TYPE}](
                std::move(arg_right), std::move(arg_left));
        }
    };
    // STRING == NIL
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::STRING_TYPE, TYPES::NIL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {false, TYPES::BOOL_TYPE};
        }
    };
    // NIL == STRING
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NIL_TYPE, TYPES::STRING_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::STRING_TYPE, TYPES::NIL_TYPE}](
                std::move(arg_right), std::move(arg_left));
        }
    };
    // NIL == BOOL
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NIL_TYPE, TYPES::BOOL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {false, TYPES::BOOL_TYPE};
        }
    };
    // BOOL == NIL
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::BOOL_TYPE, TYPES::NIL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NIL_TYPE, TYPES::BOOL_TYPE}](
                std::move(arg_right), std::move(arg_left));
        }
    };
    // LIST == NIL
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::LIST_TYPE, TYPES::NIL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {false, TYPES::BOOL_TYPE};
        }
    };
    // NIL == LIST
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NIL_TYPE, TYPES::LIST_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::LIST_TYPE, TYPES::NIL_TYPE}](
                std::move(arg_right), std::move(arg_left));
        }
    };
    // FUNC == NIL
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::FUNC_TYPE, TYPES::NIL_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return {false, TYPES::BOOL_TYPE};
        }
    };
    // NIL == FUNC
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::NIL_TYPE, TYPES::FUNC_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            return BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::FUNC_TYPE, TYPES::NIL_TYPE}](
                std::move(arg_right), std::move(arg_left));
        }
    };
}

void RegisterBinaryListOperators() noexcept {
    // (not set type) = LIST
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::LIST_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            *arg_left.pack = *arg_right.pack;
            return arg_left;
        }
    };

    // NUM (setted) = LIST
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NUM_TYPE, TYPES::LIST_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::LIST_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // STRING (setted) = LIST
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::STRING_TYPE, TYPES::LIST_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::LIST_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // BOOL (setted) = LIST
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::BOOL_TYPE, TYPES::LIST_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::LIST_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // NIL (setted) = LIST
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NIL_TYPE, TYPES::LIST_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::LIST_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // LIST (setted) = LIST
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::LIST_TYPE, TYPES::LIST_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::LIST_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // FUNC (setted) = LIST
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::FUNC_TYPE, TYPES::LIST_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::LIST_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };

    // LIST == LIST
    BINARY_OP_TABLE[{Lexer::Tokens::T_COMP_EQUAL, TYPES::LIST_TYPE, TYPES::LIST_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
            std::vector<HolderPack>& ls_left = std::get<Memory::ListHolderPtr>(arg_left->holder)->data;
            std::vector<HolderPack>& ls_right = std::get<Memory::ListHolderPtr>(arg_right->holder)->data;

            auto exit_false = HolderPack(false, TYPES::BOOL_TYPE);
            auto exit_true = HolderPack(true, TYPES::BOOL_TYPE);

            if (ls_left.size() != ls_right.size()) { return std::move(exit_false); }

            for (size_t i = 0; i != ls_left.size(); ++i) {
                if (ls_left[i]->type != ls_right[i]->type) { return std::move(exit_false); }
                try {
                    auto hp = RawExecBinaryOperation(Lexer::Tokens::T_COMP_EQUAL, HolderPack(ls_left[i]), HolderPack(ls_right[i]));
                    if (!std::get<bool>(hp->holder)) {
                        return std::move(exit_false);
                    }
                } catch (...) { return std::move(exit_false); }
            }
            return std::move(exit_true);
        }
    };

    // LIST + LIST
    BINARY_OP_TABLE[{Lexer::Tokens::T_PLUS, TYPES::LIST_TYPE, TYPES::LIST_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            std::vector<HolderPack> list;
            list.insert(list.end(),
                    std::get<Memory::ListHolderPtr>(arg_left->holder)->data.begin(),
                    std::get<Memory::ListHolderPtr>(arg_left->holder)->data.end());
            list.insert(list.end(),
                    std::get<Memory::ListHolderPtr>(arg_right->holder)->data.begin(),
                    std::get<Memory::ListHolderPtr>(arg_right->holder)->data.end());
            return HolderPack(MakeListHolder(std::move(list)), TYPES::LIST_TYPE);
        }
    };
    // LIST += LIST
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL_PLUS, TYPES::LIST_TYPE, TYPES::LIST_TYPE}] = {
         [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            *arg_left.pack = *RawExecBinaryOperation(
                Lexer::Tokens::T_PLUS,
                HolderPack(arg_left),
                std::move(arg_right)
            ).pack;
            return std::move(arg_left);
         }
    };
    // LIST * NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_MULT, TYPES::LIST_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            double index = std::get<double>(arg_right->holder);
            if (!Utils::IsInteger(index) || index < 0) {
                throw Errors::TypeErrors::IndexNotPositiveInteger();
            }
            auto& ref_list = std::get<Memory::ListHolderPtr>(arg_left->holder)->data;
            std::vector<HolderPack> list;
            for (double i = 0; i < index; ++i) {
                list.insert(list.begin(), ref_list.begin(), ref_list.end());
            }
            return HolderPack(MakeListHolder(std::move(list)), TYPES::LIST_TYPE);
        }
    };
    // LIST *= NUM
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL_MULT, TYPES::LIST_TYPE, TYPES::NUM_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            *arg_left.pack = *RawExecBinaryOperation(
                Lexer::Tokens::T_MULT,
                HolderPack(arg_left),
                std::move(arg_right)
            ).pack;
            return std::move(arg_left);

        }
    };
}

void RegisterBinaryFuncOperators() noexcept {
    // (not set type) = FUNC
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::FUNC_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) noexcept -> HolderPack {
            *arg_left.pack = *arg_right.pack;
            return arg_left;
        }
    };

    // NUM (setted) = FUNC
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NUM_TYPE, TYPES::FUNC_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::FUNC_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // STRING (setted) = FUNC
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::STRING_TYPE, TYPES::FUNC_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::FUNC_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // BOOL (setted) = FUNC
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::BOOL_TYPE, TYPES::FUNC_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::FUNC_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // NIL (setted) = FUNC
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NIL_TYPE, TYPES::FUNC_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::FUNC_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // LIST (setted) = FUNC
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::LIST_TYPE, TYPES::FUNC_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::FUNC_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
    // FUNC (setted) = FUNC
    BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::FUNC_TYPE, TYPES::FUNC_TYPE}] = {
        [](HolderPack&& arg_left, HolderPack&& arg_right) -> HolderPack {
            if (arg_left.pack.use_count() == 1) { throw Errors::RunTime::AssignLiteral(); }
            return BINARY_OP_TABLE[{Lexer::Tokens::T_EQUAL, TYPES::NOT_SET_TYPE, TYPES::FUNC_TYPE}](std::move(arg_left), std::move(arg_right));
        }
    };
}

void RegisterBinaryOperators() noexcept {
    RegisterBinaryNumOperators();
    RegisterBinaryBoolOperators();
    RegisterBinaryNilOperators();
    RegisterBinaryStringOperators();
    RegisterBinaryListOperators();
    RegisterBinaryFuncOperators();
}

[[nodiscard]] HolderPack ExecUnaryOperation(Parser::UnaryOp* node, HolderPack&& computed) {
    UnaryOpTableKey key = {node->op, computed->type};
    auto iter = UNARY_OP_TABLE.find(std::move(key));
    if (iter == UNARY_OP_TABLE.end()) {
        throw Errors::OperatorErrors::OperatorUnaryError(
            Lexer::TOKENS_TO_STR[node->op],
            TYPE_TO_STR[computed->type],
            node->token.lineno,
            node->token.column
        );
    }
    return iter->second(std::move(computed));
}

[[nodiscard]] HolderPack ExecBinaryOperation(Parser::BinaryOp* node, HolderPack&& computed_left, HolderPack&& computed_right) {
    BinaryOpTableKey key = {node->op, computed_left->type, computed_right->type};
    auto iter = BINARY_OP_TABLE.find(std::move(key));
    if (iter == BINARY_OP_TABLE.end()) {
        throw Errors::OperatorErrors::OperatorBinaryError(
            Lexer::TOKENS_TO_STR[node->op],
            TYPE_TO_STR[computed_left->type],
            TYPE_TO_STR[computed_right->type],
            node->token.lineno,
            node->token.column
        );
    }
    return iter->second(std::move(computed_left), std::move(computed_right));
}
[[nodiscard]] HolderPack ExecBinaryOperation(const Lexer::Token& op, HolderPack&& computed_left, HolderPack&& computed_right) {
    BinaryOpTableKey key = {op.token, computed_left->type, computed_right->type};
    auto iter = BINARY_OP_TABLE.find(std::move(key));
    if (iter == BINARY_OP_TABLE.end()) {
        throw Errors::OperatorErrors::OperatorBinaryError(
            Lexer::TOKENS_TO_STR[op.token],
            TYPE_TO_STR[computed_left->type],
            TYPE_TO_STR[computed_right->type],
            op.lineno,
            op.column
        );
    }
    return iter->second(std::move(computed_left), std::move(computed_right));
}
[[maybe_unused]] HolderPack RawExecBinaryOperation(Lexer::Tokens op, HolderPack&& computed_left, HolderPack&& computed_right) {
    BinaryOpTableKey key = {op, computed_left->type, computed_right->type};
    auto iter = BINARY_OP_TABLE.find(std::move(key));
    if (iter == BINARY_OP_TABLE.end()) {
        throw Errors::OperatorErrors::OperatorBinaryError(Lexer::TOKENS_TO_STR[op], TYPE_TO_STR[computed_left->type], TYPE_TO_STR[computed_right->type]);
    }
    return iter->second(std::move(computed_left), std::move(computed_right));
}

} // end Operators
