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

struct Program : public Node {
    std::vector<std::unique_ptr<Node>> nodes;
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
    Identifier(Token token, std::string value) : token(token), value(value) {}

    inline std::string string() override { return token.Literal; }
};

struct Integer : public Node {
    Token token;
    int value;
    Integer(Token token) : token(token) {};

    inline std::string string() override { return token.Literal; }
};

struct String : public Node {
    Token token;
    std::string value;
    String(Token token, std::string value) : token(token), value(value) {};

    inline std::string string() override { return token.Literal; }
};

struct Boolean : public Node {
    Token token;
    bool value;
    Boolean(Token token, bool value) : token(token), value(value) {};

    inline std::string string() override { return token.Literal; }
};

struct ConstStatement : public Node {
    Token token;
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Node> value;

    ConstStatement(Token &token) : token(token) {};

    std::string string() override;
};

struct ReturnStatement : public Node {
    Token token;
    std::unique_ptr<Node> value;

    ReturnStatement(Token &token) : token(token) {};

    std::string string() override;
};

struct ExpressionStatement : public Node {
    Token token;
    std::unique_ptr<Node> value;

    ExpressionStatement(Token &token) : token(token) {};

    std::string string() override;
};

struct CodeBlock : public Node {
    Token token;
    std::vector<std::unique_ptr<Node>> nodes;
    CodeBlock(Token& token) : token(token) {};

    std::string string() override;
};

struct IfElseNode : public Node {
    Token token;
    std::unique_ptr<Node> condition;
    std::unique_ptr<CodeBlock> consequence;
    std::unique_ptr<CodeBlock> alternative;
    IfElseNode(Token& token) : token(token) {};

    std::string string() override;
};

struct Prefix : public Node {
    Token token;
    std::string Operator;
    std::unique_ptr<Node> right;

    Prefix(Token token, std::string Operator) : token(token), Operator(Operator) {};

    std::string string() override;
};

struct Infix : public Node {
    Token token;
    std::string Operator;
    std::unique_ptr<Node> right;
    std::unique_ptr<Node> left;

    Infix(Token token, std::string Operator, std::unique_ptr<Node> left) :
        token(token), Operator(Operator), left(std::move(left)) {};

    std::string string() override;
};

struct Function : public Node {
    Token token;
    std::vector<std::unique_ptr<Identifier>> parameters;
    std::unique_ptr<CodeBlock> body;
    std::string funcName;
    Function(Token& token) : token(token) {};

    std::string string() override;
};

struct FunctionCall : public Node {
    Token token;
    std::unique_ptr<Node> func;
    std::vector<std::unique_ptr<Node>> args;
    FunctionCall(Token& token, std::unique_ptr<Node> func) : token(token), func(std::move(func)) {};

    std::string string() override;
};

struct Array : public Node {
    Token token;
    std::vector<std::unique_ptr<Node>> elements;
    Array(Token& token) : token(token) {};

    std::string string() override;
};

struct Index : public Node {
    Token token;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> index;
    Index(Token& token, std::unique_ptr<Node> left) : token(token), left(std::move(left)) {};

    std::string string() override;
};

#endif //GO_TO_TS_SIMPLE_COMPILER_AST_H
