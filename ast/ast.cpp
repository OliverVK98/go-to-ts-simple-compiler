//
// Created by oliver on 3/18/24.
//

#include "ast.h"
#include "../logger/logger.h"

std::string Program::testString() {
    std::stringstream ss;

    for (auto& stmt : nodes) {
        ss << stmt->testString();
    }

    return ss.str();
}

std::string Program::string() {
    return std::string();
}

std::string ConstNode::testString() {
    std::stringstream ss;

    ss << "ConstNode(" << name->testString() << " = " << value->testString() << ")";

    return ss.str();
}

std::string VarNode::testString() {
    std::stringstream ss;

    ss << "VarNode(" << name->testString() << " = " << value->testString() << ")";

    return ss.str();
}

std::string ReturnNode::testString() {
    std::stringstream ss;

    ss << "ReturnNode(";

    if (value) {
        ss << value->testString();
    } else {
        ss << "EMPTY";
    }

    ss << ")";

    return ss.str();
}

std::string Prefix::testString() {
    std::stringstream ss;

    ss << "(" << Operator << right->testString() << ")";

    return ss.str();
}

std::string Infix::testString() {
    std::stringstream ss;

    ss << "(" << left->testString() << " " + Operator + " " << right->testString() << ")";

    return ss.str();
}

std::string RValue::testString() {
    if (value) { return "RValue(" + value->testString() + ")"; }
    return "";
}

std::string CodeBlock::testString() {
    std::ostringstream out;

    for (const auto& s : nodes) {
        if (s) {
            out << s->testString();
        }
    }

    return out.str();
}

std::string IfElseNode::testString() {
    std::ostringstream out;

    out << "IfStatement(";
    if (condition) {
        out << "Condition(" << condition->testString() << ")";
    } else {
        out << "Condition()";
    }
    out << " ";
    if (consequence) {
        out << "Consequence(" << consequence->testString() << ")";
    } else {
        out << "Consequence()";
    }

    if (alternative) {
        out << " Alternative(";
        out << alternative->testString() << ")";
    } else {
        out << " Alternative()";
    }
    out << ")";

    return out.str();
}

std::string Function::testString() {
    std::ostringstream out;

    out << "Function(";
    if (!funcName.empty()) {
        out << "Name(" << funcName << ")";
    } else {
        out << "Name()";
    }
    out << " Params(";

    for (size_t i = 0; i < parameters.size(); ++i) {
        out << parameters[i]->testString();
        if (i < parameters.size() - 1) {
            out << ", ";
        }
    }

    out << ") Body(";
    if (body) {
        out << body->testString();
    }

    out << "))";

    return out.str();
}

std::string FunctionCall::testString() {
    std::ostringstream out;

    std::vector<std::string> arguments;
    arguments.reserve(args.size());
    for (const auto& arg : args) {
        arguments.push_back(arg->testString());
    }

    std::string argumentsString;
    for (auto i=0; i<arguments.size(); i++) {
        if (i != arguments.size()-1) {
        argumentsString += arguments[i] + ", ";
        } else {
            argumentsString += arguments[i] + ")";
        }
    }

    out << "FunctionCall(" + func->testString() << "(" << argumentsString + ")";

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
            elementsString += arguments[i];
        }
    }

    out <<  "[" << elementsString << "]";

    return out.str();
}

std::string Array::testString() {
    std::ostringstream out;

    std::vector<std::string> arguments;
    arguments.reserve(elements.size());
    for (const auto& arg : elements) {
        arguments.push_back(arg->testString());
    }

    std::string elementsString;
    for (auto i=0; i<arguments.size(); i++) {
        if (i != arguments.size()-1) {
            elementsString += arguments[i] + ", ";
        } else {
            elementsString += arguments[i] + "]";
        }
    }

    out <<  "Array([" << elementsString << ")";

    return out.str();
}

std::string Index::testString() {
    std::ostringstream out;

    out << "IndexExpression(" << "Left:(" + left->testString() + ")" << " Right: [" << index->testString() << "])";

    return out.str();
}

std::string boolToString(bool boolV) {
    if (boolV) { return "true";} else { return "false"; }
}


