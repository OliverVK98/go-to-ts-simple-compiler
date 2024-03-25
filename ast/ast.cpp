//
// Created by oliver on 3/18/24.
//

#include "ast.h"

std::string Program::string() {
    std::stringstream ss;

    for (auto& stmt : statements) {
        ss << stmt->string() << "\n";
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

std::string BlockStatement::string() {
    std::ostringstream out;

    for (const auto& s : statements) {
        if (s) {
            out << s->string();
        }
    }

    return out.str();
}

std::string IfExpressionStatement::string() {
    std::ostringstream out;

    out << "if ";
    if (condition) out << condition->string();
    out << " ";
    if (consequence) out << consequence->string();

    if (alternative) {
        out << " else ";
        out << alternative->string();
    }

    return out.str();
}

std::string Function::string() {
    std::ostringstream out;

    out << token.Literal;
    if (!funcName.empty()) {
        out << "<" << funcName << ">";
    }
    out << "(";

    for (size_t i = 0; i < parameters.size(); ++i) {
        out << parameters[i].string();
        if (i < parameters.size() - 1) {
            out << ", ";
        }
    }

    out << ") ";
    if (body) {
        out << body->string();
    }

    return out.str();
}
