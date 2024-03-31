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

class Parser {
public:
    explicit Parser(Lexer* l);
    Program parseProgram();

private:
    Lexer* lexer;
    std::vector<std::string> errors{};
    Token currentToken;
    Token nextToken;
    std::unordered_map<TokenType, prefixParseFn> prefixParseFns;
    std::unordered_map<TokenType, infixParseFn> infixParseFns;

    inline void getNextToken() { currentToken = nextToken; nextToken = lexer->nextToken(); }
    inline bool currentTokenIs(const TokenType &t) const { return currentToken.Type == t; }
    inline bool nextTokenIs(const TokenType &t) const { return nextToken.Type == t; }
    bool checkNextTokenAndAdvance(const TokenType& t);
    inline std::vector<std::string> getErrors() const { return errors; }

    Precedence peekPrecedence() const;
    Precedence currentPrecedence() const;

    void registerPrefix(const TokenType& tokenType, prefixParseFn fn);
    void registerInfix(const TokenType& tokenType, infixParseFn fn);

    std::unique_ptr<Node> parseExpression(const int &precedence);
    std::unique_ptr<Node> parseStatement();
    std::unique_ptr<ConstStatement> parseConstStatement();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<Node> parseExpressionStatement();
    inline std::unique_ptr<Identifier> parseIdentifier() { return std::make_unique<Identifier>(currentToken, currentToken.Literal); }
    std::unique_ptr<Integer> parseIntegerLiteral();
    inline std::unique_ptr<String> parseStringLiteral() { return std::make_unique<String>(currentToken, currentToken.Literal); }
    inline std::unique_ptr<Boolean> parseBoolean() { return std::make_unique<Boolean>(currentToken, currentTokenIs(TRUE)); }
    std::unique_ptr<Prefix> parsePrefixExpression();
    std::unique_ptr<Infix> parseInfixExpression(std::unique_ptr<Node> left);
    std::unique_ptr<CodeBlock> parseBlockStatement();
    std::unique_ptr<Function> parseFunctionLiteral();
    std::vector<std::unique_ptr<Identifier>> parseFunctionParameters();
    std::unique_ptr<Node> parseGroupedExpression();
    std::unique_ptr<IfElseNode> parseIfNode();
    std::unique_ptr<FunctionCall> parseFunctionCall(std::unique_ptr<Node> func);
    std::vector<std::unique_ptr<Node>> parseExpressionList(TokenType end);
    std::unique_ptr<Node> parseArray();
    std::unique_ptr<Node> parseIndex(std::unique_ptr<Node> left);
};

#endif //GO_TO_TS_SIMPLE_COMPILER_PARSER_H
