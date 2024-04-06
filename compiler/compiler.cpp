//
// Created by oliver on 3/31/24.
//

#include "./compiler.h"
#include "../logger/logger.h"


std::unordered_map<TokenType , std::string> tokenTypeToStringTypeMap = {
        {INT_TYPE, "number"},
        {STRING_TYPE, "string"},
        {BOOL_TYPE, "boolean"},
};

void Compiler::compile(const std::unique_ptr<Node>& node) {

    if (auto program = dynamic_cast<Program*>(node.get())) {
        for (const auto& statement : program->nodes) {
            compile(statement);
        }
    } else if (auto rvalue = dynamic_cast<RValue*>(node.get())) {
        if (auto declStmt = dynamic_cast<Declaration*>(rvalue->value.get())) {
            emitVar(declStmt);
        }
    } else if (auto declStmt = dynamic_cast<Declaration*>(node.get())) {
        if (declStmt->isConstant) {
            emitConst(declStmt);
        } else {
            emitVar(declStmt);
        }
    } else {
        throw std::runtime_error("Unhandled node subType in compilation.");
    }
}

void Compiler::emitVar(const Declaration* node) {
    if (!node) return;

    if (node->holdsMultipleValues) {
        for (const auto & value : node->multipleValues) {
            auto varStmt = dynamic_cast<Declaration*>(value.get());
            emitVar(varStmt);
        }
        return;
    }

    if (node->type->getType() == INT_TYPE || node->type->getType() == BOOL_TYPE || node->type->getType() == STRING_TYPE) {
        if (node->value->holdsValue) {
            outputStream << "let " << node->name->string() << ": " << tokenTypeToStringTypeMap[node->type->getType()] << " = " << node->value->string() << ";\n";
        } else {
            outputStream << "let " << node->name->string() << ": " << tokenTypeToStringTypeMap[node->type->getType()] << ";\n";
        }
    } else if (node->type->getType() == ARRAY_TYPE) {
        std::string arrType = node->type->getSubType();
        outputStream << "let " << node->name->string() <<": " << tokenTypeToStringTypeMap[arrType] << "[]";

        if (node->value->holdsValue) {
            outputStream << " = " << node->value->string();
            outputStream << ";\n";
        } else {
            outputStream << ";\n";
        }
    }
}

void Compiler::emitConst(const Declaration *node) {
    if (!node) return;

    if (node->holdsMultipleValues) {
        for (const auto & value : node->multipleValues) {
            auto constStmt = dynamic_cast<Declaration*>(value.get());
                emitConst(constStmt);
        }
        return;
    }

    if (node->type->getType() == INT_TYPE || node->type->getType() == BOOL_TYPE || node->type->getType() == STRING_TYPE) {
        if (node->value->holdsValue) {
            outputStream << "const " << node->name->string() << ": " << tokenTypeToStringTypeMap[node->type->getType()] << " = " << node->value->string() << ";\n";
        } else {
            outputStream << "const " << node->name->string() << ": " << tokenTypeToStringTypeMap[node->type->getType()] << ";\n";
        }
    }
}
