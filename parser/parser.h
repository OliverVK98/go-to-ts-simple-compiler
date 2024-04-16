//
// Created by oliver on 3/21/24.
//

#ifndef GO_TO_TS_SIMPLE_COMPILER_PARSER_H
#define GO_TO_TS_SIMPLE_COMPILER_PARSER_H

#include <unordered_map>
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
enum DeclarationType { VAR_DECL, CONST_DECL, SHORT_DECL };

bool startsWithType(const std::string& str);

class Parser {
public:
    explicit Parser(Lexer* l);
    std::unique_ptr<Program> parseProgram();

private:
    Lexer* lexer;
    std::vector<std::string> errors{};
    Token currentToken;
    Token nextToken;
    std::unordered_map<TokenType, prefixParseFn> prefixParseFns;
    std::unordered_map<TokenType, infixParseFn> infixParseFns;

    inline void getNextToken() { currentToken = nextToken; nextToken = lexer->nextToken(); }
    inline void getNextToken(int n) {
        for (int i = 0; i < n; i++) {
            getNextToken();
        }
    }

    inline bool currentTokenIs(const TokenType &t) const { return currentToken.Type == t; }
    inline bool nextTokenIs(const TokenType &t) const { return nextToken.Type == t; }
    inline bool tokenTypeIsTypeNode(const TokenType& t) const { return t == BOOL_TYPE || t == STRING_TYPE || t == INT_TYPE || t == ARRAY_TYPE; }
    bool checkNextTokenAndAdvance(const TokenType& t);
    inline std::vector<std::string> getErrors() const { return errors; }

    Precedence peekPrecedence() const;
    Precedence currentPrecedence() const;

    void registerPrefix(const TokenType& tokenType, prefixParseFn fn);
    void registerInfix(const TokenType& tokenType, infixParseFn fn);

    std::unique_ptr<Node> parseRValue(const int &precedence);
    std::unique_ptr<Node> parseNode();
    std::unique_ptr<ReturnNode> parseReturnNode();
    std::unique_ptr<Node> parseRValueNode();
    std::unique_ptr<Node> parseIdentifier();
    std::unique_ptr<Integer> parseIntegerLiteral();
    inline std::unique_ptr<String> parseStringLiteral() { return std::make_unique<String>(currentToken, currentToken.Literal); }
    inline std::unique_ptr<Boolean> parseBoolean() { return std::make_unique<Boolean>(currentToken, currentTokenIs(TRUE)); }
    std::unique_ptr<Prefix> parsePrefixNode();
    std::unique_ptr<Infix> parseInfixNode(std::unique_ptr<Node> left);
    std::unique_ptr<CodeBlock> parseBlockNode();
    std::unique_ptr<Function> parseFunctionDeclaration();
    std::vector<std::unique_ptr<Identifier>> parseFunctionParameters();
    std::unique_ptr<Node> parseGroupedNodes();
    std::unique_ptr<IfElseNode> parseIfNode();
    std::unique_ptr<FunctionCall> parseFunctionCall(std::unique_ptr<Node> func);
    std::vector<std::unique_ptr<Node>> parseNodeList(TokenType end);
    std::unique_ptr<Array> parseArray();
    std::unique_ptr<Node> parseIndex(std::unique_ptr<Node> left);

    // Parsing variables and their types
    std::unique_ptr<Node> parseDeclarationNode(DeclarationType declType);
    std::unique_ptr<Declaration> parseShortDeclarationNode(std::unique_ptr<Declaration>& node);
    std::unique_ptr<Declaration> parseGroupedDeclarationNode(std::unique_ptr<Declaration>& node);
    std::unique_ptr<Declaration> parseExplicitDeclarationNode(std::unique_ptr<Declaration>& node);
    std::unique_ptr<TypeNode> parseType();
    void parseImplicitVariableType(std::unique_ptr<Declaration>& node);
};

#endif //GO_TO_TS_SIMPLE_COMPILER_PARSER_H
