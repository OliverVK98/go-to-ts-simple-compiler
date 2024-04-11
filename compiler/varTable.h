//
// Created by oliver on 4/10/24.
//

#ifndef GO_TO_TS_SIMPLE_COMPILER_VARTABLE_H
#define GO_TO_TS_SIMPLE_COMPILER_VARTABLE_H

#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include "../token/token.h"

using VarScope = std::string;

const VarScope GLOBAL_SCOPE = "GLOBAL";
const VarScope LOCAL_SCOPE = "LOCAL";

struct Variable {
    std::string name;
    VarScope scope;
    TokenType type;

    Variable(std::string name, TokenType type, const VarScope &scope) : name(name), type(type), scope(scope) {};
};

class VarTable {
public:
    std::unique_ptr<VarTable> outer;
    std::unordered_map<std::string, std::unique_ptr<Variable>> varMap{};
    std::vector<Variable> freeSymbols;

    VarTable() {};

    void define(std::string name, TokenType type);
    std::unique_ptr<Variable> resolve(const std::string& name);
};

#endif //GO_TO_TS_SIMPLE_COMPILER_VARTABLE_H
