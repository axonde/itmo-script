#include "interpreter.h"
#include "built_in.h"
#include "operators.h"
#include <string>

using Memory::MakeListHolder;
using Memory::MakeFuncHolder;

std::istream* in;       // input stream of interpretator
std::ostream* out;      // output stream of interpretator
std::ostream* err;      // error stream of interpretator

// INTERPRETER
Interpreter::Interpreter(std::istream& i, std::ostream& o, std::ostream& e) {
    in = &i;
    out = &o;
    err = &e;
    stack_frame = std::make_unique<Memory::StackFrame>(std::move(BUILT_IN_FUNCTIONS), "global");
    Memory::stack_frame = stack_frame.get();
    BuiltIn::InitializeBuilInFunctions();
    Operators::RegisterUnaryOperators();
    Operators::RegisterBinaryOperators();
}

bool Interpreter::Interpret(std::istream& input, std::ostream& output, bool is_repl) {
    if (!is_repl) {
        return InterpretFile(input, output);
    }
    return InterpretRepl(input, output);
}

bool Interpreter::InterpretFile(std::istream& input, std::ostream& output) {
    // input.seekg(0, std::ios::end);
    // size_t size = input.tellg();
    // input.seekg(0);
    // std::string program(size, '\0');
    // input.read(&program[0], size);

    std::vector<std::string> program;
    std::string line;
    while (std::getline(input, line)) {
        program.push_back(line);
        try {
            tokenizer << line;
        } catch (const Closures::UncaughtClosure& c) { continue; }
        catch (const Closure& c) {
            Closures::PrintClosureError(c);
            return false;
        }
        catch (...) {
            Errors::PrintPanic(InternalError());
            return false;
        }
    }

    Parser parser(tokenizer);
    

}

// VISITERS
Interpreter::Expected Interpreter::Visit(Interpreter::NodePtr& node) {
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
            return MakeError<InternalError>(node);
    }
}


/// LITERALS
Interpreter::Expected Interpreter::VisitNumLiteral(Interpreter::NodePtr& node) {
    Parser::NumLiteral* num_literal = static_cast<Parser::NumLiteral*>(node.get());
    return HolderPack(num_literal->value, TYPES::NUM_TYPE);
}

Interpreter::Expected Interpreter::VisitStringLiteral(Interpreter::NodePtr& node) {
    Parser::StringLiteral* str_literal = static_cast<Parser::StringLiteral*>(node.get());
    return HolderPack(str_literal->value, TYPES::STRING_TYPE);
}

Interpreter::Expected Interpreter::VisitBoolLiteral(Interpreter::NodePtr& node) {
    Parser::BoolLiteral* bool_literal = static_cast<Parser::BoolLiteral*>(node.get());
    return HolderPack(bool_literal->value, TYPES::BOOL_TYPE);
}

Interpreter::Expected Interpreter::VisitNilLiteral(Interpreter::NodePtr& node) {
    return HolderPack(TYPES::NIL_TYPE);
}

/// LITERAL EXPR
Interpreter::Expected Interpreter::VisitVar(Interpreter::NodePtr& node) {
    std::string_view id = static_cast<Parser::Var*>(node.get())->id;
    try {
        return Interpreter::stack_frame->Lookup(id);
    } catch (Errors::MemoryErrors::NotFound e) {
        return MakeError<decltype(e)>(node);
    }
}
Interpreter::Expected Interpreter::VisitList(Interpreter::NodePtr& node) {
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
    return { HolderPack(MakeListHolder(std::move(data)), TYPES::LIST_TYPE) };
}

/// OPERATIONS
Interpreter::Expected Interpreter::VisitUnaryOp(Interpreter::NodePtr& node) {
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
Interpreter::Expected Interpreter::VisitBinaryOp(Interpreter::NodePtr& node) {
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
            return std::unexpected(std::move(computed_left.error()));
        }
        return std::unexpected(std::move(computed_right.error()));
    }
}
Interpreter::Expected Interpreter::VisitSubscript(Interpreter::NodePtr& node) {
    Parser::Subscript* ptr = static_cast<Parser::Subscript*>(node.get());

    Expected var_expr_exp = Visit(ptr->var_expr);
    if (!var_expr_exp) { return std::unexpected(var_expr_exp.error()); }
    HolderPack& var_expr = *var_expr_exp;
    if (var_expr->type != TYPES::STRING_TYPE && var_expr->type != TYPES::LIST_TYPE) {
        return MakeError<Errors::TypeErrors::TypeErrorStringOrList>(node);
    }

    if (!ptr->is_slice) {
        return SubscriptIndexer(ptr, std::move(var_expr));
    } else {
        return SubscriptSlicer(ptr, std::move(var_expr));
    }
}

/// BLOCKS
Interpreter::Expected Interpreter::VisitIf(Parser::NodePtr& node) {
    Parser::If* ptr = static_cast<Parser::If*>(node.get());

    for (Parser::If::IfCase& occasion : ptr->cases) {
        auto condition_expected = Visit(occasion.condition);
        if (!condition_expected) { return std::unexpected(condition_expected.error()); }

        HolderPack condition = *condition_expected;
        if (std::get<bool>(condition->holder)) {
            if (auto expected = Visit(occasion.body); !expected) {
                return std::unexpected(expected.error());
            }
            break;
        }
    }
    return HolderPack();
}
Interpreter::Expected Interpreter::VisitFor(Parser::NodePtr& node) {
    auto ptr = static_cast<Parser::For*>(node.get());
    auto iterator_expected = Visit(ptr->iterator);
    if (!iterator_expected) { return std::unexpected(iterator_expected.error()); }
    auto range_expected = Visit(ptr->range);
    if (!range_expected) { return std::unexpected(range_expected.error()); }

    HolderPack iterator = *iterator_expected;
    HolderPack range = *range_expected;

    if (range->type != TYPES::STRING_TYPE && range->type != TYPES::LIST_TYPE) {
        return MakeError<Errors::RunTime::NotEvaluatedSequence>(node);
    }
    size_t size;
    if (range->type == TYPES::STRING_TYPE) { size = std::get<std::string>(range->holder).size(); }
    else { size = std::get<ListHolderPtr>(range->holder)->data.size(); }

    for (size_t i = 0; i < size; ++i) {
        try {
            if (range->type == TYPES::STRING_TYPE) {
                iterator_expected = Operators::RawExecBinaryOperation(
                    Lexer::Tokens::T_EQUAL,
                    HolderPack(iterator),
                    HolderPack(std::to_string(std::get<std::string>(range->holder)[i]), TYPES::STRING_TYPE)
                );
            } else {
                iterator_expected = Operators::RawExecBinaryOperation(
                    Lexer::Tokens::T_EQUAL,
                    HolderPack(iterator),
                    HolderPack(std::get<ListHolderPtr>(range->holder)->data[i])
                );
            }
        } catch (const Error& e) {
            return MakeError(e, node);
        }
        try {
            if (auto visited = Visit(ptr->body); !visited) {
                return std::unexpected(visited.error());
            }
        }
        catch (Closures::Break&) { break; }
        catch (Closures::Continue&) { continue; }
    }
    return HolderPack();
}
Interpreter::Expected Interpreter::VisitWhile(Parser::NodePtr& node) {
    auto ptr = static_cast<Parser::While*>(node.get());

    Interpreter::Expected condition_expected;
    while (true) {
        condition_expected = Visit(ptr->condition);
        if (!condition_expected) { return std::unexpected(condition_expected.error()); }
        auto condition = *condition_expected;
        if (std::get<bool>(condition->holder)) {
            try {
                if (auto visited = Visit(ptr->body); !visited) {
                    return std::unexpected(visited.error());
                }
            }
            catch (Closures::Break&) { break; }
            catch (Closures::Continue&) {}  // continue too.
            continue;
        }
        break;
    }
    return HolderPack();
}

/// CLOSURE STATEMENTS
Interpreter::Expected Interpreter::VisitReturn(Parser::NodePtr& node) {
    Parser::Return* ptr = static_cast<Parser::Return*>(node.get());
    auto expr_expected = Visit(ptr->expr);
    if (!expr_expected) { return std::unexpected(expr_expected.error()); }
    throw Closures::Return(std::move(*expr_expected), node->token);
}
Interpreter::Expected Interpreter::VisitBreak(Parser::NodePtr& node) {
    throw Closures::Break(node->token);
}
Interpreter::Expected Interpreter::VisitContinue(Parser::NodePtr& node) {
    throw Closures::Continue(node->token);
}

/// FUNCTIONS
Interpreter::Expected Interpreter::VisitFunc(Parser::NodePtr& node) {
    return HolderPack( MakeFuncHolder(node.get()), TYPES::FUNC_TYPE );
}
Interpreter::Expected Interpreter::VisitFuncCall(Parser::NodePtr& node) {
    auto ptr = static_cast<Parser::FuncCall*>(node.get());
    auto func_expected = Visit(ptr->func);
    if (!func_expected) { return std::unexpected(func_expected.error()); }
    HolderPack& func = *func_expected;
    if (func->type != TYPES::FUNC_TYPE) {
        return MakeError<Errors::TypeErrors::TypeErrorFunc>(node);
    }

    std::vector<HolderPack> params;
    for (auto& param : ptr->params) {
        auto visited_expected = Visit(param);
        if (!visited_expected) { return std::unexpected(visited_expected.error()); }
        HolderPack& visited = *visited_expected;
        if (visited->type == TYPES::NOT_SET_TYPE) {
            return MakeError<Errors::MemoryErrors::NotFound>(param);
        }
        params.push_back(visited);
    }

    FuncHolder& function_holder = *std::get<FuncHolderPtr>(func->holder);
    if (std::holds_alternative<std::any>(function_holder.function)) {
        return VisitUserFuncCall(ptr, function_holder, params);
    }
    return VisitBuiltInFuncCall(ptr, function_holder, params);
}
Interpreter::Expected Interpreter::VisitUserFuncCall(Parser::FuncCall* ptr, FuncHolder& function_holder, std::vector<HolderPack>& params) {
    std::string func_name = "<anonimous function>";
    if (ptr->func->node == Parser::N_VAR) func_name = static_cast<Parser::Var*>(ptr->func.get())->id;
    Interpreter::stack_frame = std::make_unique<Memory::StackFrame>(std::move(Interpreter::stack_frame), std::move(func_name));

    Parser::Func* func_instance = static_cast<Parser::Func*>(std::any_cast<Parser::Node*>(
        std::get<std::any>(function_holder.function)
    ));
    if (func_instance->args.size() != ptr->params.size()) {
        return MakeError<Errors::RunTime::WrongArgumentCount>(ptr);
    }

    for (size_t i = 0; i != func_instance->args.size(); ++i) {
        HolderPack hp = Interpreter::stack_frame->Lookup(func_instance->args[i].id);
        hp->holder = std::move(params[i]->holder);
        hp->type = std::move(params[i]->type);
    }

    HolderPack result = HolderPack(TYPES::NIL_TYPE);
    try {
        if (auto visited = Visit(func_instance->body); !visited) {
            return std::unexpected(visited.error());
        }
    } catch (Closures::Return& r) {
        result = std::move(std::any_cast<HolderPack&>(r.holder_pack));
    }

    Interpreter::stack_frame = std::move(Interpreter::stack_frame->parent);
    return result;
}
Interpreter::Expected Interpreter::VisitBuiltInFuncCall(Parser::FuncCall* ptr, Memory::FuncHolder& function_holder, std::vector<HolderPack>& params) {
    auto& function = std::get<Memory::BuiltInFunction>(function_holder.function);
    try {
        return function(std::move(params));
    } catch (const Error& e) {
        return MakeError(e, ptr);
    }
}

Interpreter::Expected Interpreter::VisitCompound(Parser::NodePtr& node) {
    Parser::Compound* cmpd = static_cast<Parser::Compound*>(node.get());
    for (auto& child : cmpd->data) {
        if (auto visited = Visit(child); !visited) {
            return std::unexpected(visited.error());
        }
    }
    return HolderPack();
}


/// HELPERS

std::unexpected<Error> MakeError(const Error& e, Parser::Node* node) {
    return std::unexpected( Error(e.what(), node->token.lineno, node->token.column) );
}
std::unexpected<Error> MakeError(const Error& e, Parser::NodePtr& node) {
    return std::unexpected( Error(e.what(), node->token.lineno, node->token.column) );
}

template<typename E>
requires std::derived_from<E, Error>
std::unexpected<Error> MakeError(Parser::NodePtr& node) {
    return { E(node->token.lineno, node->token.column) };
}

template<typename E>
requires std::derived_from<E, Error>
std::unexpected<Error> MakeError(Parser::Node* node) {
    return { E(node->token.lineno, node->token.column) };
}

Interpreter::Expected Interpreter::SubscriptIndexer(Parser::Subscript* ptr, HolderPack&& var) {
    auto index_computed = GetIndex(ptr->start, var);
    if (!index_computed) { return std::unexpected(index_computed.error()); }
    if (var->type == TYPES::STRING_TYPE) {
        return HolderPack(
            std::string{std::get<std::string>(var->holder)[*index_computed]},
            TYPES::STRING_TYPE
        ); }
    return std::get<ListHolderPtr>(var->holder)->data[*index_computed];
}

Interpreter::Expected Interpreter::SubscriptSlicer(Parser::Subscript* ptr, HolderPack&& var) {
    auto start_expected = (ptr->start) ? IntegerRequirement(ptr->start) : 0;
    if (!start_expected) { return std::unexpected(start_expected.error()); }
    auto step_expected = (ptr->step) ? IntegerRequirement(ptr->step) : 1;
    if (!step_expected) { return std::unexpected(step_expected.error()); }
    if (*step_expected == 0) {
        return MakeError<Errors::RunTime::ZeroStep>(ptr);
    }

    int size;
    if (var->type == TYPES::STRING_TYPE) { size = std::get<std::string>(var->holder).size(); }
    else { size = std::get<ListHolderPtr>(var->holder)->data.size(); }

    auto end_expected = (ptr->end) ? IntegerRequirement(ptr->end) : size;
    if (!end_expected) { return std::unexpected(end_expected.error()); }

    int start = *start_expected;
    int end = *end_expected;
    int step = *step_expected;

    if (step < 0) {
        std::swap(start, end); 
        if (!ptr->start) { --end; }
        if (!ptr->end) { --start; }
    }
    auto condition = [&](int i) {
        return (i < end && step > 0) || (i > end && step < 0);
    };

    if (var->type == TYPES::STRING_TYPE) {
        std::string str;
        for (int i = start; i >= 0 && i < size && condition(i); i += step) {
            str += std::get<std::string>(var->holder)[i];
        }
        return HolderPack(std::move(str), TYPES::STRING_TYPE);
    } else {
        std::vector<HolderPack> list;
        for (int i = start; i >= 0 && i < size && condition(i); i += step) {
            list.push_back(std::get<Memory::ListHolderPtr>(var->holder)->data[i]);
        }
        return HolderPack(MakeListHolder(std::move(list)), TYPES::LIST_TYPE);
    }
}

std::expected<int, Error> Interpreter::IntegerRequirement(NodePtr& node) {
    auto index_expected = Visit(node);
    if (!index_expected) { return std::unexpected(index_expected.error()); }
    HolderPack& index = *index_expected;
    if (index->type != TYPES::NUM_TYPE) {
        return MakeError<Errors::TypeErrors::TypeErrorNum>(node);
    }
    double raw_index = std::get<double>(index->holder);
    if (raw_index != std::trunc(raw_index)) {
        return MakeError<Errors::TypeErrors::IndexNotInteger>(node);
    }
    return raw_index;
}

std::expected<int, Error> Interpreter::GetIndex(NodePtr& node, HolderPack& var) {
    auto raw_index = IntegerRequirement(node);
    if (!raw_index) { return std::unexpected(raw_index.error()); }
    int index = *raw_index;

    size_t var_size;
    if (var->type == TYPES::STRING_TYPE) {
        var_size = std::get<std::string>(var->holder).size();
    }
    else { var_size = std::get<Memory::ListHolderPtr>(var->holder)->data.size(); }
    if (index < 0) { index = var_size + index; }

    if (index < 0 || index >= var_size) {
        return MakeError<OutOfRange>(node);
    }
    return index;
}

