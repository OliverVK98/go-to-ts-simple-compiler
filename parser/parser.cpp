//
// Created by oliver on 3/21/24.
//

#include "parser.h"

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

    if (!nextTokenIs(ASSIGN)) {
        return nullptr;
    }

    getNextToken();

    stmt->value = parseExpression(LOWEST);

    // addFunctionSupport

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

std::unique_ptr<Expression> Parser::parseExpression(const int &precedence) {
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
        leftExp = std::unique_ptr<Expression>(infix(std::move(leftExp)));
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

//Program Parser::parseProgram() {
//    Program program{};
//    program.Statements = std::vector<Statement*>{};
//
//    while (!currentTokenIs(END_OF_FILE)) {
//        auto statement = parseStatement();
//        if (statement) {
//            program.Statements = program.Statements.push_back(statement);
//        }
//        getNextToken();
//    }
//
//    return program;
//}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (currentToken.Type == CONST) {
        return parseConstStatement();
    } else if (currentToken.Type == RETURN) {
        return parseReturnStatement();
    } else {
        return parseExpressionStatement();
    }
}



