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

bool Parser::checkNextTokenAndAdvance(const TokenType& t) {
    if (nextTokenIs(t)) {
        getNextToken();
        return true;
    };
    return false;
}

std::unique_ptr<ConstStatement> Parser::parseConstStatement() {
    auto node = std::make_unique<ConstStatement>(currentToken);
    if (!checkNextTokenAndAdvance(IDENTIFIER)) {
        return nullptr;
    }

    node->name = std::make_unique<Identifier>(currentToken, currentToken.Literal);

    if (!checkNextTokenAndAdvance(ASSIGN)) {
        return nullptr;
    }

    getNextToken();

    node->value = parseExpression(LOWEST);

    if (auto fl = dynamic_cast<Function*>(node->value.get())) {
        fl->funcName = node->name->value;
    }

    return node;
}

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
    auto node = std::make_unique<ReturnStatement>(currentToken);

    getNextToken();

    node->value = parseExpression(LOWEST);

    if (nextTokenIs(SEMICOLON)) {
        getNextToken();
    }

    return node;
}

std::unique_ptr<Node> Parser::parseExpressionStatement() {
    auto node = std::make_unique<ExpressionStatement>(currentToken);

    node->value = parseExpression(LOWEST);

    if (nextTokenIs(SEMICOLON)) {
        getNextToken();
    }

    return node;
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

Precedence Parser::peekPrecedence() const {
    auto it = precedences.find(nextToken.Type);
    if (it != precedences.end()) {
        return static_cast<Precedence>(it->second);
    }
    return Precedence::LOWEST;
}

Precedence Parser::currentPrecedence() const {
    auto it = precedences.find(currentToken.Type);
    if (it != precedences.end()) {
        return static_cast<Precedence>(it->second);
    }
    return Precedence::LOWEST;
}


Program Parser::parseProgram() {
    Program program{};
    program.nodes = std::move(std::vector<std::unique_ptr<Node>>{});

    while (!currentTokenIs(END_OF_FILE)) {
        auto statement = parseStatement();

        if (statement) {
            program.nodes.emplace_back(std::move(statement));
        }
        getNextToken();
    }

    return program;
}

std::unique_ptr<Integer> Parser::parseIntegerLiteral() {
    auto intLit = std::make_unique<Integer>(currentToken);
    intLit->value = std::stoi(currentToken.Literal);
    return intLit;
}

std::unique_ptr<Node> Parser::parseStatement() {
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

std::unique_ptr<Prefix> Parser::parsePrefixExpression() {
    auto expression = std::make_unique<Prefix>(currentToken, currentToken.Literal);
    getNextToken();
    expression->right = parseExpression(PREFIX);
    return expression;
}

std::unique_ptr<Infix> Parser::parseInfixExpression(std::unique_ptr<Node> left) {
    auto expression = std::make_unique<Infix>(currentToken, currentToken.Literal, std::move(left));
    auto precedence = currentPrecedence();
    getNextToken();
    expression->right = parseExpression(precedence);
    return expression;
}

std::unique_ptr<CodeBlock> Parser::parseBlockStatement() {
    auto block = std::make_unique<CodeBlock>(currentToken);
    block->nodes = std::move(std::vector<std::unique_ptr<Node>>{});

    getNextToken();

    while (!currentTokenIs(RBRACE) && !currentTokenIs(END_OF_FILE)) {
        auto node = parseStatement();
        if (node) {
            block->nodes.emplace_back(std::move(node));
        }
        getNextToken();
    }

    return block;
}

std::unique_ptr<Function> Parser::parseFunctionLiteral() {
    auto func = std::make_unique<Function>(currentToken);

    if(!checkNextTokenAndAdvance(LPAREN)) {
        return nullptr;
    }

    func->parameters = parseFunctionParameters();

    if(!checkNextTokenAndAdvance(LBRACE)) {
        return nullptr;
    }

    func->body = parseBlockStatement();

    return func;
}

std::vector<std::unique_ptr<Identifier>> Parser::parseFunctionParameters() {
    auto params = std::vector<std::unique_ptr<Identifier>>{};

    if (nextTokenIs(RPAREN)) {
        getNextToken();
        return params;
    }

    getNextToken();

    auto param = std::make_unique<Identifier>(currentToken, currentToken.Literal);
    params.emplace_back(std::move(param));

    while (nextTokenIs(COMMA)) {
        getNextToken();
        getNextToken();
        auto newParam = std::make_unique<Identifier>(currentToken, currentToken.Literal);
        params.emplace_back(std::move(newParam));
    }

    if (!checkNextTokenAndAdvance(RPAREN)) {
        return std::vector<std::unique_ptr<Identifier>>{};
    }

    return params;
}

std::unique_ptr<Node> Parser::parseGroupedExpression() {
    getNextToken();

    auto expr = parseExpression(LOWEST);

    if (!checkNextTokenAndAdvance(RPAREN)) {return nullptr;}

    return expr;
}

std::unique_ptr<IfElseNode> Parser::parseIfNode() {
    auto ifNode = std::make_unique<IfElseNode>(currentToken);

    if (!checkNextTokenAndAdvance(LPAREN)) {return nullptr;}

    getNextToken();
    ifNode->condition = std::move(parseExpression(LOWEST));

    if (!checkNextTokenAndAdvance(RPAREN)) {return nullptr;}
    if (!checkNextTokenAndAdvance(LBRACE)) {return nullptr;}

    ifNode->consequence = std::move(parseBlockStatement());

    if (nextTokenIs(ELSE)) {
        getNextToken();

        if (!checkNextTokenAndAdvance(LBRACE)) {return nullptr;}

        ifNode->alternative = std::move(parseBlockStatement());
    }

    return ifNode;
}

std::unique_ptr<FunctionCall> Parser::parseFunctionCall(std::unique_ptr<Node> func) {
    auto funcCall = std::make_unique<FunctionCall>(currentToken, std::move(func));
    funcCall->args = std::move(parseExpressionList(RPAREN));

    return funcCall;
}

std::vector<std::unique_ptr<Node>> Parser::parseExpressionList(TokenType end) {
    auto list = std::vector<std::unique_ptr<Node>>{};
    if (nextTokenIs(end)) {
        getNextToken();
        return list;
    }

    getNextToken();
    list.push_back(std::move(parseExpression(LOWEST)));

    while (nextTokenIs(COMMA)) {
        getNextToken();
        getNextToken();
        list.push_back(std::move(parseExpression(LOWEST)));
    }

    if (!checkNextTokenAndAdvance(end)) {
        return std::vector<std::unique_ptr<Node>>{};
    }

    return list;
}

std::unique_ptr<Node> Parser::parseArray() {
    auto array = std::make_unique<Array>(currentToken);
    array->elements = std::move(parseExpressionList(RBRACKET));
    return array;
}

std::unique_ptr<Node> Parser::parseIndex(std::unique_ptr<Node> left) {
    auto indexNode = std::make_unique<Index>(currentToken, std::move(left));
    getNextToken();
    indexNode->index = std::move(parseExpression(LOWEST));

    if (!checkNextTokenAndAdvance(RBRACKET)) {
        return nullptr;
    }

    return indexNode;
}

Parser::Parser(Lexer* l) : lexer(l) {
    registerPrefix(IDENTIFIER, [this]() { return this->parseIdentifier(); });
    registerPrefix(INT, [this]() { return this->parseIntegerLiteral(); });
    registerPrefix(STRING, [this]() { return this->parseStringLiteral(); });
    registerPrefix(BANG, [this]() { return this->parsePrefixExpression(); });
    registerPrefix(MINUS, [this]() { return this->parsePrefixExpression(); });
    registerPrefix(TRUE, [this]() { return this->parseBoolean(); });
    registerPrefix(FALSE, [this]() { return this->parseBoolean(); });
    registerPrefix(LPAREN, [this]() { return this->parseGroupedExpression(); });
    registerPrefix(IF, [this]() { return this->parseIfNode(); });
    registerPrefix(FUNCTION, [this]() { return this->parseFunctionLiteral(); });
    registerPrefix(LBRACKET, [this]() { return this->parseArray(); });

    registerInfix(PLUS, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });
    registerInfix(MINUS, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });
    registerInfix(SLASH, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });
    registerInfix(ASTERISK, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });
    registerInfix(EQ, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });
    registerInfix(NOT_EQ, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });
    registerInfix(LESS_THAN, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });
    registerInfix(GREATER_THAN, [this](std::unique_ptr<Node> left) { return this->parseInfixExpression(std::move(left)); });
    registerInfix(LPAREN, [this](std::unique_ptr<Node> left) { return this->parseFunctionCall(std::move(left)); });
    registerInfix(LBRACKET, [this](std::unique_ptr<Node> left) { return this->parseIndex(std::move(left)); });

    getNextToken();
    getNextToken();
}