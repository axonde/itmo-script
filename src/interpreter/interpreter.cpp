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
    Runner runner(std::move(program));

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
    return static_cast<Parser::NumLiteral*>(node.get())->value;
}
Runner::Expected Runner::VisitStringLiteral(Runner::NodePtr& node) {
    return std::move(static_cast<Parser::StringLiteral*>(node.get())->value);
}
Runner::Expected Runner::VisitVar(Runner::NodePtr& node) {
    return std::move(static_cast<Parser::Var*>(node.get())->id);
}
Runner::Expected Runner::VisitNil(Runner::NodePtr&) {
    return std::monostate{};
}

/// OPERATIONS
Runner::Expected Runner::VisitNoOp(NodePtr&) {
    std::cout << "visit no op\n";
    return std::monostate{};
}
Runner::Expected Runner::VisitUnaryOp(Runner::NodePtr& node) {
    Parser::UnaryOp* ptr = static_cast<Parser::UnaryOp*>(node.get());
    auto computed_child = Visit(ptr->child);
    if (computed_child) {
        if (auto computed = Operators::ExecUnaryOperation(ptr, std::move(*computed_child)); computed) {
            // need to update the new type of the node.
            return std::move(*computed);
        } else {
            return std::unexpected(std::move(computed.error()));
        }
    } else {
        return std::unexpected(std::move(computed_child.error()));
    }
}
Runner::Expected Runner::VisitBinaryOp(Runner::NodePtr& node) {
    std::cout << "visit binary op, curr line " << node->token.line << ", col " << node->token.pos << '\n';
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

Runner::Expected Runner::VisitAssignmentOp(Runner::NodePtr& node) {
    // creata a new variable in interpretator memory
    std::cout << "visit assign\n";
    return std::unexpected(Lexer::Token{Errors::InternalErrors::NotImplemented(), node->token});
}

Runner::Expected Runner::VisitCompound(Runner::NodePtr& node) {
    std::cout << "visit compound\n";
    Parser::Compound* ptr = static_cast<Parser::Compound*>(node.get());
    for (auto& child : ptr->children) {
        std::cout << "Yes, Im here.\n";
        if (auto visit = Visit(child); !visit) {
            std::cout << "before dying!";
            return std::unexpected(std::move(visit.error()));
        }
    }
    return std::monostate{};
}

Runner::Expected Runner::VisitBad(Runner::NodePtr& node) {
    std::cout << "visit bad\n";
    return std::unexpected(Lexer::Token{Errors::InternalErrors::NotImplemented(), node->token});
}

Runner::Expected Runner::Visit(Runner::NodePtr& node) {
    std::cout << "visit visit\n";
    switch (node->node) {
        case Parser::Nodes::N_NUM_LITERAL:
            return VisitNumLiteral(node);
        case Parser::Nodes::N_STRING_LITERAL:
            return VisitStringLiteral(node);
        case Parser::Nodes::N_VAR:
            return VisitVar(node);
        case Parser::Nodes::N_NIL:
            return VisitNil(node);

        case Parser::Nodes::N_NO_OP:
            return VisitNoOp(node);
        case Parser::Nodes::N_UNARY_OP:
            return VisitUnaryOp(node);
        case Parser::Nodes::N_BINARY_OP:
            return VisitBinaryOp(node);
        
        case Parser::Nodes::N_ASSIGNMENT_OP:
            return VisitAssignmentOp(node);
        
        case Parser::Nodes::N_COMPOUND:
            return VisitCompound(node);
        case Parser::Nodes::N_BAD:
            return VisitBad(node);
    }
}
