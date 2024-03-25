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
using prefixParseFn = std::function<std::unique_ptr<Node>()>;
using infixParseFn = std::function<std::unique_ptr<Node>(std::unique_ptr<Node>)>;

struct Parser {
    explicit Parser(Lexer* l);

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
    Precedence currentPrecedence();

    void registerPrefix(const TokenType& tokenType, prefixParseFn fn);
    void registerInfix(const TokenType& tokenType, infixParseFn fn);

    Program parseProgram();
    std::unique_ptr<Node> parseExpression(const int &precedence);
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<ConstStatement> parseConstStatement();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<ExpressionStatement> parseExpressionStatement();
    inline std::unique_ptr<Node> parseIdentifier() { return std::make_unique<Identifier>(currentToken, currentToken.Literal); }
    std::unique_ptr<Node> parseIntegerLiteral();
    inline std::unique_ptr<Node> parseStringLiteral() { return std::make_unique<StringLiteral>(currentToken, currentToken.Literal); }
    inline std::unique_ptr<Node> parseBoolean() { return std::make_unique<Boolean>(currentToken, currentTokenIs(TRUE)); }
    std::unique_ptr<Node> parsePrefixExpression();
    std::unique_ptr<Node> parseInfixExpression(std::unique_ptr<Node> left);
};

#endif //GO_TO_TS_SIMPLE_COMPILER_PARSER_H
