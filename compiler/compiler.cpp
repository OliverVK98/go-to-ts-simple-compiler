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

std::string getTsType(const TokenType& type, const std::unique_ptr<TypeNode>& nodeType) {
    if (type != ARRAY_TYPE) {
        return tokenTypeToStringTypeMap[type];
    } else {
        if (auto arrType = dynamic_cast<ArrayType*>(nodeType.get())) {
            return tokenTypeToStringTypeMap[arrType->getSubType()] + "[]";
        } else {
            throw std::runtime_error("Unknown type in getTsType");
        }
    }
}

void Compiler::compile(const std::unique_ptr<Node>& node) {
    logger console;
//    console.log(node->testString());
    if (auto program = dynamic_cast<Program*>(node.get())) {
        for (const auto& statement : program->nodes) {
            compile(statement);
        }
    } else if (auto rvalue = dynamic_cast<RValue*>(node.get())) {
        if (auto declStmt = dynamic_cast<Declaration*>(rvalue->value.get())) {
            emitVar(declStmt);
        } else if (auto funcCall = dynamic_cast<FunctionCall*>(rvalue->value.get())) {
            emitFunctionCall(funcCall);
        } else {
            throw std::runtime_error("Unhandled RValue type in compilation.");
        }
    } else if (auto declStmt = dynamic_cast<Declaration*>(node.get())) {
//        console.log(declStmt->value->string());

        if (auto infix = dynamic_cast<Infix*>(declStmt->value.get())) {
            parseInfix(infix, declStmt);
            return;
        }

        if (declStmt->isConstant) {
            emitConst(declStmt);
        } else {
            emitVar(declStmt);
        }
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

void Compiler::emitFunc(const Function *node) {
    if (!node) return;

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

    outputStream << "function " << node->funcName << "(" << paramString << ")";

    if (node->type) {
        outputStream << ": " << getTsType(node->type->getType(), node->type) << " ";
    } else {
        outputStream << ": void ";
    }

    outputStream << "{" << "\n";

    for (const auto &stmt : node->body->nodes) {
        outputStream << "\t";
        compile(stmt);
    }

    outputStream << "}" << "\n";
    exitScope();

}

void Compiler::emitReturn(const ReturnNode *node) {
    if (!node) return;
    if (node->value) {
        outputStream << "return " << node->value->string() << ";" << "\n";
    } else {
        outputStream << "return;" << "\n";
    }
}

void Compiler::emitFunctionCall(const FunctionCall *node) {
    if (!node) return;
    outputStream << node->funcName << "(";
    for (int i=0; i < node->args.size(); i++) {
        if (i < node->args.size()-1) {
            outputStream << node->args[i]->string() << ",";
        } else {
            outputStream << node->args[i]->string();
        }
    }
    outputStream << ")";
}

void Compiler::parseInfix(Infix *node, const Declaration* decl) {
    logger console;
    if (node->type) {
        outputStream << "let " << decl->name->string() << ": " << getTsType(node->type->getType(), node->type) << " = " << node->string() << ";\n";
    } else {
        std::string type;
        std::string tsType;
        if (auto ident = dynamic_cast<Identifier*>(node->left.get())) {
            auto scope = currentScope();
            type = scope->resolve(ident->name)->type;
            tsType = tokenTypeToStringTypeMap[type];
            console.log(tsType);
        }
        outputStream << "let " << decl->name->string() << ": " << tsType << " = " << node->string() << ";\n";
    }

}
