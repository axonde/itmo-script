#include "interpreter.h"

namespace Interpreter {

/// CONFIG
std::unique_ptr<Memory::StackFrame> stack_frame;
std::istream* in;
std::ostream* out;

void Init() {
    BuiltIn::InitializeBuilInFunctions();
    stack_frame = std::make_unique<Memory::StackFrame>(std::move(BUILT_IN_FUNCTIONS), "global");
    Operators::RegisterUnaryOperators();
    Operators::RegisterBinaryOperators();
}

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
    // IsRepl is not the Interpret deal.
    Interpreter::in = &input;
    Interpreter::out = &output;
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

    // reorganize readness from file. it it not interpreter work.
    size_t size = input.tellg();
    input.seekg(0);
    std::string program(size, '\0');
    input.read(&program[0], size);

    return Interpret(program, std::cin, std::cout);
}

bool Interpret(std::string& program, std::istream& input, std::ostream& output) {
    Interpreter::in = &input;
    Interpreter::out = &output;
    Init();  // точка входа! без нее все летит!
    Runner runner(std::move(program));

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
    return { std::make_shared<RawHolderPack>(num_literal->value, TYPES::NUM_TYPE) };
}

Runner::Expected Runner::VisitStringLiteral(Runner::NodePtr& node) {
    std::cout << "visit str literal\n";

    Parser::StringLiteral* str_literal = static_cast<Parser::StringLiteral*>(node.get());
    return { std::make_shared<RawHolderPack>(str_literal->value, TYPES::STRING_TYPE) };
}

Runner::Expected Runner::VisitBoolLiteral(Runner::NodePtr& node) {
    std::cout << "visit bool literal\n";

    Parser::BoolLiteral* bool_literal = static_cast<Parser::BoolLiteral*>(node.get());
    return { std::make_shared<RawHolderPack>(bool_literal->value, TYPES::BOOL_TYPE) };
}

Runner::Expected Runner::VisitNilLiteral(Runner::NodePtr& node) {
    std::cout << "visit nil literal\n";

    return { std::make_shared<RawHolderPack>(TYPES::NIL_TYPE) };
}

/// LITERAL EXPR
Runner::Expected Runner::VisitVar(Runner::NodePtr& node) {
    std::cout << "visit var\n";

    std::string_view id = static_cast<Parser::Var*>(node.get())->id;
    try {
        return Interpreter::stack_frame->Lookup(id);
    } catch (Errors::MemoryErrors::NotFound e) {
        return std::unexpected(Lexer::Token(std::move(e), node->token));
     } //catch (...) {
    //     std::cout << "gabella"
    //     return std::unexpected(Lexer::Token(InternalError(), node->token));
    // }
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
    return { std::make_shared<RawHolderPack>(
        Memory::MakeList(std::move(data)),
        TYPES::LIST_TYPE
    ) };
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

    Expected var_expr_exp = Visit(ptr->var_expr);
    if (!var_expr_exp) { return std::unexpected(var_expr_exp.error()); }
    HolderPack& var_expr = *var_expr_exp;
    if (var_expr->type != TYPES::STRING_TYPE && var_expr->type != TYPES::LIST_TYPE) {
        return std::unexpected(Lexer::Token(
            Errors::TypeErrors::TypeErrorStringOrList(), node->token
        ));
    }

    if (!ptr->is_slice) {
        return SubscriptIndexer(ptr, std::move(var_expr));
    } else {
        return SubscriptSlicer(ptr, std::move(var_expr));
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
    std::cout << "visit func declaration\n";

    return Memory::MakeHolderPack( Memory::MakeFunc(node.get()), TYPES::FUNC_TYPE );
}
Runner::Expected Runner::VisitFuncCall(Parser::NodePtr& node) {
    std::cout << "visit func call\n";

    auto ptr = static_cast<Parser::FuncCall*>(node.get());
    auto func_expected = Visit(ptr->func);
    if (!func_expected) { return std::unexpected(func_expected.error()); }
    HolderPack& func = *func_expected;
    if (func->type != TYPES::FUNC_TYPE) {
        return std::unexpected(Lexer::Token(Errors::TypeErrors::ExpectedFuncType(), node->token));
    }

    std::vector<HolderPack> params;
    for (auto& param : ptr->params) {
        auto visited_expected = Visit(param);
        if (!visited_expected) { return std::unexpected(visited_expected.error()); }
        HolderPack& visited = *visited_expected;
        if (visited->type == TYPES::NOT_SET_TYPE) { return std::unexpected(Lexer::Token(
            Errors::MemoryErrors::NotFound(), std::move(param->token)
        )); }
        params.push_back(visited);
    }

    Memory::FuncHolder& function_holder = *std::get<Memory::FuncHolderPtr>(func->holder);
    if (std::holds_alternative<std::any>(function_holder.function)) {
        return VisitUserFuncCall(ptr, function_holder, params);
    }
    return VisitBuiltInFuncCall(ptr, function_holder, params);
}
Runner::Expected Runner::VisitUserFuncCall(Parser::FuncCall* ptr, Memory::FuncHolder& function_holder, std::vector<HolderPack>& params) {
    std::string func_name = "<anonimous function>";
    if (ptr->func->node == Parser::N_VAR) func_name = static_cast<Parser::Var*>(ptr->func.get())->id;
    Interpreter::stack_frame = std::make_unique<Memory::StackFrame>(std::move(Interpreter::stack_frame), std::move(func_name));

    Parser::Func* func_instance = std::move(
        static_cast<Parser::Func*>(std::any_cast<Parser::Node*>(
            std::get<std::any>(function_holder.function)
        ))
    );
    if (func_instance->args.size() != ptr->params.size()) { return std::unexpected(Lexer::Token(
        Errors::RunTime::WrongArgumentCount(), ptr->token
    )); }

    for (size_t i = 0; i != func_instance->args.size(); ++i) {
        HolderPack hp = Interpreter::stack_frame->Lookup(func_instance->args[i].id);
        hp->holder = std::move(params[i]->holder);
        hp->type = std::move(params[i]->type);
    }

    HolderPack result = Memory::MakeHolderPack(TYPES::NIL_TYPE);
    try {
        Visit(func_instance->body);
    } catch (const Errors::RunTime::UncaughtReturn& e) {
        result = std::any_cast<HolderPack>(e.holder_pack);
    }
    
    Interpreter::stack_frame = std::move(Interpreter::stack_frame->parent);
    return result;
}
Runner::Expected Runner::VisitBuiltInFuncCall(Parser::FuncCall* ptr, Memory::FuncHolder& function_holder, std::vector<HolderPack>& params) {
    auto& function = std::get<Memory::BuiltInFunction>(function_holder.function);
    try {
        return function(std::move(params));
    } catch (const Error& e) {
        return std::unexpected(Lexer::Token(Error(e.what()), ptr->token));
    }
}

Runner::Expected Runner::VisitCompound(Parser::NodePtr& node) {
    std::cout << "visit compound\n";

    Parser::Compound* cmpd = static_cast<Parser::Compound*>(node.get());
    for (auto& child : cmpd->data) {
        if (auto visited = Visit(child); !visited) {
            return std::unexpected(visited.error());
        }
    }
    return std::make_shared<RawHolderPack>();
}


/// HELPERS
Runner::Expected Runner::SubscriptIndexer(Parser::Subscript* ptr, HolderPack&& var) {
    auto index_computed = GetIndex(ptr->start, var);
    if (!index_computed) { return std::unexpected(index_computed.error()); }
    if (var->type == TYPES::STRING_TYPE) {
        return std::make_shared<RawHolderPack>(
            std::string{std::get<std::string>(var->holder)[*index_computed]},
            TYPES::STRING_TYPE
        ); };
    return std::get<Memory::ListHolderPtr>(var->holder)->data[*index_computed];
}

Runner::Expected Runner::SubscriptSlicer(Parser::Subscript* ptr, HolderPack&& var) {
    auto start_expected = (ptr->start) ? IntegerRequirement(ptr->start) : 0;
    if (!start_expected) { return std::unexpected(start_expected.error()); }
    auto step_expected = (ptr->step) ? IntegerRequirement(ptr->step) : 1;
    if (!step_expected) { return std::unexpected(step_expected.error()); }
    if (*step_expected == 0) { return std::unexpected(Lexer::Token(
        Errors::RunTime::SliceStep(), ptr->token
    )); }

    int size;
    if (var->type == TYPES::STRING_TYPE) { size = std::get<std::string>(var->holder).size(); }
    else { size = std::get<Memory::ListHolderPtr>(var->holder)->data.size(); }
    
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
        return Memory::MakeHolderPack(std::move(str), TYPES::STRING_TYPE);
    } else {
        std::vector<HolderPack> list;
        for (int i = start; i >= 0 && i < size && condition(i); i += step) {
            list.push_back(std::get<Memory::ListHolderPtr>(var->holder)->data[i]);
        }
        return Memory::MakeHolderPack(Memory::MakeList(std::move(list)), TYPES::LIST_TYPE);
    }
}

std::expected<int, Lexer::Token> Runner::IntegerRequirement(NodePtr& node) {
    auto index_expected = Visit(node);
    if (!index_expected) { return std::unexpected(index_expected.error()); }
    HolderPack& index = *index_expected;
    if (index->type != TYPES::NUM_TYPE) {
        return std::unexpected(Lexer::Token(
            Errors::TypeErrors::TypeErrorInt(), node->token
        ));
    }
    double raw_index = std::get<double>(index->holder);
    if (raw_index != std::trunc(raw_index)) {
        return std::unexpected(Lexer::Token(
            Errors::TypeErrors::IndexNotInteger(), node->token
        ));
    }
    return raw_index;
}

std::expected<int, Lexer::Token> Runner::GetIndex(NodePtr& node, HolderPack& var) {
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
        return std::unexpected(Lexer::Token( OutOfRange(), node->token ));
    }
    return index;
}
