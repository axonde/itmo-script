#include "interpreter.h"

bool Interpreter::Interpret(std::istream& input, std::ostream& output, bool IsRepl = false) {
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

bool Interpreter::Interpret(std::string& program, std::ostream& output) {
    Runner runner(std::move(program), output);

    if (auto expected = runner.Run(); !expected) {
        SyntaxError(std::move(expected.error()));
        return false;
    }
    return true;
}

Runner::Expected Runner::Run() {
    Operators::RegisterUnaryOperators();
    Operators::RegisterBinaryOperators();
    return Visit(parser.root);
}

/// LITERALS
Runner::Expected Runner::VisitNumLiteral(Runner::NodePtr& node) {
    std::cout << "visit num literal\n";

    return static_cast<Parser::NumLiteral*>(node.get())->value;
}
Runner::Expected Runner::VisitStringLiteral(Runner::NodePtr& node) {
    std::cout << "visit str literal\n";

    return std::make_shared<Memory::StringHolder>(
        std::move(static_cast<Parser::StringLiteral*>(node.get())->value
    ));
}
Runner::Expected Runner::VisitBoolLiteral(Runner::NodePtr& node) {
    std::cout << "visit bool literal\n";

    return static_cast<Parser::BoolLiteral*>(node.get())->value;
}
Runner::Expected Runner::VisitNilLiteral(Runner::NodePtr& node) {
    std::cout << "visit nil literal\n";

    return std::monostate{};
}

/// LITERAL EXPR
Runner::Expected Runner::VisitVar(Runner::NodePtr& node) {
    std::cout << "visit var\n";

    std::string id = std::move(static_cast<Parser::Var*>(node.get())->id);
    return stack_frame.Lookup(std::move(id));
}
Runner::Expected Runner::VisitList(Runner::NodePtr& node) {
    std::cout << "visit list\n";

    Parser::List* list = static_cast<Parser::List*>(node.get());

    std::vector<Memory::Holder> data;
    for (auto& n : list->data) {
        auto visited = Visit(n);
        if (visited) {
            data.push_back(std::move(*visited));
        } else {
            return std::unexpected(visited.error());
        }
    }
    return std::make_shared<Memory::ListHolder>(std::move(data));
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
            return std::unexpected(std::move(computed_left.error()));
        }
        return std::unexpected(std::move(computed_right.error()));
    }
}
Runner::Expected Runner::VisitSubscript(Runner::NodePtr& node) {
    std::cout << "visit subscript\n";
}



Runner::Expected Runner::Visit(Runner::NodePtr& node) {
    std::cout << "visit ...\n\t";
    switch (node->node) {
        case Parser::Nodes::N_NUM_LITERAL:
            return VisitNumLiteral(node);
        case Parser::Nodes::N_STRING_LITERAL:
            return VisitStringLiteral(node);
        case Parser::Nodes::N_VAR:
            return VisitVar(node);
        case Parser::Nodes::N_NIL:
            return VisitNil(node);

        case Parser::Nodes::N_UNARY_OP:
            return VisitUnaryOp(node);
        case Parser::Nodes::N_BINARY_OP:
            return VisitBinaryOp(node);
        
        case Parser::Nodes::N_ASSIGNMENT_OP:
            return VisitAssignmentOp(node);
        
        case Parser::Nodes::N_EMPTY:
            return VisitEmpty(node);
        case Parser::Nodes::N_COMPOUND:
            return VisitCompound(node);
        case Parser::Nodes::N_BAD:
            return VisitBad(node);
    }
}
