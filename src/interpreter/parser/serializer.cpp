#include <iostream>
#include <filesystem>
#include <fstream>
#include <memory>

#include <nlohmann/json.hpp>
#include "parser.h"

using json = nlohmann::json;
using NodePtr = std::unique_ptr<Parser::Node>;

struct Serializer {
    Serializer(Parser&& p) : parser(std::move(p)) {
        Lexer::Token token;
        parser.Parse();
        tree = Visit(parser.root);
    }

    json VisitNumLiteral(NodePtr& node) {
        return {
            {"type", "Num Literal"},
            {"value", static_cast<Parser::NumLiteral*>(node.get())->value}
        };
    }
    json VisitStringLiteral(NodePtr& node) {
        return {
            {"type", "String Literal"},
            {"value", static_cast<Parser::StringLiteral*>(node.get())->value}
        };
    }
    json VisitBoolLiteral(NodePtr& node) {
        return {
            {"type", "Bool Literal"},
            {"value", static_cast<Parser::BoolLiteral*>(node.get())->value ? "true" : "false"}
        };
    }
    json VisitNilLiteral(NodePtr& node) {
        return {
            {"type", "Nil Literal"}
        };
    }
    json VisitVar(NodePtr& node) {
        return {
            {"type", "Var"},
            {"id", static_cast<Parser::Var*>(node.get())->id}
        };
    }
    json VisitList(NodePtr& node) {
        Parser::List* list = static_cast<Parser::List*>(node.get());
        json j; j += {"type", "List"};
        json data;
        for (auto& ptr : list->data.data) {
            data += Visit(ptr);
        }
        j += {"data", data};
        return j;
    }

    json VisitUnaryOp(NodePtr& node) {
        Parser::UnaryOp* un_op = static_cast<Parser::UnaryOp*>(node.get());
        json j;
        j += {"type", "Unary Op"};
        j += {"operator", un_op->op};
        j += {"operand", Visit(un_op->operand)};
        return j;
    }
    json VisitBinaryOp(NodePtr& node) {
        Parser::BinaryOp* bin_op = static_cast<Parser::BinaryOp*>(node.get());
        json j;
        j += {"type", "Binary Op"};
        j += {"operator", bin_op->op};
        j += {"left", Visit(bin_op->left)};
        j += {"right", Visit(bin_op->right)};
        return j;
    }
    json VisitSubscript(NodePtr& node) {
        Parser::Subscript* sub = static_cast<Parser::Subscript*>(node.get());
        json j;
        j += {"type", "Subscript"};
        j += {"is_slice", sub->is_slice};
        j += {"var expr", Visit(sub->var_expr)};
        j += {"left", Visit(sub->left)};
        j += {"right", Visit(sub->right)};
        return j;
    }

    json VisitIf(NodePtr& node) {
        Parser::If* if_block = static_cast<Parser::If*>(node.get());
        json j;
        j += {"type", "If Block"};
        j += {"condition", Visit(if_block->condition)};
        j += {"body", Visit(if_block->body)};
        return j;
    }

    json VisitFor(NodePtr& node) {
        Parser::For* for_block = static_cast<Parser::For*>(node.get());
        json j;
        j += {"type", "For Block"};
        j += {"iterator", Visit(for_block->iterator)};
        j += {"range", Visit(for_block->range)};
        j += {"body", Visit(for_block->body)};
        return j;
    }
    json VisitWhile(NodePtr& node) {
        Parser::While* while_block = static_cast<Parser::While*>(node.get());
        json j;
        j += {"type", "While Block"};
        j += {"condition", Visit(while_block->condition)};
        j += {"body", Visit(while_block->body)};
        return j;
    }
    json VisitBreak(NodePtr& node) {
        return {"type", "break statement"};
    }
    json VisitContinue(NodePtr& node) {
        return {"type", "continue statement"};
    }

    json VisitFunc(NodePtr& node) {
        Parser::Func* func = static_cast<Parser::Func*>(node.get());
        json j; j += {"type", "function"};
        j += {"id", func->id.id};
        json params;
        for (auto& ptr : func->params.data) {
            params += Visit(ptr);
        }
        j += {"params", params};
        j += {"body", Visit(func->body)};
        return j;
    }
    json VisitFuncCall(NodePtr& node) {
        Parser::FuncCall* func_call = static_cast<Parser::FuncCall*>(node.get());
        json j; j += {"type", "function call"};
        j += {"func", Visit(func_call->func)};
        json params;
        for (auto& ptr : func_call->params) {
            params += Visit(ptr);
        }
        j += {"params", params};
        return j;
    }
    json VisitReturn(NodePtr& node) {
        Parser::Return* return_expr = static_cast<Parser::Return*>(node.get());
        json j; j += {"type", "return statement"};
        j += {"expr", Visit(return_expr->expr)};
        return j;
    }

    json VisitCompound(NodePtr& node) {
        json j;
        j += {"type", "compound"};
        json children;
        Parser::Compound* cmpd = static_cast<Parser::Compound*>(node.get());
        for (auto& child : cmpd->data) {
            children += Visit(child);
        }
        j += {"children", children};
        return j;
    }
    json VisitBad(NodePtr& node) {
        return {"type", "BAD"};
    }

    json Visit(NodePtr& node) {
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
            case Parser::Nodes::N_BAD:
                return VisitBad(node);
        }
    }

    Parser parser;
    json tree;
};

int main(int argc, char** argv) {
    if (argc != 2) return 1;

    std::filesystem::path path = argv[1];
    std::fstream file{path, std::ios::in | std::ios::ate};

    if (!file.is_open()) return 2;

    size_t size = file.tellg();
    file.seekg(0);
    std::string program(size, '\0');
    file.read(&program[0], size);

    Serializer serializer(Parser(Lexer::Tokenizer(std::move(program))));
    std::cout << serializer.tree.dump(2) << '\n';
}