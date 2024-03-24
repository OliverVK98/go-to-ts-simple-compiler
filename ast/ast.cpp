//
// Created by oliver on 3/18/24.
//

#include "ast.h"

std::string Program::tokenLiteral() {
    if (!Statements.empty()) {
        return Statements[0]->tokenLiteral();
    } else {
        return "";
    }
}

std::string Program::string() {
    std::stringstream ss;

    for (Statement* stmt : Statements) {
        ss << stmt->string();
    }

    return ss.str();
}

std::string ConstStatement::string() {
    std::stringstream ss;

    ss << token.Literal << " ";
    if (name) ss << name->string() << " = ";
    if (value) ss << value->string();
    ss << ";";

    return ss.str();
}

std::string ReturnStatement::string() {
    std::stringstream ss;

    ss << token.Literal << " ";
    if(value) ss<<value->string();

    ss << ";";

    return ss.str();
}

std::string PrefixExpression::string() {
    std::stringstream ss;

    ss << "(" << Operator << right->string() << ")";

    return ss.str();
}

std::string InfixExpression::string() {
    std::stringstream ss;

    ss << "(" << left->string() << " " + Operator + " " << right->string() << ")";

    return ss.str();
}

std::string ExpressionStatement::string() {
    if (value) { return value->string(); }
    return "";
}
