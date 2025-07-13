#include "parser.h"

/// HELP
template<typename T>
requires std::derived_from<T, Error>
T Parser::MakeError() {
    return T(token.lineno, token.column);
}

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

/// GRAMMAR

// Var: T_VAR ( ('[' (Expr)? 1(':' | ':' (Expr)? 2(':' | ':' (Expr)? )? )? ) ']')
//            | ('(' (expr (',' expr)* )? ')') )*
Parser::NodePtr Parser::VarExpr() {
    // std::cout << "[parser log] var expr \n";

    if (token.token != Lexer::Tokens::T_VAR) {
        throw MakeError<Errors::ParserErrors::ExpectedVarExpr>();
    }

    std::string id = std::get<std::string>(token.value);
    if (!Eat(token.token)) { throw MakeError<ParserError>(); }
    NodePtr var = std::make_unique<Var>(std::move(id), GetTraitedToken());

    using namespace Lexer;

    while (token.token == Tokens::T_LEFT_SQUARE_BRACKET || token.token == Tokens::T_LEFT_BRACKET) {
        if (token.token == Tokens::T_LEFT_SQUARE_BRACKET) {
            if (!Eat(token.token)) { throw MakeError<ParserError>(); }
            NodePtr expr_start, expr_end, expr_step;
            bool is_slice = (token.token == Tokens::T_RIGHT_SQUARE_BRACKET);

            if (token.token != Tokens::T_RIGHT_SQUARE_BRACKET && token.token != Tokens::T_COLON) {
                expr_start = Expr();
            }
            if (token.token == Tokens::T_COLON) {
                if (!Eat(token.token)) { throw MakeError<ParserError>(); } GetTraitedToken();
                is_slice = true;

                if (token.token != Tokens::T_RIGHT_SQUARE_BRACKET && token.token != Tokens::T_COLON) {
                    expr_end = Expr();
                }

                if (token.token == Tokens::T_COLON) {
                    if (!Eat(token.token)) { throw MakeError<ParserError>(); } GetTraitedToken();

                    if (token.token != Tokens::T_RIGHT_SQUARE_BRACKET) {
                        expr_step = Expr();
                    }
                }
            }

            if (!Eat(Tokens::T_RIGHT_SQUARE_BRACKET)) {
                throw MakeError<Errors::ParserErrors::ExpectedRightSquareBracket>();
            } GetTraitedToken();
            var = std::make_unique<Subscript>(std::move(var), std::move(expr_start), std::move(expr_end), std::move(expr_step), is_slice, GetTraitedToken());
        }
        else {
            if (!Eat(Tokens::T_LEFT_BRACKET)) { throw MakeError<ParserError>(); }
            FuncCall func_call(std::move(var), GetTraitedToken());
            if (token.token != Tokens::T_RIGHT_BRACKET) {
                func_call.params.push_back(Expr());
                while (token.token == Lexer::Tokens::T_COMMA) {
                    if (!Eat(token.token)) { throw MakeError<ParserError>(); } GetTraitedToken();
                    func_call.params.push_back(Expr());
                }
            }
            if (!Eat(Tokens::T_RIGHT_BRACKET)) {
                throw MakeError<Errors::ParserErrors::ExpectedRightBracket>();
            } GetTraitedToken();
            var = std::make_unique<FuncCall>(std::move(func_call));
        }
    }

    return var;
}

// ListExpr: '[' (T_EOL)* (Expr (T_EOL)* (, (T_EOL)* Expr (T_EOL)*)* )? (,)? (T_EOL)* ']'
Parser::NodePtr Parser::ListExpr() {
    // std::cout << "[parser log] list expr \n";

    if (!Eat(Lexer::Tokens::T_LEFT_SQUARE_BRACKET)) {
        throw MakeError<Errors::ParserErrors::ExpectedLeftSquareBracket>();
    }

    auto skip_eol = [&]() {
        while (token.token == Lexer::Tokens::T_EOL) {
            if (!Eat(token.token)) { throw MakeError<ParserError>(); } GetTraitedToken();
        }
    };

    auto list = std::make_unique<List>(GetTraitedToken());
    skip_eol();

    if (token.token != Lexer::Tokens::T_RIGHT_SQUARE_BRACKET) {
        list->data.push_back(Expr());
        skip_eol();
        while (token.token == Lexer::T_COMMA) {
            if (!Eat(token.token)) { throw MakeError<ParserError>(); } GetTraitedToken();
            skip_eol();
            if (token.token != Lexer::Tokens::T_RIGHT_SQUARE_BRACKET) {
                list->data.push_back(Expr());
            }
            skip_eol();
        }
    }

    if (!Eat(Lexer::Tokens::T_RIGHT_SQUARE_BRACKET)) {
        throw MakeError<Errors::ParserErrors::ExpectedRightSquareBracket>();
    } GetTraitedToken();

    return list;
}

// Factor: T_EOL* Number | String | Bool | Nil | VarExpr | ListExpr | FuncExpr
//         | ('not' | '+' | '-') Factor
//         | '(' expr ')'
Parser::NodePtr Parser::Factor() {
    // std::cout << "[parser log] factor \n";

    using namespace Lexer;

    auto make_func_call = [&](auto&& expr) {
        if (!Eat(Tokens::T_LEFT_BRACKET)) { throw MakeError<ParserError>(); }
        FuncCall func_call(std::move(expr), GetTraitedToken());
        if (token.token != Tokens::T_RIGHT_BRACKET) {
            func_call.params.push_back(Expr());
            while (token.token == Lexer::Tokens::T_COMMA) {
                if (!Eat(token.token)) { throw MakeError<ParserError>(); } GetTraitedToken();
                func_call.params.push_back(Expr());
            }
        }
        if (!Eat(Tokens::T_RIGHT_BRACKET)) {
            throw MakeError<Errors::ParserErrors::ExpectedRightBracket>();
        } GetTraitedToken();
        return std::make_unique<FuncCall>(std::move(func_call));
    };

    switch (token.token) {
        case Tokens::T_NUMBER:
        {   double value = std::get<double>(token.value);
            if (!Eat(token.token)) { throw MakeError<ParserError>(); }
            return std::make_unique<NumLiteral>(std::move(value), GetTraitedToken()); }
        case Tokens::T_STRING:
        {   std::string value = std::get<std::string>(token.value);
            if (!Eat(token.token)) { throw MakeError<ParserError>(); }
            return std::make_unique<StringLiteral>(std::move(value), GetTraitedToken()); }
        case Tokens::T_FALSE:
            if (!Eat(token.token)) { throw MakeError<ParserError>(); }
            return std::make_unique<BoolLiteral>(false, GetTraitedToken());
        case Tokens::T_TRUE:
            if (!Eat(token.token)) { throw MakeError<ParserError>(); }
            return std::make_unique<BoolLiteral>(true, GetTraitedToken());
        case Tokens::T_NIL:
            if (!Eat(token.token)) { throw MakeError<ParserError>(); }
            return std::make_unique<NilLiteral>(GetTraitedToken());
        case Tokens::T_VAR:
            return VarExpr();
        case Tokens::T_FUNC:
        {   auto func_expr = FuncExpr();
            if (token.token == Tokens::T_LEFT_BRACKET) {
                return make_func_call(std::move(func_expr));
            }
            return func_expr; }
        case Tokens::T_MINUS:
        case Tokens::T_PLUS:
        case Tokens::T_NOT:
        {   Tokens op = token.token;
            if (!Eat(token.token)) { throw MakeError<ParserError>(); }
            return std::make_unique<UnaryOp>(op, Factor(), GetTraitedToken()); }
        case Tokens::T_LEFT_SQUARE_BRACKET:
            return ListExpr();
        case Tokens::T_LEFT_BRACKET:
        {   if (!Eat(token.token)) { throw MakeError<ParserError>(); } GetTraitedToken();
            NodePtr expr;
            if (token.token != Tokens::T_RIGHT_BRACKET) { expr = Expr(); }
            else { expr = std::make_unique<NilLiteral>(token); }
            if (!Eat(Tokens::T_RIGHT_BRACKET)) { throw MakeError<ParserError>(); } GetTraitedToken();
            if (token.token == Tokens::T_LEFT_BRACKET && expr->node == N_FUNC) {
                return make_func_call(std::move(expr));
            }
            return expr; }
        default:
            throw MakeError<Errors::ParserErrors::FactorError>();
    }
}

// Term: Factor (('*' | '/' | '%' | '^' | 'and') Factor)*
Parser::NodePtr Parser::Term() {
    // std::cout << "[parser log] term \n";

    NodePtr term = Factor();
    while (token.token == Lexer::Tokens::T_MULT
    || token.token == Lexer::Tokens::T_DIV
    || token.token == Lexer::Tokens::T_MOD
    || token.token == Lexer::Tokens::T_XOR
    || token.token == Lexer::Tokens::T_AND) {
        auto op = token.token;
        if (!Eat(token.token)) { throw MakeError<ParserError>(); }
        term = std::make_unique<BinaryOp>(op, std::move(term), Term(), GetTraitedToken());
    }
    return term;
}

// Expr: Term (('+' | '-' | 'or' | '==' | '!=' | '<' | '>' | '<=' | '>=') Term)*
Parser::NodePtr Parser::Expr() {
    // std::cout << "[parser log] expr\n";

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
        if (!Eat(token.token)) { throw MakeError<ParserError>(); }
        expr = std::make_unique<BinaryOp>(op, std::move(expr), Term(), GetTraitedToken());
    }
    return expr;
}

// Assignment: VarExpr ('=', '+=', '-=', '*=', '/=', '%=', '^=') Expr
Parser::NodePtr Parser::Assignment() {
    // std::cout << "[parser log] assignment expr\n";

    using namespace Lexer;

    NodePtr var = VarExpr();
    switch (token.token) {
        case Tokens::T_EQUAL:
        case Tokens::T_EQUAL_PLUS:
        case Tokens::T_EQUAL_MINUS:
        case Tokens::T_EQUAL_MULT:
        case Tokens::T_EQUAL_DIV:
        case Tokens::T_EQUAL_MOD:
        case Tokens::T_EQUAL_XOR:
        {   auto op = token.token;
            if (!Eat(token.token)) { throw MakeError<ParserError>(); }
            return std::make_unique<BinaryOp>(op, std::move(var), Expr(), GetTraitedToken()); }
        default:
            throw MakeError<Errors::ParserErrors::ExpectedAssignment>();
    }
}

// BreakExpr: T_BREAK
Parser::NodePtr Parser::BreakExpr() {
    // std::cout << "[parser log] break expr\n";

    if (!Eat(Lexer::Tokens::T_BREAK)) { throw MakeError<ParserError>(); }
    return std::make_unique<Break>(GetTraitedToken());
}

// ContinueExpr: T_CONTINUE
Parser::NodePtr Parser::ContinueExpr() {
    // std::cout << "[parser log] continue expr\n";

    if (!Eat(Lexer::Tokens::T_CONTINUE)) { throw MakeError<ParserError>(); }
    return std::make_unique<Continue>(GetTraitedToken());
}

// ReturnExpr: 'return' Expr
Parser::NodePtr Parser::ReturnExpr() {
    // std::cout << "[parser log] return expr\n";

    if (!Eat(Lexer::Tokens::T_RETURN)) { throw MakeError<ParserError>(); }
    return std::make_unique<Return>(Expr(), GetTraitedToken());
}

// Statement: ReturnExpr | BreakExpr | ContinueExpr | Assignment | Expr
Parser::NodePtr Parser::Statement() {
    // std::cout << "[parser log] looke at statement\n";

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
    // std::cout << "[parser log] if block\n";

    if (!Eat(Lexer::Tokens::T_IF)) { throw MakeError<ParserError>(); }

    auto condition = std::make_unique<BinaryOp>(
        Lexer::Tokens::T_COMP_EQUAL,
        Expr(),
        std::make_unique<BoolLiteral>(true, token),
        GetTraitedToken()
    );
    if (!Eat(Lexer::Tokens::T_THEN)) {
        throw MakeError<Errors::ParserErrors::ExpectedThen>();
    } GetTraitedToken();

    If if_block;
    if_block.cases.push_back(If::IfCase(std::move(condition), Block()));

    while (token.token == Lexer::Tokens::T_ELSE_IF) {
        if (!Eat(token.token)) { throw MakeError<ParserError>(); }

        condition = std::make_unique<BinaryOp>(
            Lexer::Tokens::T_COMP_EQUAL,
            Expr(),
            std::make_unique<BoolLiteral>(true, token),
            GetTraitedToken()
        );
        if (!Eat(Lexer::Tokens::T_THEN)) {
            throw MakeError<Errors::ParserErrors::ExpectedThen>();
        } GetTraitedToken();
        if_block.cases.push_back(If::IfCase(std::move(condition), Block()));
    }
    if (token.token == Lexer::Tokens::T_ELSE) {
        if (!Eat(token.token)) { throw MakeError<ParserError>(); }

        condition = std::make_unique<BinaryOp>(
            Lexer::Tokens::T_COMP_EQUAL,
            std::make_unique<BoolLiteral>(true, token),
            std::make_unique<BoolLiteral>(true, token),
            GetTraitedToken()
        );

        if_block.cases.push_back(If::IfCase(
            std::move(condition),
            Block()
        ));
    }
    if (!Eat(Lexer::Tokens::T_END_IF)) {
        throw MakeError<Errors::ParserErrors::ExpectedEndIf>();
    } GetTraitedToken();
    return std::make_unique<If>(std::move(if_block));
}

// ForBlock: T_FOR Expr T_IN Expr BLOCK T_FOR_END
Parser::NodePtr Parser::ForBlock() {
    // std::cout << "[parser log] for block\n";

    if (!Eat(Lexer::Tokens::T_FOR)) { throw MakeError<ParserError>(); }

    auto iterator = Expr();
    if (!Eat(Lexer::Tokens::T_IN)) { throw MakeError<Errors::ParserErrors::ExpectedIn>(); } GetTraitedToken();
    auto range = Expr();

    For for_block = For(std::move(iterator), std::move(range), Block(), GetTraitedToken());
    if (!Eat(Lexer::Tokens::T_END_FOR)) { throw MakeError<Errors::ParserErrors::ExpectedEndFor>(); } GetTraitedToken();
    return std::make_unique<For>(std::move(for_block));
}

// WhileBlock: T_WHILE Expr BLOCK T_END_WHILE
Parser::NodePtr Parser::WhileBlock() {
    // std::cout << "[parser log] while block\n";

    if (!Eat(Lexer::Tokens::T_WHILE)) { throw MakeError<ParserError>(); }

    auto condition = std::make_unique<BinaryOp>(
        Lexer::Tokens::T_COMP_EQUAL,
        Expr(),
        std::make_unique<BoolLiteral>(true, token),
        token
    );

    While while_block = While(std::move(condition), Block(), GetTraitedToken());
    if (!Eat(Lexer::Tokens::T_END_WHILE)) { throw MakeError<Errors::ParserErrors::ExpectedEndWhile>(); } GetTraitedToken();
    return std::make_unique<While>(std::move(while_block));
}

// FuncExpr: T_FUNC '(' (T_VAR (',' T_VAR)*)? ')' BLOCK T_END_FUNC
Parser::NodePtr Parser::FuncExpr() {
    // std::cout << "[parser log] parser func expression\n";

    if (!Eat(Lexer::Tokens::T_FUNC)) { throw MakeError<ParserError>(); }

    if (!Eat(Lexer::Tokens::T_LEFT_BRACKET)) {
        throw MakeError<Errors::ParserErrors::ExpectedLeftBracket>();
    } GetTraitedToken();

    std::vector<Var> args;

    if (token.token != Lexer::Tokens::T_RIGHT_BRACKET) {
        bool was_comma = false;
        do {
            if (was_comma) { GetTraitedToken(); }
            switch (token.token) {
                case Lexer::Tokens::T_VAR:
                {   std::string id = std::get<std::string>(token.value);
                    if (!Eat(token.token)) { throw MakeError<ParserError>(); }
                    args.push_back(Var(std::move(id), GetTraitedToken())); }
                    break;
                default:
                    throw MakeError<Errors::ParserErrors::FunctionParamsError>();
            }
            was_comma = true;
        } while (Eat(Lexer::Tokens::T_COMMA));
    }

    if (!Eat(Lexer::Tokens::T_RIGHT_BRACKET)) {
        throw MakeError<Errors::ParserErrors::ExpectedRightBracket>();
    } GetTraitedToken();

    NodePtr func = std::make_unique<Func>(std::move(args), Block(), GetTraitedToken());

    if (!Eat(Lexer::Tokens::T_END_FUNC)) {
        throw MakeError<Errors::ParserErrors::ExpectedEndFunc>();
    } GetTraitedToken();

    return std::move(func);
}

// StatementList: If | While | For | Statement
Parser::NodePtr Parser::StatementList() {
    // std::cout << "[parser log] choose between statements\n";

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

// Block: (StatementList)? (T_EOL | StatementList)*
Parser::NodePtr Parser::Block() {
    // std::cout << "[parser log] come in block\n";

    Compound node;
    while ( token.token != Lexer::Tokens::T_ELSE_IF && token.token != Lexer::Tokens::T_ELSE
    && token.token != Lexer::Tokens::T_END_IF && token.token != Lexer::Tokens::T_END_WHILE
    && token.token != Lexer::Tokens::T_END_FOR && token.token != Lexer::Tokens::T_END_FUNC
    && token.token != Lexer::Tokens::T_EOF) {
        if (token.token == Lexer::Tokens::T_EOL) {
            if (!Eat(token.token)) { throw MakeError<ParserError>(); } GetTraitedToken();
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
    try {
        return Assignment();
    } catch (const Errors::ParserErrors::ExpectedAssignment&) {
        token = std::move(saved_token);
        tokens_traits = std::move(saved_tokens_traits);
        tokenizer = std::move(saved_tokenizer);
        return Expr();
    }
}

void Parser::Parse() {
    tokenizer >> token;
    root = Block();
}
