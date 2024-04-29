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
std::string getTsSubType(const std::string& type);

class Compiler {
public:
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
    void compile(const std::unique_ptr<Node>& node);
private:
    int indentLevel = -1;
    std::unique_ptr<VarTable> varTable;
    std::ofstream outputStream;
    std::vector<std::unique_ptr<VarTable>> scopeStack{};

    // Scope management
    void enterScope();
    void exitScope();
    VarTable* currentScope();

    std::string getIndent();

    void emitDeclaration(Declaration* node, bool isConstant);
    void emitFunc(Function* node);
    void emitReturn(ReturnNode* node);
    inline void emitFunctionCall(FunctionCall* node) {if (!node) return; outputStream << node->string();}
    std::pair<std::string, std::string> emitInfix(Infix *node, Declaration *decl, bool isRootCall);
    void emitIfElse(IfElseNode *node);
    inline void emitAssignment(Assignment *node) {if (!node) return; outputStream << getIndent() << node->string() << ";\n";}
    void emitPrintNode(PrintNode *node);
};

#endif //GO_TO_TS_SIMPLE_COMPILER_COMPILER_H
