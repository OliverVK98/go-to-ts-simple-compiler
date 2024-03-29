//
// Created by oliver on 3/18/24.
//

#include "ast.h"
#include "../logger/logger.h"

std::string Program::string() {
    std::stringstream ss;

    for (auto& stmt : nodes) {
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

std::string Prefix::string() {
    std::stringstream ss;

    ss << "(" << Operator << right->string() << ")";

    return ss.str();
}

std::string Infix::string() {
    std::stringstream ss;

    ss << "(" << left->string() << " " + Operator + " " << right->string() << ")";

    return ss.str();
}

std::string ExpressionStatement::string() {
    if (value) { return value->string(); }
    return "";
}

std::string CodeBlock::string() {
    std::ostringstream out;

    for (const auto& s : nodes) {
        if (s) {
            out << s->string();
        }
    }

    return out.str();
}

std::string IfElseNode::string() {
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
        out << parameters[i]->string();
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

std::string FunctionCall::string() {
    std::ostringstream out;

    std::vector<std::string> arguments;
    arguments.reserve(args.size());
    for (const auto& arg : args) {
        arguments.push_back(arg->string());
    }

    std::string argumentsString;
    for (auto i=0; i<arguments.size(); i++) {
        if (i != arguments.size()-1) {
        argumentsString += arguments[i] + ", ";
        } else {
            argumentsString += arguments[i] + ")";
        }
    }

    out << func->string() << "(" << argumentsString;

    return out.str();
}

std::string Array::string() {
    std::ostringstream out;

    std::vector<std::string> arguments;
    arguments.reserve(elements.size());
    for (const auto& arg : elements) {
        arguments.push_back(arg->string());
    }

    std::string elementsString;
    for (auto i=0; i<arguments.size(); i++) {
        if (i != arguments.size()-1) {
            elementsString += arguments[i] + ", ";
        } else {
            elementsString += arguments[i] + "]";
        }
    }

    out <<  "[" << elementsString;

    return out.str();
}

std::string Index::string() {
    std::ostringstream out;

    out << "(" << left->string() << "[" << index->string() << "])";

    return out.str();
}