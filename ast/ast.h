//
// Created by oliver on 3/18/24.
//

#ifndef GO_TO_TS_SIMPLE_COMPILER_AST_H
#define GO_TO_TS_SIMPLE_COMPILER_AST_H

#include <utility>
#include <vector>
#include <sstream>
#include <memory>
#include "string"
#include "../token/token.h"

struct Node {
    virtual ~Node() = default;
    virtual std::string string() = 0;
};

struct Statement : public Node {
    Statement() = default;
    ~Statement() override = default;
};

//struct Expression : public Node {
//    ~Expression() override = default;
//};

struct Program : public Node {
    std::vector<std::unique_ptr<Statement>> statements;
    Program() = default;
    ~Program() override = default;
    Program(const Program&) = delete;
    Program& operator=(const Program&) = delete;
    Program(Program&&) noexcept = default;
    Program& operator=(Program&&) noexcept = default;

    std::string string() override;
};

struct Identifier : public Node {
    Token token;
    std::string value;
    Identifier(Token token, std::string value) : token(std::move(token)), value(std::move(value)) {}

    inline std::string string() override { return token.Literal; }
};

struct IntegerLiteral : public Node {
    Token token;
    int value;
    IntegerLiteral(Token token) : token(std::move(token)) {};

    inline std::string string() override { return token.Literal; }
};

struct StringLiteral : public Node {
    Token token;
    std::string value;
    StringLiteral(Token token, std::string value) : token(std::move(token)), value(std::move(value)) {};

    inline std::string string() override { return token.Literal; }
};

struct Boolean : public Node {
    Token token;
    bool value;
    Boolean(Token token, bool value) : token(std::move(token)), value(value) {};

    inline std::string string() override { return token.Literal; }
};

struct ConstStatement : public Statement {
    Token token;
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Node> value;

    ConstStatement(Token &token) : token(token) {};

    std::string string() override;
};

struct ReturnStatement : public Statement {
    Token token;
    std::unique_ptr<Node> value;

    ReturnStatement(Token &token) : token(token) {};

    std::string string() override;
};

struct ExpressionStatement : public Statement {
    Token token;
    std::unique_ptr<Node> value;

    ExpressionStatement(Token &token) : token(token) {};

    std::string string() override;
};

struct BlockStatement : public Node {
    Token token;
    std::vector<std::unique_ptr<Statement>> statements;

    std::string string() override;
};

struct IfExpressionStatement : public Statement {
    Token token;
    Node* condition;
    BlockStatement* consequence;
    BlockStatement* alternative;

    std::string string() override;
};

struct PrefixExpression : public Node {
    Token token;
    std::string Operator;
    std::unique_ptr<Node> right;

    PrefixExpression(Token token, std::string Operator) : token(std::move(token)), Operator(std::move(Operator)) {};

    std::string string() override;
};

struct InfixExpression : public Node {
    Token token;
    std::string Operator;
    std::unique_ptr<Node> right;
    std::unique_ptr<Node> left;

    InfixExpression(Token token, std::string Operator, std::unique_ptr<Node> left) :
        token(std::move(token)), Operator(std::move(Operator)), left(std::move(left)) {};

    std::string string() override;
};

struct Function : public Node {
    Token token;
    std::vector<Identifier> parameters;
    BlockStatement* body;
    std::string funcName;

    std::string string() override;
};

#endif //GO_TO_TS_SIMPLE_COMPILER_AST_H
