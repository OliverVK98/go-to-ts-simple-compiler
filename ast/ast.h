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
    explicit ArrayType(std::unique_ptr<TypeNode> type) : subType(std::move(type)) {};
    ArrayType() = default;
    TokenType getType() override  {return ARRAY_TYPE;}
    TokenType getSubType() override  {return subType->getType();}
    std::unique_ptr<TypeNode> clone() const override {
        return std::make_unique<ArrayType>(subType ? subType->clone() : nullptr);
    }
};

struct NoType : public TypeNode {
    TokenType getType() override  {return NOTYPE_TYPE;}
    TokenType getSubType() override  {return NOTYPE_TYPE;}
    std::unique_ptr<TypeNode> clone() const override {
        return std::make_unique<NoType>(*this);
    }
};

struct Node {
    bool holdsValue = true;
    bool holdsMultipleValues = false;
    std::unique_ptr<TypeNode> type;

    virtual ~Node() = default;
    virtual std::string testString() = 0;
    virtual std::string string() = 0;
};

struct Program : public Node {
    std::vector<std::unique_ptr<Node>> nodes;
    Program() = default;
    ~Program() override = default;
    Program(const Program&) = delete;
    Program& operator=(const Program&) = delete;
    Program(Program&&) noexcept = delete;
    Program& operator=(Program&&) noexcept = delete;

    std::string string() override;
    std::string testString() override;
};

struct ValueNode : public Node {
    std::unique_ptr<TypeNode> type;

    explicit ValueNode(std::unique_ptr<TypeNode> initType) : type(std::move(initType)) {}

    ~ValueNode() override = default;
};

struct Integer : public ValueNode {
    int value{};

    Integer() : ValueNode(std::make_unique<IntegerType>()) {};

    inline std::string string() override {return std::to_string(value);}
    inline std::string testString() override { return "Integer(" + string() + ")"; }
};

struct String : public ValueNode {
    std::string value;

    explicit String(std::string val) : ValueNode(std::make_unique<StringType>()), value(std::move(val)) {}
    String() : ValueNode(std::make_unique<StringType>()) {};

    inline std::string string() override {return "\"" + value + "\"";}
    inline std::string testString() override { return "String(" + string() + ")"; }
};

struct Boolean : public ValueNode {
    bool value{};

    explicit Boolean(bool val) : ValueNode(std::make_unique<BoolType>()), value(val) {}
    Boolean() : ValueNode(std::make_unique<BoolType>()) {};

    inline std::string string() override {return boolToString(value);}
    inline std::string testString() override { return "Boolean(" + string() + ")"; }
};

struct Array : public ValueNode {
    std::vector<std::unique_ptr<Node>> elements;

    explicit Array(std::unique_ptr<TypeNode> type) : ValueNode(std::move(type)) {};
    Array() : ValueNode(std::make_unique<ArrayType>()) {};

    std::string string() override;
    std::string testString() override;
};

struct Identifier : public Node {
    std::string name;
    std::unique_ptr<TypeNode> type;
    explicit Identifier(std::string name) : name(std::move(name)) {}

    inline std::string string() override {return name;}
    inline std::string testString() override { return "Identifier(" + name + ")"; }
};

struct Declaration : public Node {
    std::unique_ptr<Node> name;
    std::unique_ptr<Node> value;
    std::vector<std::unique_ptr<Declaration>> multipleValues;
    std::unique_ptr<TypeNode> type;
    bool isConstant = false;

    Declaration() = default;

    inline std::string string()  override {return name->string();}
    std::string testString() override {return "Declaration: " + name->string();}
};

struct Assignment : public Node {
    std::unique_ptr<Node> variable;
    std::unique_ptr<Node> value;

    Assignment() = default;

    inline std::string string()  override {return variable->string() + " = " + value->string();}
    std::string testString() override {return "Assignment(" + variable->string() + " = " + value->string() + ")";}
};

struct ReturnNode : public Node {
    std::unique_ptr<Node> value;

    ReturnNode() = default;

    inline std::string string() override {return value->string();}
    std::string testString() override;
};

struct RValue : public Node {
    std::unique_ptr<Node> value;

    RValue() = default;

    inline std::string string() override {return value->string();}
    std::string testString() override;
};

struct CodeBlock : public Node {
    std::vector<std::unique_ptr<Node>> nodes;

    CodeBlock() = default;

    inline std::string string() override {return "CODE_BLOCK";}
    std::string testString() override;
};

struct IfElseNode : public Node {
    std::unique_ptr<Node> condition;
    std::unique_ptr<CodeBlock> consequence;
    std::unique_ptr<CodeBlock> alternative;

    IfElseNode() = default;

    inline std::string string() override {return "IF_STATEMENT";}
    std::string testString() override;
};

struct Prefix : public Node {
    std::string Operator;
    std::unique_ptr<Node> right;

    explicit Prefix(std::string Operator) : Operator(std::move(Operator)) {};

    inline std::string string() override;
    std::string testString() override;
};

struct Infix : public Node {
    std::string Operator;
    std::unique_ptr<Node> right;
    std::unique_ptr<Node> left;

    Infix(std::string Operator, std::unique_ptr<Node> left) : Operator(std::move(Operator)), left(std::move(left)) {};

    std::string string() override;
    std::string testString() override;
};

struct Function : public Node {
    std::vector<std::unique_ptr<Identifier>> parameters;
    std::unique_ptr<CodeBlock> body;
    std::string funcName;
    std::unique_ptr<TypeNode> type;

    Function() = default;

    inline std::string string() override {return "FUNCTION";}
    std::string testString() override;
};

struct FunctionCall : public Node {
    std::string funcName;
    std::vector<std::unique_ptr<Node>> args;

    explicit FunctionCall(std::string funcName) : funcName(std::move(funcName)) {};

    std::string string() override;
    std::string testString() override;
};

struct Index : public Node {
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> index;

    explicit Index(std::unique_ptr<Node> left) : left(std::move(left)) {};

    inline std::string string() override {return left->string() + "[" + index->string() + "]";}
    std::string testString() override;
};

struct PrintNode : public Node {
    std::vector<std::unique_ptr<Node>> values{};

    PrintNode() = default;

    inline std::string string() override {return "console.log()";}
    inline std::string testString() {return "OUTPUTNODE ()";};

};

#endif //GO_TO_TS_SIMPLE_COMPILER_AST_H
