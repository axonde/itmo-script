#include "interpreter.h"

namespace Interpreter {

/// ERRORS
void SyntaxError(const Lexer::Token& token) {
    using ErrorType = std::shared_ptr<Errors::Error>;
    Errors::PrintError("Syntax error", std::get<ErrorType>(token.value).get(), token.column, token.lineno);
}
void RunTimeError(const Lexer::Token& token) {
    using ErrorType = std::shared_ptr<Errors::Error>;
    Errors::PrintError("RunTime error", std::get<ErrorType>(token.value).get(), token.column, token.lineno);
}

/// INTERPRET
bool Interpret(std::istream& input, std::ostream& output, bool IsRepl = false) {
    if (IsRepl) {
        std::string line;
        do {
            std::cout << Patterns::CMD;
            std::getline(input, line);
        } while (line.size() == 0);

        Lexer::Tokenizer tokenizer(line);

        Lexer::Token token;
        tokenizer >> token;
        while (token.token != Lexer::Tokens::T_EOF) {
            std::cout << token.token << " ; ";
            if (token.token == Lexer::Tokens::T_BAD) {
                SyntaxError(token);
                return false;
            }
            tokenizer >> token;
        }
        std::cout << '\n';

        return true;
    }

    size_t size = input.tellg();
    input.seekg(0);
    std::string program(size, '\0');
    input.read(&program[0], size);

    return Interpret(program, output);
}

bool Interpret(std::string& program, std::ostream& output) {
    Runner runner(std::move(program), output);

    if (runner.GetRoot()->node == Parser::Nodes::N_BAD) {
        SyntaxError(std::move(runner.GetRoot()->token));
        return false;
    }

    if (auto expected = runner.Run(); !expected) {
        RunTimeError(std::move(expected.error()));
        return false;
    }
    return true;
}

} // end Interpreter


Runner::Expected Runner::Run() {
    Operators::RegisterUnaryOperators();
    Operators::RegisterBinaryOperators();

    return Visit(parser.root);
}

Runner::Expected Runner::Visit(Runner::NodePtr& node) {
    std::cout << "visit ...\n\t";

    switch (node->node) {
        case Parser::Nodes::N_NUM_LITERAL:
            return VisitNumLiteral(node);
        case Parser::Nodes::N_STRING_LITERAL:
            return VisitStringLiteral(node);
        case Parser::Nodes::N_BOOL_LITERAL:
            return VisitBoolLiteral(node);
        case Parser::Nodes::N_NIL_LITERAL:
            return VisitNilLiteral(node);

        case Parser::Nodes::N_VAR:
            return VisitVar(node);
        case Parser::Nodes::N_LIST:
            return VisitList(node);

        case Parser::Nodes::N_UNARY_OP:
            return VisitUnaryOp(node);
        case Parser::Nodes::N_BINARY_OP:
            return VisitBinaryOp(node);
        case Parser::Nodes::N_SUBSCRIPT:
            return VisitSubscript(node);
        
        case Parser::Nodes::N_IF:
            return VisitIf(node);
        case Parser::Nodes::N_FOR:
            return VisitFor(node);
        case Parser::Nodes::N_WHILE:
            return VisitWhile(node);
        case Parser::Nodes::N_BREAK:
            return VisitBreak(node);
        case Parser::Nodes::N_CONTINUE:
            return VisitContinue(node);

        case Parser::Nodes::N_FUNC:
            return VisitFunc(node);
        case Parser::Nodes::N_FUNC_CALL:
            return VisitFuncCall(node);
        case Parser::Nodes::N_RETURN:
            return VisitReturn(node);
        case Parser::Nodes::N_COMPOUND:
            return VisitCompound(node);
        default:
            return std::unexpected(Lexer::Token(
                InternalError(), node->token
            ));
    }
}


/// LITERALS
Runner::Expected Runner::VisitNumLiteral(Runner::NodePtr& node) {
    std::cout << "visit num literal\n";

    Parser::NumLiteral* num_literal = static_cast<Parser::NumLiteral*>(node.get());
    return HolderPack{
        std::make_shared<HolderTypes>(num_literal->value),
        TYPES::NUM_TYPE
    };
}

Runner::Expected Runner::VisitStringLiteral(Runner::NodePtr& node) {
    std::cout << "visit str literal\n";

    Parser::StringLiteral* str_literal = static_cast<Parser::StringLiteral*>(node.get());
    return HolderPack{
        std::make_shared<HolderTypes>(str_literal->value),
        TYPES::STRING_TYPE
    };
}

Runner::Expected Runner::VisitBoolLiteral(Runner::NodePtr& node) {
    std::cout << "visit bool literal\n";

    Parser::BoolLiteral* bool_literal = static_cast<Parser::BoolLiteral*>(node.get());
    return HolderPack{
        std::make_shared<HolderTypes>(bool_literal->value),
        TYPES::BOOL_TYPE
    };
}

Runner::Expected Runner::VisitNilLiteral(Runner::NodePtr& node) {
    std::cout << "visit nil literal\n";

    return HolderPack{
        std::make_shared<HolderTypes>(),
        TYPES::NIL_TYPE
    };
}

/// LITERAL EXPR
Runner::Expected Runner::VisitVar(Runner::NodePtr& node) {
    std::cout << "visit var\n";

    std::string_view id = static_cast<Parser::Var*>(node.get())->id;
    try {
        return stack_frame.Lookup(id);
    } catch (Errors::MemoryErrors::NotFound e) {
        return std::unexpected(Lexer::Token(std::move(e), node->token));
    }
}
Runner::Expected Runner::VisitList(Runner::NodePtr& node) {
    std::cout << "visit list\n";

    Parser::List* list = static_cast<Parser::List*>(node.get());
    std::vector<HolderPack> data;
    for (auto& child : list->data) {
        auto visited = Visit(child);
        if (visited) {
            data.push_back(std::move(*visited));
        } else {
            return std::unexpected(visited.error());
        }
    }
    return HolderPack{
        std::make_shared<HolderTypes>(Memory::ListHolder(std::move(data))),
        TYPES::LIST_TYPE
    };
}

/// OPERATIONS
Runner::Expected Runner::VisitUnaryOp(Runner::NodePtr& node) {
    std::cout << "visit unary\n";

    Parser::UnaryOp* ptr = static_cast<Parser::UnaryOp*>(node.get());
    auto computed_operand = Visit(ptr->operand);
    if (computed_operand) {
        if (auto computed = Operators::ExecUnaryOperation(ptr, std::move(*computed_operand)); computed) {
            return std::move(*computed);
        } else {
            return std::unexpected(std::move(computed.error()));
        }
    } else {
        return std::unexpected(std::move(computed_operand.error()));
    }
}
Runner::Expected Runner::VisitBinaryOp(Runner::NodePtr& node) {
    std::cout << "visit binary\n";

    Parser::BinaryOp* ptr = static_cast<Parser::BinaryOp*>(node.get());
    auto computed_left = Visit(ptr->left);
    auto computed_right = Visit(ptr->right);
    if (computed_left && computed_right) {
        auto computed = Operators::ExecBinaryOperation(ptr, std::move(*computed_left), std::move(*computed_right));
        if (computed) {
            return std::move(*computed);
        } else {
            return std::unexpected(std::move(computed.error()));
        }
    } else {
        if (!computed_left) {
            // we need to create instance!
            return std::unexpected(std::move(computed_left.error()));
        }
        return std::unexpected(std::move(computed_right.error()));
    }
}
Runner::Expected Runner::VisitSubscript(Runner::NodePtr& node) {
    std::cout << "visit subscript\n";

    Parser::Subscript* ptr = static_cast<Parser::Subscript*>(node.get());

    Expected var_expr = Visit(ptr->var_expr);
    if (!var_expr) { return std::unexpected(var_expr.error()); }
    if (var_expr->type != TYPES::STRING_TYPE && var_expr->type != TYPES::LIST_TYPE) {
        return std::unexpected(Lexer::Token(
            Errors::TypeErrors::TypeErrorStringOrList(), node->token
        ));
    }

    if (!ptr->is_slice) {
        return SubscriptIndexer(ptr, std::move(*var_expr));
    } else {
        return SubscriptSlicer(ptr, std::move(*var_expr));
    }
}

/// BLOCKS
Runner::Expected Runner::VisitIf(Parser::NodePtr& node) {
    return std::unexpected(Lexer::Token(Errors::InternalErrors::NotImplemented(), node->token));
}
Runner::Expected Runner::VisitFor(Parser::NodePtr& node) {
    return std::unexpected(Lexer::Token(Errors::InternalErrors::NotImplemented(), node->token));
}
Runner::Expected Runner::VisitWhile(Parser::NodePtr& node) {
    return std::unexpected(Lexer::Token(Errors::InternalErrors::NotImplemented(), node->token));
}

/// CLOSURE STATEMENTS
Runner::Expected Runner::VisitReturn(Parser::NodePtr& node) {
    return std::unexpected(Lexer::Token(Errors::InternalErrors::NotImplemented(), node->token));
}
Runner::Expected Runner::VisitBreak(Parser::NodePtr& node) {
    return std::unexpected(Lexer::Token(Errors::InternalErrors::NotImplemented(), node->token));
}
Runner::Expected Runner::VisitContinue(Parser::NodePtr& node) {
    return std::unexpected(Lexer::Token(Errors::InternalErrors::NotImplemented(), node->token));
}

/// FUNCTIONS
Runner::Expected Runner::VisitFunc(Parser::NodePtr& node) {
    return std::unexpected(Lexer::Token(Errors::InternalErrors::NotImplemented(), node->token));
}
Runner::Expected Runner::VisitFuncCall(Parser::NodePtr& node) {
    return std::unexpected(Lexer::Token(Errors::InternalErrors::NotImplemented(), node->token));
}
Runner::Expected Runner::VisitCompound(Parser::NodePtr& node) {
    std::cout << "visit compound\n";

    Parser::Compound* cmpd = static_cast<Parser::Compound*>(node.get());
    for (auto& child : cmpd->data) {
        if (auto visited = Visit(child); !visited) {
            return std::unexpected(visited.error());
        }
    }
    return HolderPack();
}

/// HELPERS
Runner::Expected Runner::SubscriptIndexer(Parser::Subscript* ptr, HolderPack&& var) {
    auto index_computed = GetIndex(ptr->start, var);
    if (!index_computed) { return std::unexpected(index_computed.error()); }
    if (var.type == TYPES::STRING_TYPE) {
        return HolderPack{
            std::make_shared<HolderTypes>(
                std::string{std::get<std::string>(*var.holder)[*index_computed]}
            ),
            TYPES::STRING_TYPE
        };
    }
    return std::get<Memory::ListHolder>(*var.holder).data[*index_computed];
}

Runner::Expected Runner::SubscriptSlicer(Parser::Subscript* ptr, HolderPack&& var) {}

std::expected<int, Lexer::Token> Runner::GetIndex(NodePtr& node, HolderPack& var) {
    auto index = Visit(node);
    if (!index) { return std::unexpected(index.error()); }
    if (index->type != TYPES::NUM_TYPE) {
        return std::unexpected(Lexer::Token(
            Errors::TypeErrors::TypeErrorInt(), node->token
        ));
    }
    double raw_index = std::get<double>(*index->holder);
    if (raw_index != std::trunc(raw_index)) {
        return std::unexpected(Lexer::Token(
            Errors::TypeErrors::IndexNotInteger(), node->token
        ));
    }

    size_t var_size;
    if (var.type == TYPES::STRING_TYPE) {
        var_size = std::get<std::string>(*var.holder).size();
    } else {
        var_size = std::get<Memory::ListHolder>(*var.holder).data.size();
    }

    if (raw_index < 0) { raw_index = var_size + raw_index; }

    if (raw_index < 0 || raw_index >= var_size) {
        return std::unexpected(Lexer::Token(
            OutOfRange(), node->token
        ));
    }

    return raw_index;
}
