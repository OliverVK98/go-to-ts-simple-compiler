//
// Created by oliver on 3/31/24.
//

#ifndef GO_TO_TS_SIMPLE_COMPILER_COMPILER_H
#define GO_TO_TS_SIMPLE_COMPILER_COMPILER_H

#include <fstream>
#include "../ast/ast.h"
#include "varTable.h"

extern std::unordered_map<TokenType , std::string> tokenTypeToStringTypeMap;
std::string getTsType(const TokenType& type, const std::unique_ptr<TypeNode>& nodeType);

class Compiler {
public:
    std::ofstream outputStream;
    VarTable varTable;
    std::vector<std::unique_ptr<VarTable>> scopeStack;

    Compiler(const std::string& outputFile) : outputStream(outputFile, std::ios::app) {
        if (!outputStream.is_open()) {
            throw std::runtime_error("Failed to open output file: " + outputFile);
        }
    }
    ~Compiler() {
        if (outputStream.is_open()) {
            outputStream.close();
        }
    }


    void enterScope() {
        auto newScope = std::make_unique<VarTable>();
        if (!scopeStack.empty()) {
            // The new scope's outer is the current top of the stack.
            newScope->outer = std::move(scopeStack.back());
        }
        scopeStack.push_back(std::move(newScope));
    }

    void exitScope() {
        if (!scopeStack.empty()) {
            // Simply remove the current scope from the stack.
            scopeStack.pop_back();
        }
    }

    VarTable* currentScope() {
        if (scopeStack.empty()) return nullptr;
        return scopeStack.back().get();
    }



    void compile(const std::unique_ptr<Node>& node);

    void emitVar(const Declaration* node);
    void emitConst(const Declaration* node);
    void emitFunc(const Function* node);
    void emitReturn(const ReturnNode* node);
    void emitFunctionCall(const FunctionCall* node);
    void parseInfix(Infix *node, const Declaration* decl);
};

#endif //GO_TO_TS_SIMPLE_COMPILER_COMPILER_H
