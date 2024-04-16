//
// Created by oliver on 3/31/24.
//

#ifndef GO_TO_TS_SIMPLE_COMPILER_COMPILER_H
#define GO_TO_TS_SIMPLE_COMPILER_COMPILER_H

#include <fstream>
#include "../ast/ast.h"
#include "varTable.h"
#include "../logger/logger.h"

extern std::unordered_map<TokenType , std::string> tokenTypeToStringTypeMap;
std::string getTsType(const TokenType& type, const std::unique_ptr<TypeNode>& nodeType);
std::string getTsSubType(const std::string& type);

class Compiler {
private:
    int indentLevel = -1;
public:
    std::unique_ptr<VarTable> varTable;
    std::ofstream outputStream;
    std::vector<std::unique_ptr<VarTable>> scopeStack{};

    Compiler(const std::string& outputFile) : outputStream(outputFile, std::ios::app) {
        if (!outputStream.is_open()) {
            throw std::runtime_error("Failed to open output file: " + outputFile);
        }
        enterScope();
    }
    ~Compiler() {
        if (outputStream.is_open()) {
            outputStream.close();
        }
    }

    // Scope management
    void enterScope();
    void exitScope();
    VarTable* currentScope();

    std::string getIndent();

    void compile(const std::unique_ptr<Node>& node);

    void emitDeclaration(Declaration* node, bool isConstant);
    void emitFunc(Function* node);
    void emitReturn(ReturnNode* node);
    void emitFunctionCall(FunctionCall* node);
    std::pair<std::string, std::string> emitInfix(Infix *node, Declaration *decl, bool isRootCall);
};

#endif //GO_TO_TS_SIMPLE_COMPILER_COMPILER_H
