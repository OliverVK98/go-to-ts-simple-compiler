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

std::string boolToString(bool boolV);

struct TypeNode {
    virtual ~TypeNode() = default;


    virtual TokenType getType() = 0;
    virtual TokenType getSubType() = 0;
    virtual std::unique_ptr<TypeNode> clone() const = 0;
};

struct StringType : public TypeNode {
    TokenType getType() override  {return STRING_TYPE;}
    TokenType getSubType() override  {return NOTYPE_TYPE;}
    std::unique_ptr<TypeNode> clone() const override {
        return std::make_unique<StringType>(*this);
    }
};

struct IntegerType : public TypeNode {
    TokenType getType() override  {return INT_TYPE;}
    TokenType getSubType() override  {return NOTYPE_TYPE;}
    std::unique_ptr<TypeNode> clone() const override {
        return std::make_unique<IntegerType>(*this);
    }
};

struct BoolType : public TypeNode {
    TokenType getType() override  {return BOOL_TYPE;}
    TokenType getSubType() override  {return NOTYPE_TYPE;}
    std::unique_ptr<TypeNode> clone() const override {
        return std::make_unique<BoolType>(*this);
    }
};

struct ArrayType : public TypeNode {
    std::unique_ptr<TypeNode> subType;
    ArrayType(std::unique_ptr<TypeNode> type) : subType(std::move(type)) {};
    ArrayType() {};
    TokenType getType() override  {return ARRAY_TYPE;}
    TokenType getSubType() override  {return subType->getType();}
    std::unique_ptr<TypeNode> clone() const override {
        return std::make_unique<ArrayType>(subType ? subType->clone() : nullptr);
    }
};

struct Node {
    bool holdsValue = true;
    bool holdsMultipleValues = false;
    std::unique_ptr<TypeNode> type;

    virtual ~Node() = default;
    virtual std::string testString() = 0;
    virtual std::string string() = 0;
    inline void setHoldsValue(bool holds) {holdsValue = holds;}
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
    std::string testString() override;
};

struct Identifier : public Node {
    Token token;
    std::string value;
    std::unique_ptr<TypeNode> type;
    std::string name;
    Identifier(Token token, std::string value) : token(token), name(token.Literal), value(value) {}

    inline std::string string() override {return value;}
    inline std::string testString() override { return "Identifier(" + token.Literal + ")"; }
};

struct Integer : public Node {
    Token token;
    int value;
    Integer(Token token) : token(token) {};
    Integer() = default;

    inline std::string string() override {return std::to_string(value);}
    inline std::string testString() override { return "Integer(" + token.Literal + ")"; }
};

struct String : public Node {
    Token token;
    std::string value;
    String(Token token, std::string value) : token(token), value(value) {};
    String() = default;

    inline std::string string() override {return "\"" + value + "\"";}
    inline std::string testString() override { return "String(" + token.Literal + ")"; }
};

struct Boolean : public Node {
    Token token;
    bool value;
    Boolean(Token token, bool value) : token(token), value(value) {};
    Boolean() = default;

    inline std::string string() override {return boolToString(value);}
    inline std::string testString() override { return "Boolean(" + token.Literal + ")"; }
};

struct Declaration : public Node {
    Token token;
    std::unique_ptr<Node> name;
    std::unique_ptr<Node> value;
    std::vector<std::unique_ptr<Declaration>> multipleValues;
    std::unique_ptr<TypeNode> type;
    bool isConstant = false;

    Declaration(Token &token) : token(token) {};
    Declaration() {};

    inline std::string string() override {return token.Literal;}
    std::string testString() override {return "Declaration: " + token.Literal;}
};

struct ReturnNode : public Node {
    Token token;
    std::unique_ptr<Node> value;

    ReturnNode(Token &token) : token(token) {};

    inline std::string string() override {return token.Literal;}
    std::string testString() override;
};

struct RValue : public Node {
    Token token;
    std::unique_ptr<Node> value;

    RValue(Token &token) : token(token) {};

    inline std::string string() override {return token.Literal;}
    std::string testString() override;
};

struct CodeBlock : public Node {
    Token token;
    std::vector<std::unique_ptr<Node>> nodes;
    CodeBlock(Token& token) : token(token) {};

    inline std::string string() override {return token.Literal;}
    std::string testString() override;
};

struct IfElseNode : public Node {
    Token token;
    std::unique_ptr<Node> condition;
    std::unique_ptr<CodeBlock> consequence;
    std::unique_ptr<CodeBlock> alternative;
    IfElseNode(Token& token) : token(token) {};

    inline std::string string() override {return token.Literal;}
    std::string testString() override;
};

struct Prefix : public Node {
    Token token;
    std::string Operator;
    std::unique_ptr<Node> right;

    Prefix(Token token, std::string Operator) : token(token), Operator(Operator) {};

    inline std::string string() override;
    std::string testString() override;
};

struct Infix : public Node {
    Token token;
    std::string Operator;
    std::unique_ptr<Node> right;
    std::unique_ptr<Node> left;

    Infix(Token token, std::string Operator, std::unique_ptr<Node> left) :
        token(token), Operator(Operator), left(std::move(left)) {};

    std::string string() override;
    std::string testString() override;
};

struct Function : public Node {
    Token token;
    std::vector<std::unique_ptr<Identifier>> parameters;
    std::unique_ptr<CodeBlock> body;
    std::string funcName;
    std::unique_ptr<TypeNode> type;
    Function(Token& token) : token(token) {};

    inline std::string string() override {return token.Literal;}
    std::string testString() override;
};

struct FunctionCall : public Node {
    Token token;
    std::string funcName;
    std::vector<std::unique_ptr<Node>> args;
    FunctionCall(Token& token, std::string funcName) : token(token), funcName(std::move(funcName)) {};

    inline std::string string() override {return token.Literal;}
    std::string testString() override;
};

struct Array : public Node {
    Token token;
    std::vector<std::unique_ptr<Node>> elements;
    std::unique_ptr<TypeNode> type;
    Array(Token& token) : token(token) {};
    Array(Token& token, std::unique_ptr<TypeNode> type) : token(token), type(std::move(type)) {};
    Array() {};

    std::string string() override;
    std::string testString() override;
};

struct Index : public Node {
    Token token;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> index;
    Index(Token& token, std::unique_ptr<Node> left) : token(token), left(std::move(left)) {};

    inline std::string string() override {return token.Literal;}
    std::string testString() override;
};

#endif //GO_TO_TS_SIMPLE_COMPILER_AST_H
