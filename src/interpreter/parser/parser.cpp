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
    tokenizer.Broke(); tokenizer >> token;
    return std::make_unique<Bad>(Lexer::Token(Errors::ParserErrors::Panic(), GetTraitedToken()));
}
template<typename T>
requires std::derived_from<T, Error>
Parser::NodePtr Parser::MakeBadNode(T&& e) {
    tokenizer.Broke(); tokenizer >> token;
    auto tkn = GetTraitedToken();
    return std::make_unique<Bad>(Lexer::Token(std::forward<T>(e), std::move(tkn)));
}

/// GRAMMAR

// Var: T_VAR ( ('[' (Expr)? (':')? (Expr)? ']') | ('(' (expr (',' expr)* )? ')') )*
Parser::NodePtr Parser::VarExpr() {
    if (token.token != Lexer::Tokens::T_VAR) { return MakeBadNode(Errors::ParserErrors::ExpectedVarExpr()); }

    std::string id = std::get<std::string>(token.value);
    if (!Eat(token.token)) { return MakeBadNode(); }
    NodePtr var = std::make_unique<Var>(std::move(id), GetTraitedToken());

    using namespace Lexer;

    while (token.token == Tokens::T_LEFT_SQUARE_BRACKET || token.token == Tokens::T_LEFT_BRACKET) {
        if (token.token == Tokens::T_LEFT_SQUARE_BRACKET) {
            if (!Eat(token.token)) { return MakeBadNode(); }
            NodePtr expr_left, expr_right;
            bool is_slice = false;
            if (token.token != Tokens::T_RIGHT_SQUARE_BRACKET && token.token != Tokens::T_COLON) {
                expr_left = Expr();
            }
            if (token.token == Tokens::T_COLON) {
                if (!Eat(token.token)) { return MakeBadNode(); } GetTraitedToken();
                is_slice = true;
            }
            if (token.token != Tokens::T_RIGHT_SQUARE_BRACKET) {
                expr_right = Expr();
            }
            if (!Eat(Tokens::T_RIGHT_SQUARE_BRACKET)) { return MakeBadNode(); } GetTraitedToken();
            // expr_left and expr_right can be empty! check it without fail!
            var = std::make_unique<Subscript>(std::move(var), std::move(expr_left), std::move(expr_right), is_slice, GetTraitedToken());
        }
        else {
            if (!Eat(Tokens::T_LEFT_BRACKET)) { return MakeBadNode(); }
            FuncCall func_call(std::move(var), GetTraitedToken());
            if (token.token != Tokens::T_RIGHT_BRACKET) {
                func_call.params.push_back(Expr());
                while (token.token == Lexer::Tokens::T_COMMA) {
                    if (!Eat(token.token)) { return MakeBadNode(); } GetTraitedToken();
                    func_call.params.push_back(Expr());
                }
            }
            if (!Eat(Tokens::T_RIGHT_BRACKET)) { return MakeBadNode(Errors::ParserErrors::ExpectedRightBracket()); }
            var = std::make_unique<FuncCall>(std::move(func_call));
        }
    }

    return var;
}

// Factor: Number | String | Bool | Nil | VarExpr | List
//         | ('not' | '+' | '-') Factor
//         | '(' expr ')'
//         | T_BAD
Parser::NodePtr Parser::Factor() {   // add support for list literals.
    std::cout << "now try to dispatch in factor > " << Lexer::TOKENS_TO_STR[token.token] << '\n';

    using namespace Lexer;

    switch (token.token) {
        case Tokens::T_NUMBER:
        {   double value = std::get<double>(token.value);
            if (!Eat(token.token)) { return MakeBadNode(); }
            return std::make_unique<NumLiteral>(std::move(value), GetTraitedToken()); }
        case Tokens::T_STRING:
        {   std::string value = std::get<std::string>(token.value);
            if (!Eat(token.token)) { return MakeBadNode(); }
            return std::make_unique<StringLiteral>(std::move(value), GetTraitedToken()); }
        case Tokens::T_FALSE:
            if (!Eat(token.token)) { return MakeBadNode(); }
            return std::make_unique<BoolLiteral>(false, GetTraitedToken());
        case Tokens::T_TRUE:
            if (!Eat(token.token)) { return MakeBadNode(); }
            return std::make_unique<BoolLiteral>(true, GetTraitedToken());
        case Tokens::T_NIL:
            if (!Eat(token.token)) { return MakeBadNode(); }
            return std::make_unique<NilLiteral>(GetTraitedToken());
        case Tokens::T_VAR:
            return VarExpr();
        case Tokens::T_FUNC:
            return FuncExpr();
        case Tokens::T_MINUS:
        case Tokens::T_PLUS:
        case Tokens::T_NOT:
        {   Tokens op = token.token;
            if (!Eat(token.token)) { return MakeBadNode(); }
            return std::make_unique<UnaryOp>(op, Factor(), GetTraitedToken()); }
        case Tokens::T_LEFT_BRACKET:
        {   if (!Eat(token.token)) { return MakeBadNode(); }
            auto expr = Expr();
            if (!Eat(Tokens::T_RIGHT_BRACKET)) { return MakeBadNode(); }
            return expr; }
        case Tokens::T_BAD:
            return MakeBadNode(std::move(*std::get<std::shared_ptr<Error>>(token.value)));
        default:
            std::cout << "must to be here!\n";
            return MakeBadNode(Errors::ParserErrors::FactorError());
    }
}

// Term: Factor (('*' | '/' | '%' | '^' | 'and') Factor)*
Parser::NodePtr Parser::Term() {
    std::cout << "now try to dispatch in term > " << Lexer::TOKENS_TO_STR[token.token] << '\n';

    NodePtr term = Factor();
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
    std::cout << "now try to dispatch in expr > " << Lexer::TOKENS_TO_STR[token.token] << '\n';

    using namespace Lexer;

    NodePtr expr = Term();
    while (token.token == Tokens::T_MINUS 
    || token.token == Tokens::T_PLUS
    || token.token == Tokens::T_OR
    || token.token == Tokens::T_COMP_EQUAL
    || token.token == Tokens::T_COMP_NON_EQUAL
    || token.token == Tokens::T_COMP_SMALLER
    || token.token == Tokens::T_COMP_GREATER
    || token.token == Tokens::T_COMP_SMALLER_OR_EQ
    || token.token == Tokens::T_COMP_GREATER_OR_EQ) {
        auto op = token.token;
        if (!Eat(token.token)) { return MakeBadNode(); }
        expr = std::make_unique<BinaryOp>(op, std::move(expr), Term(), GetTraitedToken());
    }
    return expr;
}

// Assignment: VarExpr ('=', '+=', '-=', '*=', '/=', '%=', '^=') Expr
Parser::NodePtr Parser::Assignment() {
    using namespace Lexer;

    NodePtr var = VarExpr();
    switch (token.token) {
        case Tokens::T_EQUAL:
        case Tokens::T_EQUAL_PLUS:
        case Tokens::T_EQUAL_MINUS:
        case Tokens::T_EQUAL_DIV:
        case Tokens::T_EQUAL_MOD:
        case Tokens::T_EQUAL_XOR:
        {   auto op = token.token;
            if (!Eat(token.token)) { return MakeBadNode(); }
            return std::make_unique<BinaryOp>(op, std::move(var), Expr(), GetTraitedToken()); }
        default:
            return MakeBadNode(Errors::ParserErrors::ExpectedAssignment());
    }
}

// BreakExpr: T_BREAK
Parser::NodePtr Parser::BreakExpr() {
    if (!Eat(Lexer::Tokens::T_BREAK)) { return MakeBadNode(); }
    return std::make_unique<Break>(GetTraitedToken());
}

// ContinueExpr: T_CONTINUE
Parser::NodePtr Parser::ContinueExpr() {
    if (!Eat(Lexer::Tokens::T_CONTINUE)) { return MakeBadNode(); }
    return std::make_unique<Continue>(GetTraitedToken());
}

// ReturnExpr: 'return' Expr
Parser::NodePtr Parser::ReturnExpr() {
    if (!Eat(Lexer::Tokens::T_RETURN)) { return MakeBadNode(); }
    return std::make_unique<Return>(Expr(), GetTraitedToken());
}

// Statement: ReturnExpr | BreakExpr | ContinueExpr | Assignment | Expr
Parser::NodePtr Parser::Statement() {
    std::cout << "now try to dispatch in statement > " << Lexer::TOKENS_TO_STR[token.token] << '\n';

    using namespace Lexer;

    switch (token.token) {
        case Tokens::T_RETURN:
            return ReturnExpr();
        case Tokens::T_BREAK:
            return BreakExpr();
        case Tokens::T_CONTINUE:
            return ContinueExpr();
        case Tokens::T_VAR:
            return VarDispatch();
        default:
            return Expr();
    }
}

// IfBlock: T_IF Expr T_THEN BLOCK (T_ELSE_IF Expr T_THEN BLOCK)* (T_ELSE BLOCK)? T_END_IF
Parser::NodePtr Parser::IfBlock() {
    if (!Eat(Lexer::Tokens::T_IF)) { return MakeBadNode(); }

    auto condition = Expr();
    if (!Eat(Lexer::Tokens::T_THEN)) {
        return MakeBadNode(Errors::ParserErrors::ExpectedThen());
    } GetTraitedToken();

    Compound if_block;
    if_block.data.push_back(std::make_unique<If>(std::move(condition), Block(), GetTraitedToken()));
    
    while (token.token == Lexer::Tokens::T_ELSE_IF) {
        if (!Eat(token.token)) { return MakeBadNode(); }

        condition = Expr();
        if (!Eat(Lexer::Tokens::T_THEN)) {
            return MakeBadNode(Errors::ParserErrors::ExpectedThen());
        } GetTraitedToken();
        if_block.data.push_back(std::make_unique<If>(std::move(condition), Block(), GetTraitedToken()));
    }
    if (token.token == Lexer::Tokens::T_ELSE) {
        if (!Eat(token.token)) { return MakeBadNode(); }

        if_block.data.push_back(std::make_unique<If>(
            std::make_unique<BoolLiteral>(true, Lexer::Token(token)),
            Block(),
            GetTraitedToken()
        ));
    }
    if (!Eat(Lexer::Tokens::T_END_IF)) { return MakeBadNode(Errors::ParserErrors::ExpectedEndIf()); }
    return std::make_unique<Compound>(std::move(if_block));
}

// ForBlock: T_FOR VarExpr T_IN VarExpr BLOCK T_FOR_END
Parser::NodePtr Parser::ForBlock() {
    std::cout << "for entrance\n";
    if (!Eat(Lexer::Tokens::T_FOR)) { return MakeBadNode(); }

    auto iterator = VarExpr();
    if (!Eat(Lexer::Tokens::T_IN)) { return MakeBadNode(Errors::ParserErrors::ExpectedIn()); } GetTraitedToken();
    auto range = VarExpr();

    For for_block = For(std::move(iterator), std::move(range), Block(), GetTraitedToken());
    if (!Eat(Lexer::Tokens::T_END_FOR)) { return MakeBadNode(Errors::ParserErrors::ExpectedEndFor()); } GetTraitedToken();
    return std::make_unique<For>(std::move(for_block));
}

// WhileBlock: T_WHILE Expr BLOCK T_END_WHILE
Parser::NodePtr Parser::WhileBlock() {
    if (!Eat(Lexer::Tokens::T_WHILE)) { return MakeBadNode(); }

    auto condition = Expr();
    While while_block = While(std::move(condition), Block(), GetTraitedToken());
    if (!Eat(Lexer::Tokens::T_END_WHILE)) { return MakeBadNode(Errors::ParserErrors::ExpectedEndWhile()); } GetTraitedToken();
    return std::make_unique<While>(std::move(while_block));
}

// FuncExpr: T_FUNC '(' ((T_VAR | Func) (',' (T_VAR | Func))*)? ')' BLOCK T_END_FUNC
Parser::NodePtr Parser::FuncExpr() {
    if (!Eat(Lexer::Tokens::T_FUNC)) { return MakeBadNode(); }

    if (!Eat(Lexer::Tokens::T_LEFT_BRACKET)) {
        return MakeBadNode(Errors::ParserErrors::ExpectedLeftBracket());
    } GetTraitedToken();

    Compound params;

    if (token.token != Lexer::Tokens::T_RIGHT_BRACKET) {
        bool was_comma = false;
        do {
            if (was_comma) { GetTraitedToken(); }
            switch (token.token) {
                case Lexer::Tokens::T_VAR:
                {   std::string id = std::get<std::string>(token.value);
                    if (!Eat(token.token)) { return MakeBadNode(); }
                    params.data.push_back(std::make_unique<Var>(std::move(id), GetTraitedToken())); }
                    break;
                case Lexer::Tokens::T_FUNC:
                    params.data.push_back(FuncExpr());
                    break;
                default:
                    return MakeBadNode(Errors::ParserErrors::FunctionParamsError());
            }
            was_comma = true;
        } while (Eat(Lexer::Tokens::T_COMMA));
    }

    if (!Eat(Lexer::Tokens::T_RIGHT_BRACKET)) {
        return MakeBadNode(Errors::ParserErrors::ExpectedRightBracket());
    } GetTraitedToken();
    
    NodePtr func = std::make_unique<Func>(std::move(params), Block(), GetTraitedToken());
    
    if (!Eat(Lexer::Tokens::T_END_FUNC)) {
        return MakeBadNode(Errors::ParserErrors::ExpectedEndFunc());
    } GetTraitedToken();

    return std::move(func);
}

// StatementList: If | While | For | Statement
Parser::NodePtr Parser::StatementList() {
    std::cout << "now try to dispatch in statement list > " << Lexer::TOKENS_TO_STR[token.token] << '\n';
    switch (token.token) {
        case Lexer::Tokens::T_IF:
            return IfBlock();
        case Lexer::Tokens::T_FOR:
            return ForBlock();
        case Lexer::Tokens::T_WHILE:
            return WhileBlock();
        default:
            return Statement();
    }
}

// Block: StatementList (T_EOL | StatementList)*
Parser::NodePtr Parser::Block() {
    Compound node;
    while ( token.token != Lexer::Tokens::T_ELSE_IF && token.token != Lexer::Tokens::T_ELSE
    && token.token != Lexer::Tokens::T_END_IF && token.token != Lexer::Tokens::T_END_WHILE
    && token.token != Lexer::Tokens::T_END_FOR && token.token != Lexer::Tokens::T_END_FUNC
    && token.token != Lexer::Tokens::T_EOF) {
        if (token.token == Lexer::Tokens::T_EOL) {
            if (!Eat(token.token)) { return MakeBadNode(); }
        } else {
            node.data.push_back(StatementList());
        }
    }
    return std::make_unique<Compound>(std::move(node));
}

// Helper for dispatching Expr and Assignment grammar
Parser::NodePtr Parser::VarDispatch() {
    auto saved_token = token;
    auto saved_tokens_traits = tokens_traits;
    auto saved_tokenizer = tokenizer;
    NodePtr assignment_try = Assignment();
    if (assignment_try->node == Nodes::N_BAD) {
        token = std::move(saved_token);
        tokens_traits = std::move(saved_tokens_traits);
        tokenizer = std::move(saved_tokenizer);
        return Expr();
    } else {
        return assignment_try;
    }
}

void Parser::Parse() {
    tokenizer >> token;
    root = Block();
}