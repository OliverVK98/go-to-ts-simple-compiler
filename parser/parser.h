//
// Created by oliver on 3/21/24.
//

#ifndef GO_TO_TS_SIMPLE_COMPILER_PARSER_H
#define GO_TO_TS_SIMPLE_COMPILER_PARSER_H

#include <unordered_map>
#include "../token/token.h"
#include <functional>
#include <memory>
#include "../ast/ast.h"
#include "../lexer/lexer.h"

enum Precedence {
    LOWEST = 1,
    EQUALS,
    LESSORGREATER,
    SUM,
    PRODUCT,
    PREFIX,
    CALL,
    INDEX
};

extern std::unordered_map<TokenType, int> precedences;
using prefixParseFn = std::function<std::unique_ptr<Expression>()>;
using infixParseFn = std::function<std::unique_ptr<Expression>(std::unique_ptr<Expression>)>;

struct Parser {
    Lexer* lexer;
    std::vector<std::string> errors{};

    Token currentToken;
    Token nextToken;

    std::unordered_map<TokenType, prefixParseFn> prefixParseFns;
    std::unordered_map<TokenType, infixParseFn> infixParseFns;

    inline void getNextToken() { currentToken = nextToken; nextToken = lexer->nextToken(); }
    inline bool currentTokenIs(const TokenType &t) { return currentToken.Type == t; }
    inline bool nextTokenIs(const TokenType &t) { return nextToken.Type == t; }
    bool expectNextTokenIs(const Token& t);
    inline std::vector<std::string> getErrors() const { return errors; }
    Precedence peekPrecedence();

    Program parseProgram();
    std::unique_ptr<Expression> parseExpression(const int &precedence);
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<ConstStatement> parseConstStatement();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<ExpressionStatement> parseExpressionStatement();
};

#endif //GO_TO_TS_SIMPLE_COMPILER_PARSER_H
