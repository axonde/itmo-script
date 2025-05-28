#include "parser.h"

/// ADVANCE
bool Parser::Eat(Lexer::Tokens token_type) {
    tokens_traits.push(token);
    if (token.token == token_type) {
        tokenizer >> token;
        return true;
    }
    return false;
}
Lexer::Token Parser::GetTraitedToken() {
    if (tokens_traits.empty()) { return token; }
    Lexer::Token tkn = tokens_traits.top();
    tokens_traits.pop();
    return tkn;
}

/// ERROR TRACKING
Parser::NodePtr Parser::MakeBadNode() {
    return std::make_unique<Bad>(Lexer::Token(Errors::ParserErrors::Base(), GetTraitedToken()));
}
template<typename T>
requires std::derived_from<T, Error>
Parser::NodePtr Parser::MakeBadNode(T&& e) {
    auto token = GetTraitedToken();
    return std::make_unique<Bad>(Lexer::Token(std::forward<T>(e), std::move(token)));
}

/// GRAMMAR

// Var: T_VAR ( ('[' (Expr)? (':')? (Expr)? ']') | ('(' (expr (',' expr)* )? ')') )*
Parser::NodePtr Parser::VarExpr() {
    std::string id = std::get<std::string>(token.value);
    if (!Eat(token.token)) return MakeBadNode();
    NodePtr var = std::make_unique<Var>(std::move(id), GetTraitedToken());

    using namespace Lexer;

    while (token.token == Tokens::T_LEFT_SQUARE_BRACKET || token.token == Tokens::T_LEFT_BRACKET) {
        if (token.token == Tokens::T_LEFT_SQUARE_BRACKET) {
            if (!Eat(token.token)) return MakeBadNode();
            NodePtr expr_left, expr_right;
            if (token.token != Tokens::T_RIGHT_SQUARE_BRACKET || token.token != Tokens::T_COLON) {
                expr_left = Expr();
            }
            if (token.token == Tokens::T_COLON) {
                if (!Eat(token.token)) return MakeBadNode(); GetTraitedToken();
            }
            if (token.token != Tokens::T_RIGHT_SQUARE_BRACKET) {
                expr_right = Expr();
            }
            if (!Eat(Tokens::T_RIGHT_SQUARE_BRACKET)) return MakeBadNode(); GetTraitedToken();
            // expr_left and expr_right can be empty! check it without fail!
            var = std::make_unique<TernaryOp>(Lexer::T_LEFT_SQUARE_BRACKET, std::move(var), std::move(expr_left), std::move(expr_right), GetTraitedToken());
        }
        else {
            if (!Eat(Tokens::T_LEFT_BRACKET)) return MakeBadNode();
            FuncCall func_call(GetTraitedToken());
            if (token.token != Tokens::T_RIGHT_BRACKET) {
                func_call.params.push_back(Expr());
                while (token.token == Lexer::Tokens::T_COMMA) {
                    if (!Eat(Tokens::T_LEFT_BRACKET)) return MakeBadNode(); GetTraitedToken();
                    func_call.params.push_back(Expr());
                }
            }
            return std::make_unique<FuncCall>(std::move(func_call));
        }
    }

    return var;
}

// Factor: Number | String | Bool | Nil | Var | List
//         | ('not' | '+' | '-') Factor
//         | '(' expr ')'
Parser::NodePtr Parser::Factor() {
    using namespace Lexer;

    switch (token.token) {
        case Tokens::T_NUMBER:
        {   double value = std::get<double>(token.value);
            if (!Eat(token.token)) return MakeBadNode();
            return std::make_unique<NumLiteral>(std::move(value), GetTraitedToken()); }
        case Tokens::T_STRING:
        {   std::string value = std::get<std::string>(token.value);
            if (!Eat(token.token)) return MakeBadNode();
            return std::make_unique<StringLiteral>(std::move(value), GetTraitedToken()); }
        case Tokens::T_FALSE:
            if (!Eat(token.token)) return MakeBadNode();
            return std::make_unique<BoolLiteral>(false, GetTraitedToken());
        case Tokens::T_TRUE:
            if (!Eat(token.token)) return MakeBadNode();
            return std::make_unique<BoolLiteral>(true, GetTraitedToken());
        case Tokens::T_NIL:
            if (!Eat(token.token)) return MakeBadNode();
            return std::make_unique<NilLiteral>(GetTraitedToken());
        case Tokens::T_VAR:
            return VarExpr();
        case Tokens::T_FUNC:
            return Func();
        case Tokens::T_MINUS:
        case Tokens::T_PLUS:
        case Tokens::T_NOT:
        {   Tokens op = token.token;
            if (!Eat(token.token)) return MakeBadNode();
            return std::make_unique<UnaryOp>(op, Factor(), GetTraitedToken()); }
        case Tokens::T_LEFT_BRACKET:
        {   if (!Eat(token.token)) return MakeBadNode();
            auto expr = Expr();
            if (!Eat(Tokens::T_RIGHT_BRACKET)) return MakeBadNode();
            return expr; }
        default:
            return MakeBadNode(Errors::ParserErrors::FactorError());
    }
}

// Term: Factor (('*' | '/' | '%' | '^' | 'and') Factor)*
Parser::NodePtr Parser::Term() {
    Parser::NodePtr term = Factor();
    while (token.token == Lexer::Tokens::T_MULT
    || token.token == Lexer::Tokens::T_MOD
    || token.token == Lexer::Tokens::T_XOR
    || token.token == Lexer::Tokens::T_AND) {
        auto op = token.token;
        if (!Eat(token.token)) { return MakeBadNode(); }
        term = std::make_unique<BinaryOp>(op, std::move(term), Term(), GetTraitedToken());
    }
    return term;
}

// Expr: Term (('+' | '-' | 'or' | '==' | '!=' | '<' | '>' | '<=' | '>=') Term)*
Parser::NodePtr Parser::Expr() {
    Parser::NodePtr expr = Term();
    while (token.token == Lexer::Tokens::T_MINUS 
    || token.token == Lexer::Tokens::T_PLUS
    || token.token == Lexer::Tokens::T_OR
    || token.token == Lexer::Tokens::T_COMP_EQUAL
    || token.token == Lexer::Tokens::T_COMP_NON_EQUAL
    || token.token == Lexer::Tokens::T_COMP_SMALLER
    || token.token == Lexer::Tokens::T_COMP_GREATER
    || token.token == Lexer::Tokens::T_COMP_SMALLER_OR_EQ
    || token.token == Lexer::Tokens::T_COMP_GREATER_OR_EQ) {
        auto op = token.token;
        if (!Eat(token.token)) { return MakeBadNode(); }
        expr = std::make_unique<BinaryOp>(op, std::move(expr), Term(), GetTraitedToken());
    }
    return expr;
}


// Assignment: Var ('=', '+=', '-=', '*=', '/=', '%=', '^=') Expr
Parser::NodePtr Parser::Assignment() {
    return MakeBadNode();
}


// Return: 'return' Expr
Parser::NodePtr Parser::ReturnExpr() {
    if (!Eat(Lexer::Tokens::T_RETURN)) { return MakeBadNode(); }
    return std::make_unique<Return>(Expr(), GetTraitedToken());
}

// std::unique_ptr<Parser::Node> Parser::RValueStatement() {
//     switch (token.token) {
//         case Lexer::Tokens::T_EOL:
//         case Lexer::Tokens::T_EOF:
//             if (!Eat(token.token)) { return MakeBadNode(); }
//             return std::make_unique<Empty>(GetTraitedToken());
//         default:
//             return Expr();
//     }
// }
// std::unique_ptr<Parser::Node> Parser::AssignmentStatement(Var&& var) {
//     if (!Eat(Lexer::Tokens::T_EQUAL)) { return MakeBadNode(); }
//     return std::make_unique<AssignOp>(std::move(var), Expr(), GetTraitedToken());
// }
// std::unique_ptr<Parser::Node> Parser::Statement() {
//     std::unique_ptr<Parser::Node> statement;
//     switch (token.token) {
//         case Lexer::Tokens::T_VAR:
//             statement = VarDecomposition();
//         default:
//             statement = RValueStatement();
//     }

//     if (token.token == Lexer::Tokens::T_EOL || token.token == Lexer::Tokens::T_EOF) {
//         if (token.token == Lexer::Tokens::T_EOL) {
//             if (!Eat(token.token)) { return MakeBadNode(); }
//         }
//         return std::move(statement);
//     }
//     return MakeBadNode(Errors::ParserErrors::RValueAssignment());
// }


// std::unique_ptr<Parser::Node> Parser::VarDecomposition() {
//     Var var(std::get<std::string>(token.value), GetTraitedToken());
//     if (!Eat(Lexer::Tokens::T_VAR)) { return MakeBadNode(); }

//     if (tokenizer.Peek().token == Lexer::Tokens::T_EQUAL) {
//         return AssignmentStatement(std::move(var));
//     }

//     return RValueStatement();
// }
std::unique_ptr<Parser::Node> Parser::Block() {
    Compound node;
    node.data.push_back(Statement());
    while (token.token == Lexer::Tokens::T_EOL) {
        if (!Eat(Lexer::Tokens::T_EOL)) { return MakeBadNode(); }
        node.data.push_back(Statement());
    }
    return std::make_unique<Compound>(std::move(node));
}

void Parser::Parse() {
    tokenizer >> token;
    root = Block();
}