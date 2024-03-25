//
// Created by oliver on 3/21/24.
//

#include "parser.h"

#include <utility>

std::unordered_map<TokenType, int> precedences = {
        {EQ,       EQUALS},
        {NOT_EQ,   EQUALS},
        {LESS_THAN,       LESSORGREATER},
        {GREATER_THAN,       LESSORGREATER},
        {PLUS,     SUM},
        {MINUS,    SUM},
        {SLASH,    PRODUCT},
        {ASTERISK, PRODUCT},
        {LPAREN,   CALL},
        {LBRACKET, INDEX},
};

bool Parser::expectNextTokenIs(const Token& t) {
    if (nextTokenIs(t.Type)) {
        getNextToken();
        return true;
    };
    return false;
}

std::unique_ptr<ConstStatement> Parser::parseConstStatement() {
    auto stmt = std::make_unique<ConstStatement>(currentToken);

    if (!nextTokenIs(IDENTIFIER)) {
        return nullptr;
    }

    stmt->name = std::make_unique<Identifier>(currentToken, currentToken.Literal);

    if (!nextTokenIs(ASSIGN)) {
        return nullptr;
    }

    getNextToken();

    stmt->value = parseExpression(LOWEST);

    // addFunctionSupport
    if (auto fl = dynamic_cast<Function*>(stmt->value.get())) {
        fl->funcName = stmt->name->value;
    }

    return stmt;
}

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
    auto stmt = std::make_unique<ReturnStatement>(currentToken);

    getNextToken();

    stmt->value = parseExpression(LOWEST);

    if (nextTokenIs(SEMICOLON)) {
        getNextToken();
    }

    return stmt;
}

std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    auto stmt = std::make_unique<ExpressionStatement>(currentToken);

    stmt->value = parseExpression(LOWEST);

    if (nextTokenIs(SEMICOLON)) {
        getNextToken();
    }

    return stmt;
}

std::unique_ptr<Node> Parser::parseExpression(const int &precedence) {
    auto prefix = prefixParseFns[currentToken.Type];

    if (!prefix) {
        return nullptr;
    }

    auto leftExp = prefix();

    while (!nextTokenIs(SEMICOLON) && precedence < peekPrecedence()) {
        auto infix = infixParseFns[nextToken.Type];
        if (!infix) {
            return nullptr;
        }

        getNextToken();
        leftExp = std::unique_ptr<Node>(infix(std::move(leftExp)));
    };

    return leftExp;
}

Precedence Parser::peekPrecedence() {
    auto it = precedences.find(nextToken.Type);
    if (it != precedences.end()) {
        return static_cast<Precedence>(it->second);
    }
    return Precedence::LOWEST;
}

Precedence Parser::currentPrecedence() {
    auto it = precedences.find(currentToken.Type);
    if (it != precedences.end()) {
        return static_cast<Precedence>(it->second);
    }
    return Precedence::LOWEST;
}


Program Parser::parseProgram() {
    Program program{};
    program.statements = std::move(std::vector<std::unique_ptr<Statement>>{});

    while (!currentTokenIs(END_OF_FILE)) {
        auto statement = parseStatement();
        if (statement) {
            program.statements.emplace_back(std::move(statement));
        }
        getNextToken();
    }

    return program;
}

std::unique_ptr<Node> Parser::parseIntegerLiteral() {
    auto intLit = std::make_unique<IntegerLiteral>(currentToken);
    intLit->value = std::stoi(currentToken.Literal);
    return intLit;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (currentToken.Type == CONST) {
        return parseConstStatement();
    } else if (currentToken.Type == RETURN) {
        return parseReturnStatement();
    } else {
        return parseExpressionStatement();
    }
}

void Parser::registerPrefix(const TokenType& tokenType, prefixParseFn fn) {
    prefixParseFns[tokenType] = std::move(fn);
}

void Parser::registerInfix(const TokenType& tokenType, infixParseFn fn) {
    infixParseFns[tokenType] = std::move(fn);
}

std::unique_ptr<Node> Parser::parsePrefixExpression() {
    auto expression = std::make_unique<PrefixExpression>(currentToken, currentToken.Literal);
    getNextToken();
    expression->right = parseExpression(PREFIX);
    return expression;
}

std::unique_ptr<Node> Parser::parseInfixExpression(std::unique_ptr<Node> left) {
    auto expression = std::make_unique<InfixExpression>(currentToken, currentToken.Literal, std::move(left));
    auto precedence = currentPrecedence();
    getNextToken();
    expression->right = parseExpression(precedence);
    return expression;
}

Parser::Parser(Lexer* l) : lexer(l) {
    registerPrefix(IDENTIFIER, [this]() { return this->parseIdentifier(); });
    registerPrefix(INT, [this]() { return this->parseIntegerLiteral(); });
    registerPrefix(STRING, [this]() { return this->parseStringLiteral(); });
    registerPrefix(BANG, [this]() { return this->parsePrefixExpression(); });
    registerPrefix(MINUS, [this]() { return this->parsePrefixExpression(); });
    registerPrefix(TRUE, [this]() { return this->parseBoolean(); });
    registerPrefix(FALSE, [this]() { return this->parseBoolean(); });

    registerInfix(PLUS, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });
    registerInfix(MINUS, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });
    registerInfix(SLASH, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });
    registerInfix(ASTERISK, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });
    registerInfix(EQ, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });
    registerInfix(NOT_EQ, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });
    registerInfix(LESS_THAN, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });
    registerInfix(GREATER_THAN, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });

    getNextToken();
    getNextToken();
}

