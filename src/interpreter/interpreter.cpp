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
        } catch (...) {
            Errors::PrintPanic(InternalError());
            return false;
        }
    }

    return RunSafely([&]() {
        Parser parser(std::move(tokenizer));
        parser.Parse();
    }, program);
}
bool Interpreter::InterpretRepl(std::istream& input, std::ostream& output) {
    // TODO do it)
    Errors::PrintPanic(Errors::InternalErrors::NotImplemented());
    return false;
}

// VISITERS
Interpreter::HolderPack Interpreter::Visit(Interpreter::NodePtr& node) {
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
            throw MakeError<InternalError>(node);
    }
}


/// LITERALS
Interpreter::HolderPack Interpreter::VisitNumLiteral(Interpreter::NodePtr& node) {
    Parser::NumLiteral* num_literal = static_cast<Parser::NumLiteral*>(node.get());
    return HolderPack(num_literal->value, TYPES::NUM_TYPE);
}

Interpreter::HolderPack Interpreter::VisitStringLiteral(Interpreter::NodePtr& node) {
    Parser::StringLiteral* str_literal = static_cast<Parser::StringLiteral*>(node.get());
    return HolderPack(str_literal->value, TYPES::STRING_TYPE);
}

Interpreter::HolderPack Interpreter::VisitBoolLiteral(Interpreter::NodePtr& node) {
    Parser::BoolLiteral* bool_literal = static_cast<Parser::BoolLiteral*>(node.get());
    return HolderPack(bool_literal->value, TYPES::BOOL_TYPE);
}

Interpreter::HolderPack Interpreter::VisitNilLiteral(Interpreter::NodePtr& node) {
    return HolderPack(TYPES::NIL_TYPE);
}

/// LITERAL EXPR
Interpreter::HolderPack Interpreter::VisitVar(Interpreter::NodePtr& node) {
    std::string_view id = static_cast<Parser::Var*>(node.get())->id;
    try {
        return Interpreter::stack_frame->Lookup(id);
    } catch (Errors::MemoryErrors::NotFound e) {
        throw MakeError<decltype(e)>(node);
    } catch (...) {
        throw MakeError<InternalError>(node);
    }
}
Interpreter::HolderPack Interpreter::VisitList(Interpreter::NodePtr& node) {
    Parser::List* list = static_cast<Parser::List*>(node.get());
    std::vector<HolderPack> data;
    for (auto& child : list->data) {
        auto visited = Visit(child);
        data.push_back(std::move(visited));
    }
    return { HolderPack(MakeListHolder(std::move(data)), TYPES::LIST_TYPE) };
}

/// OPERATIONS
Interpreter::HolderPack Interpreter::VisitUnaryOp(Interpreter::NodePtr& node) {
    Parser::UnaryOp* ptr = static_cast<Parser::UnaryOp*>(node.get());
    auto computed_operand = Visit(ptr->operand);
    return Operators::ExecUnaryOperation(ptr, std::move(computed_operand));
}
Interpreter::HolderPack Interpreter::VisitBinaryOp(Interpreter::NodePtr& node) {
    Parser::BinaryOp* ptr = static_cast<Parser::BinaryOp*>(node.get());
    auto computed_left = Visit(ptr->left);
    auto computed_right = Visit(ptr->right);
    auto computed = Operators::ExecBinaryOperation(ptr, std::move(computed_left), std::move(computed_right));
    return std::move(computed);
}
Interpreter::HolderPack Interpreter::VisitSubscript(Interpreter::NodePtr& node) {
    Parser::Subscript* ptr = static_cast<Parser::Subscript*>(node.get());

    HolderPack var_expr = Visit(ptr->var_expr);
    if (var_expr->type != TYPES::STRING_TYPE && var_expr->type != TYPES::LIST_TYPE) {
        throw MakeError<Errors::TypeErrors::TypeErrorStringOrList>(node);
    }

    if (!ptr->is_slice) {
        return SubscriptIndexer(ptr, std::move(var_expr));
    } else {
        return SubscriptSlicer(ptr, std::move(var_expr));
    }
}

/// BLOCKS
Interpreter::HolderPack Interpreter::VisitIf(Parser::NodePtr& node) {
    Parser::If* ptr = static_cast<Parser::If*>(node.get());
    for (Parser::If::IfCase& occasion : ptr->cases) {
        HolderPack condition = Visit(occasion.condition);
        if (std::get<bool>(condition->holder)) {
            Visit(occasion.body);
            break;
        }
    }
    return HolderPack();
}
Interpreter::HolderPack Interpreter::VisitFor(Parser::NodePtr& node) {
    auto ptr = static_cast<Parser::For*>(node.get());
    auto iterator = Visit(ptr->iterator);
    auto range = Visit(ptr->range);

    if (range->type != TYPES::STRING_TYPE && range->type != TYPES::LIST_TYPE) {
        throw MakeError<Errors::RunTime::NotEvaluatedSequence>(node);
    }
    size_t size;
    if (range->type == TYPES::STRING_TYPE) { size = std::get<std::string>(range->holder).size(); }
    else { size = std::get<ListHolderPtr>(range->holder)->data.size(); }

    for (size_t i = 0; i < size; ++i) {
        if (range->type == TYPES::STRING_TYPE) {
            iterator = Operators::RawExecBinaryOperation(
                Lexer::Tokens::T_EQUAL,
                HolderPack(iterator),
                HolderPack(std::to_string(std::get<std::string>(range->holder)[i]), TYPES::STRING_TYPE)
            );
        } else {
            iterator = Operators::RawExecBinaryOperation(
                Lexer::Tokens::T_EQUAL,
                HolderPack(iterator),
                HolderPack(std::get<ListHolderPtr>(range->holder)->data[i])
            );
        }
        try { Visit(ptr->body); }
        catch (Closures::Break&) { break; }
        catch (Closures::Continue&) { continue; }
    }
    return HolderPack();
}
Interpreter::HolderPack Interpreter::VisitWhile(Parser::NodePtr& node) {
    auto ptr = static_cast<Parser::While*>(node.get());

    Interpreter::HolderPack condition_expected;
    while (true) {
        auto condition = Visit(ptr->condition);
        if (std::get<bool>(condition->holder)) {
            try { Visit(ptr->body); }
            catch (Closures::Break&) { break; }
            catch (Closures::Continue&) { /*literally continue*/ }
            continue;
        }
        break;
    }
    return HolderPack();
}

/// CLOSURE STATEMENTS
Interpreter::HolderPack Interpreter::VisitReturn(Parser::NodePtr& node) {
    Parser::Return* ptr = static_cast<Parser::Return*>(node.get());
    auto expr = Visit(ptr->expr);
    throw Closures::Return(std::move(expr), node->token.lineno, node->token.column);
}
Interpreter::HolderPack Interpreter::VisitBreak(Parser::NodePtr& node) {
    throw Closures::Break(node->token.lineno, node->token.column);
}
Interpreter::HolderPack Interpreter::VisitContinue(Parser::NodePtr& node) {
    throw Closures::Continue(node->token.lineno, node->token.column);
}

/// FUNCTIONS
Interpreter::HolderPack Interpreter::VisitFunc(Parser::NodePtr& node) {
    return HolderPack( MakeFuncHolder(node.get()), TYPES::FUNC_TYPE );
}
Interpreter::HolderPack Interpreter::VisitFuncCall(Parser::NodePtr& node) {
    auto ptr = static_cast<Parser::FuncCall*>(node.get());
    HolderPack func = Visit(ptr->func);
    if (func->type != TYPES::FUNC_TYPE) {
        throw MakeError<Errors::TypeErrors::TypeErrorFunc>(node);
    }

    std::vector<HolderPack> params;
    for (auto& param : ptr->params) {
        auto visited = Visit(param);
        if (visited->type == TYPES::NOT_SET_TYPE) {
            throw MakeError<Errors::MemoryErrors::NotFound>(param);
        }
        params.push_back(visited);
    }

    FuncHolder& function_holder = *std::get<FuncHolderPtr>(func->holder);
    if (std::holds_alternative<std::any>(function_holder.function)) {
        return VisitUserFuncCall(ptr, function_holder, params);
    }
    return VisitBuiltInFuncCall(ptr, function_holder, params);
}
Interpreter::HolderPack Interpreter::VisitUserFuncCall(Parser::FuncCall* ptr, FuncHolder& function_holder, std::vector<HolderPack>& params) {
    std::string func_name = "<anonimous function>";
    if (ptr->func->node == Parser::N_VAR) func_name = static_cast<Parser::Var*>(ptr->func.get())->id;
    Interpreter::stack_frame = std::make_unique<Memory::StackFrame>(std::move(Interpreter::stack_frame), std::move(func_name));

    Parser::Func* func_instance = static_cast<Parser::Func*>(std::any_cast<Parser::Node*>(
        std::get<std::any>(function_holder.function)
    ));
    if (func_instance->args.size() != ptr->params.size()) {
        throw MakeError<Errors::RunTime::WrongArgumentCount>(ptr);
    }

    for (size_t i = 0; i != func_instance->args.size(); ++i) {
        HolderPack hp = Interpreter::stack_frame->Lookup(func_instance->args[i].id);
        hp->holder = std::move(params[i]->holder);
        hp->type = std::move(params[i]->type);
    }

    HolderPack result = HolderPack(TYPES::NIL_TYPE);
    try {
        Visit(func_instance->body);
    } catch (Closures::Return& r) {
        result = std::move(std::any_cast<HolderPack&>(r.holder_pack));
    }

    Interpreter::stack_frame = std::move(Interpreter::stack_frame->parent);
    return result;
}
Interpreter::HolderPack Interpreter::VisitBuiltInFuncCall(Parser::FuncCall* ptr, Memory::FuncHolder& function_holder, std::vector<HolderPack>& params) {
    auto& function = std::get<Memory::BuiltInFunction>(function_holder.function);
    return function(std::move(params));
}

Interpreter::HolderPack Interpreter::VisitCompound(Parser::NodePtr& node) {
    Parser::Compound* cmpd = static_cast<Parser::Compound*>(node.get());
    for (auto& child : cmpd->data) {
        Visit(child);
    }
    return HolderPack();
}


/// HELPERS
template<typename E>
requires std::derived_from<E, Error>
E MakeError(Parser::NodePtr& node) {
    return E(node->token.lineno, node->token.column);
}

template<typename E>
requires std::derived_from<E, Error>
E MakeError(Parser::Node* node) {
    return E(node->token.lineno, node->token.column);
}

template<typename Func>
requires std::invocable<Func>
bool Interpreter::RunSafely(Func&& func, std::vector<std::string>& program) {

    auto stacktrace = [&]() {
        *err << Memory::StackFrame::PrintStack(*Memory::stack_frame);
    };

    try {
        func();
    } catch (const Closure& c) {
        stacktrace();
        Closures::PrintClosureError(c);
        Errors::PrintProgramSnippet(program, c.lineno, c.column);
        return false;
    } catch (const Errors::LexerErrors::LexerError& e) {
        stacktrace();
        Errors::PrintSyntaxError(e);
        Errors::PrintProgramSnippet(program, e.lineno, e.column);
        return false;
    } catch (const Errors::ParserErrors::ParserError& e) {
        stacktrace();
        Errors::PrintSyntaxError(e);
        Errors::PrintProgramSnippet(program, e.lineno, e.column);
        return false;
    } catch (const Errors::OperatorErrors::OperatorError& e) {
        stacktrace();
        Errors::PrintOperatorError(e);
        Errors::PrintProgramSnippet(program, e.lineno, e.column);
        return false;
    } catch (const Errors::MemoryErrors::MemoryError& e) {
        stacktrace();
        Errors::PrintRunTimeError(e);
        Errors::PrintProgramSnippet(program, e.lineno, e.column);
        return false;
    } catch (const Errors::TypeErrors::TypeError& e) {
        stacktrace();
        Errors::PrintTypeError(e);
        Errors::PrintProgramSnippet(program, e.lineno, e.column);
        return false;
    } catch (const Errors::RunTime::RunTimeError& e) {
        stacktrace();
        Errors::PrintRunTimeError(e);
        Errors::PrintProgramSnippet(program, e.lineno, e.column);
        return false;
    } catch (const Errors::InternalErrors::InternalError& e) {
        stacktrace();
        Errors::PrintPanic(e);
        return false;
    } catch (...) {
        stacktrace();
        Errors::PrintPanic(InternalError());
        return false;
    }

    return true;
}

Interpreter::HolderPack Interpreter::SubscriptIndexer(Parser::Subscript* ptr, HolderPack&& var) {
    auto index = GetIndex(ptr->start, var);
    if (var->type == TYPES::STRING_TYPE) {
        return HolderPack(
            std::string{std::get<std::string>(var->holder)[index]},
            TYPES::STRING_TYPE
        ); }
    return std::get<ListHolderPtr>(var->holder)->data[index];
}

Interpreter::HolderPack Interpreter::SubscriptSlicer(Parser::Subscript* ptr, HolderPack&& var) {
    auto start = (ptr->start) ? IntegerRequirement(ptr->start) : 0;
    auto step = (ptr->step) ? IntegerRequirement(ptr->step) : 1;
    if (step == 0) {
        throw MakeError<Errors::RunTime::ZeroStep>(ptr);
    }

    int size;
    if (var->type == TYPES::STRING_TYPE) { size = std::get<std::string>(var->holder).size(); }
    else { size = std::get<ListHolderPtr>(var->holder)->data.size(); }

    auto end = (ptr->end) ? IntegerRequirement(ptr->end) : size;

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

int64_t Interpreter::IntegerRequirement(NodePtr& node) {
    HolderPack index = Visit(node);
    if (index->type != TYPES::NUM_TYPE) {
        throw MakeError<Errors::TypeErrors::TypeErrorNum>(node);
    }
    double raw_index = std::get<double>(index->holder);
    if (raw_index != std::trunc(raw_index)) {
        throw MakeError<Errors::TypeErrors::IndexNotInteger>(node);
    }
    return raw_index;
}

int64_t Interpreter::GetIndex(NodePtr& node, HolderPack& var) {
    int64_t index = IntegerRequirement(node);

    size_t var_size;
    if (var->type == TYPES::STRING_TYPE) {
        var_size = std::get<std::string>(var->holder).size();
    }
    else { var_size = std::get<Memory::ListHolderPtr>(var->holder)->data.size(); }
    if (index < 0) { index = var_size + index; }

    if (index < 0 || index >= var_size) {
        throw MakeError<OutOfRange>(node);
    }
    return index;
}

