//
// Created by oliver on 3/18/24.
//

#ifndef GO_TO_TS_SIMPLE_COMPILER_AST_H
#define GO_TO_TS_SIMPLE_COMPILER_AST_H

#include <vector>
#include <sstream>
#include <memory>
#include "string"
#include "../token/token.h"

struct Node {
    virtual ~Node() = default;
    virtual std::string tokenLiteral() = 0;
    virtual std::string string() = 0;
};

struct Statement : public Node {
    ~Statement() override = default;
};

struct Expression : public Node {
    ~Expression() override = default;
};

struct Program : public Node {
    std::vector<Statement*> Statements;
    ~Program() override = default;

    std::string tokenLiteral() override;
    std::string string() override;
};

struct Identifier : public Node {
    Token token;
    std::string value;

    inline std::string tokenLiteral() override { return token.Literal; }
    inline std::string string() override { return token.Literal; }
};

struct ConstStatement : public Statement {
    Token token;
    Identifier* name;
    std::unique_ptr<Expression> value;

    ConstStatement(Token &token) : token(token) {};

    inline std::string tokenLiteral() override { return token.Literal; }
    std::string string() override;
};

struct ReturnStatement : public Statement {
    Token token;
    std::unique_ptr<Expression> value;

    ReturnStatement(Token &token) : token(token) {};

    inline std::string tokenLiteral() override { return token.Literal; }
    std::string string() override;
};

struct ExpressionStatement : public Statement {
    Token token;
    std::unique_ptr<Expression> value;

    ExpressionStatement(Token &token) : token(token) {};

    inline std::string tokenLiteral() override { return token.Literal; }
    std::string string() override;
};

struct IntegerLiteral : public Node {
    Token token;
    int value;

    inline std::string tokenLiteral() override { return token.Literal; }
    inline std::string string() override { return token.Literal; }
};

struct PrefixExpression : public Node {
    Token token;
    std::string Operator;
    Expression* right;

    inline std::string tokenLiteral() override { return token.Literal; }
    inline std::string string() override;
};

struct InfixExpression : public Node {
    Token token;
    std::string Operator;
    Expression* right;
    Expression* left;

    inline std::string tokenLiteral() override { return token.Literal; }
    inline std::string string() override;
};

struct Boolean : public Node {
    Token token;
    bool value;

    inline std::string tokenLiteral() override { return token.Literal; }
    inline std::string string() override { return token.Literal; }
};

#endif //GO_TO_TS_SIMPLE_COMPILER_AST_H
