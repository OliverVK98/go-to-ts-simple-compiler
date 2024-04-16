//
// Created by oliver on 3/31/24.
//

#include "./compiler.h"
#include "../logger/logger.h"


std::unordered_map<TokenType , std::string> tokenTypeToStringTypeMap = {
        {INT_TYPE, "number"},
        {STRING_TYPE, "string"},
        {BOOL_TYPE, "boolean"},
        {ARRAY_TYPE+INT_TYPE, "number"},
        {BOOL_TYPE, "boolean"},
        {BOOL_TYPE, "boolean"},
        {NOTYPE_TYPE, "void"},
};

std::string getTsType(const TokenType& type, const std::unique_ptr<TypeNode>& nodeType) {
    if (type.find(ARRAY_TYPE) == 0) {
        if (auto arrType = dynamic_cast<ArrayType*>(nodeType.get())) {
            return tokenTypeToStringTypeMap[arrType->getSubType()] + "[]";
        }
    } else {
        return tokenTypeToStringTypeMap[type];
    }
}

std::string getTsSubType(const std::string& type) {
    auto pos = type.find(ARRAY_TYPE);

    if (pos == 0) {
        std::string subType = type.substr(ARRAY_TYPE.length());
        return  tokenTypeToStringTypeMap[subType];
    } else {
        throw std::runtime_error("Type has no subtypes.");
    }
}

void Compiler::enterScope() {
    auto newScope = std::make_unique<VarTable>();
    newScope->outer = std::move(varTable);
    varTable = std::move(newScope);
    indentLevel++;
}

void Compiler::exitScope() {
    varTable = std::move(varTable->outer);
    indentLevel--;
}

VarTable* Compiler::currentScope() {
    return varTable.get();
}

std::string Compiler::getIndent() {
    std::string ident;

    for (int i=0; i < indentLevel; i ++) {
        ident += "\t";
    }

    return ident;
}

void Compiler::compile(const std::unique_ptr<Node>& node) {
    if (auto program = dynamic_cast<Program*>(node.get())) {
        for (const auto& statement : program->nodes) {
            compile(statement);
        }
    } else if (auto rvalue = dynamic_cast<RValue*>(node.get())) {
        if (auto declStmt = dynamic_cast<Declaration*>(rvalue->value.get())) {
            emitDeclaration(declStmt, false);
        } else if (auto funcCall = dynamic_cast<FunctionCall*>(rvalue->value.get())) {
            emitFunctionCall(funcCall);
        } else {
            throw std::runtime_error("Unhandled RValue type in compilation.");
        }
    } else if (auto declStmt = dynamic_cast<Declaration*>(node.get())) {
        if (auto infix = dynamic_cast<Infix*>(declStmt->value.get())) {
            emitInfix(infix, declStmt, true);
            return ;
        }
        return declStmt->isConstant ? emitDeclaration(declStmt, true) : emitDeclaration(declStmt, false);
    } else if (auto func = dynamic_cast<Function*>(node.get())) {
        emitFunc(func);
    } else if  (auto returnStmt = dynamic_cast<ReturnNode*>(node.get())) {
        emitReturn(returnStmt);
    } else if  (auto integer = dynamic_cast<Integer*>(node.get())) {
        outputStream << integer->string();
    } else {
        throw std::runtime_error("Unhandled node subType in compilation.");
    }
}

void Compiler::emitDeclaration(Declaration *node, bool isConstant) {
    if (!node) return;
    auto indent = getIndent();
    auto scope = currentScope();
    auto declKeyword = isConstant ? "const " : "let ";
    outputStream << indent;

    if (auto funcCall = dynamic_cast<FunctionCall*>(node->value.get())) {
        if (isConstant) throw std::runtime_error("Const value can't be a result of function call.");
        auto type = scope->resolve(funcCall->funcName)->type;
        auto tsType = tokenTypeToStringTypeMap[type];
        outputStream << "let " << node->name->string() << ": " << tsType << " = ";
        emitFunctionCall(funcCall);
        outputStream << ";\n";
        return;
    }

    if (auto index = dynamic_cast<Index*>(node->value.get())) {
        if (isConstant) throw std::runtime_error("Const value can't be a result of function call.");
        std::string type;
        std::string tsType;

        if (auto ident = dynamic_cast<Identifier*>(index->left.get())) {
            type = ident->type ? ident->type->getType() : scope->resolve(ident->name)->type;
            tsType = getTsSubType(type);
        } else if (index->left->type->getType() != ARRAY_TYPE) throw std::runtime_error("Index can be only used with arrays");
        outputStream << "let " << node->name->string() << ": " << tsType << " = " << index->string() << ";\n";
        return;
    }

    if (node->holdsMultipleValues) {
        for (const auto & value : node->multipleValues) {
            auto declStmt = dynamic_cast<Declaration*>(value.get());
            emitDeclaration(declStmt, isConstant);
        }
        return;
    }

    if (node->type->getType() == INT_TYPE || node->type->getType() == BOOL_TYPE || node->type->getType() == STRING_TYPE) {
        if (node->value->holdsValue) {
            outputStream << declKeyword << node->name->string() << ": " << tokenTypeToStringTypeMap[node->type->getType()] << " = " << node->value->string() << ";\n";
        } else {
            outputStream << declKeyword << node->name->string() << ": " << tokenTypeToStringTypeMap[node->type->getType()] << ";\n";
        }
    } else if (node->type->getType() == ARRAY_TYPE && !isConstant) {
        std::string arrType = node->type->getSubType();
        scope->define(node->name->string(), node->type->getType() + node->type->getSubType());
        outputStream << "let " << node->name->string() <<": " << tokenTypeToStringTypeMap[arrType] << "[]";

        if (node->value->holdsValue) {
            outputStream << " = " << node->value->string();
            outputStream << ";\n";
        } else {
            outputStream << ";\n";
        }
    } else if (node->type->getType() == NOTYPE_TYPE && !isConstant) {
        auto type = scope->resolve(node->value->string())->type;
        auto tsType = tokenTypeToStringTypeMap[type];
        outputStream << "let " << node->name->string() << ": " << tsType << " = " << node->value->string() << ";\n";
    }
}

void Compiler::emitFunc(Function *node) {
    if (!node) return;
    auto indent = getIndent();
    enterScope();
    std::string paramString;

    auto scope = currentScope();

    for (int i = 0; i < node->parameters.size(); i++) {

        scope->define(node->parameters[i]->string(), node->parameters[i]->type->getType());

        if (i < node->parameters.size()-1) {
            paramString += node->parameters[i]->string() + ": " + getTsType(node->parameters[i]->type->getType(), node->parameters[i]->type) + ", ";
        } else {
            paramString += node->parameters[i]->string() + ": " + getTsType(node->parameters[i]->type->getType(), node->parameters[i]->type);
        }
    }

    outputStream << indent << "function " << node->funcName << "(" << paramString << ")";

    if (node->type) {
        scope->outer->define(node->funcName, node->type->getType());
        outputStream << ": " << getTsType(node->type->getType(), node->type) << " ";
    } else {
        scope->outer->define(node->funcName, NOTYPE_TYPE);
        outputStream << ": void ";
    }

    outputStream << "{" << "\n";

    for (const auto &stmt : node->body->nodes) {
        compile(stmt);
    }

    outputStream << indent << "}" << "\n";
    exitScope();
}

void Compiler::emitReturn(ReturnNode *node) {
    if (!node) return;
    if (node->value) {
        outputStream << getIndent() << "return " << node->value->string() << ";" << "\n";
    } else {
        outputStream << getIndent() << "return;" << "\n";
    }
}

void Compiler::emitFunctionCall(FunctionCall *node) {
    if (!node) return;
    outputStream << node->string();
}

std::pair<std::string, std::string> Compiler::emitInfix(Infix *node, Declaration *decl, bool isRootCall) {
    if (!node) return {"", ""};

    auto scope = currentScope();
    std::string exprType;
    std::string expr;

    if (auto ident = dynamic_cast<Identifier*>(node->left.get())) {
        exprType = scope->resolve(ident->name)->type;
        expr = ident->string();
    } else if (auto infix = dynamic_cast<Infix*>(node->left.get())) {
        auto result = emitInfix(infix, decl, false);
        expr = result.first;
        exprType = result.second;
    } else if (auto funcCall = dynamic_cast<FunctionCall*>(node->left.get())) {
        if (decl->isConstant) throw std::runtime_error("Const value can't be a result of function call.");
        exprType = scope->resolve(funcCall->funcName)->type;
        expr = funcCall->string();
    } else if (auto index = dynamic_cast<Index*>(node->left.get())) {
        if (decl->isConstant) throw std::runtime_error("Const value can't be a result of index subscription");
        exprType = index->type ? index->type->getType() : scope->resolve(index->left->string())->type;
        expr = index->string();
    } else if (auto valNode = dynamic_cast<ValueNode*>(node->left.get())) {
        expr = valNode->string();
        exprType = valNode->type->getType();
    }

    expr += " " + node->Operator + " " + node->right->string();

    if (isRootCall) {
        outputStream << getIndent() << "let " << decl->name->string() << ": " << tokenTypeToStringTypeMap[exprType] << " = " << expr << ";\n";
    }

    return {expr, exprType};
}



