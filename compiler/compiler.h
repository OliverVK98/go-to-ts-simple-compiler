//
// Created by oliver on 3/31/24.
//

#ifndef GO_TO_TS_SIMPLE_COMPILER_COMPILER_H
#define GO_TO_TS_SIMPLE_COMPILER_COMPILER_H

#include <fstream>
#include "../ast/ast.h"

extern std::unordered_map<TokenType , std::string> tokenTypeToStringTypeMap;

class Compiler {
public:
    std::ofstream outputStream;

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

    void compile(const std::unique_ptr<Node>& node);

    void emitVar(const VarNode* node);
    void emitConst(const ConstNode* node);
};

#endif //GO_TO_TS_SIMPLE_COMPILER_COMPILER_H
