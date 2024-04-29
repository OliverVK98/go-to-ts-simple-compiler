//
// Created by oliver on 4/10/24.
//

#include "varTable.h"

void VarTable::define(std::string name, TokenType type) {
    VarScope scope = outer == nullptr ? GLOBAL_SCOPE : LOCAL_SCOPE;
    varMap.emplace(name, std::make_unique<Variable>(name, type, scope));
}

std::unique_ptr<Variable> VarTable::resolve(const std::string& name) {
    auto it = varMap.find(name);
    if (it != varMap.end()) {
        return std::make_unique<Variable>(*it->second);
    } else if (outer != nullptr) {
        return outer->resolve(name);
    }
    return nullptr;
}
