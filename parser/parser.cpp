//
// Created by oliver on 3/21/24.
//

#include "parser.h"
#include "../logger/logger.h"

#include <utility>

// TODO: add factory?

bool startsWithType(const std::string& str) {
    const std::string prefix = "type_";
    if (str.length() < prefix.length()) {
        return false;
    }
    return str.substr(0, prefix.length()) == prefix;
}

std::unordered_map<TokenType, int> precedences = {
        {EQ,       EQUALS},
        {NOT_EQ,   EQUALS},
        {LESS_THAN,       LESSORGREATER},
        {GREATER_THAN,       LESSORGREATER},
        {PLUS,     SUM},
        {MINUS,    SUM},
        {SLASH,    PRODUCT},
        {ASTERISK, PRODUCT},
        {LPAREN,   CALL},
        {LBRACKET, INDEX},
};

bool Parser::checkNextTokenAndAdvance(const TokenType& t) {
    if (nextTokenIs(t)) {
        getNextToken();
        return true;
    };
    return false;
}

// PARSING VARIABLES AND THEIR TYPES
std::unique_ptr<Node> Parser::parseDeclarationNode(DeclarationType declType) {
    auto node = std::make_unique<Declaration>(currentToken);
    if (declType == CONST_DECL) node->isConstant = true;
    if (declType == SHORT_DECL) {
        return parseShortDeclarationNode(node);
    } else if (checkNextTokenAndAdvance(IDENTIFIER)) {
        // Parsing non-grouped declarations
        return parseExplicitDeclarationNode(node);
    } else if (checkNextTokenAndAdvance(LPAREN)) {
        return parseGroupedDeclarationNode(node);
    } else {
        throw std::runtime_error("Unknown variable declaration");
    }
}

std::unique_ptr<Declaration> Parser::parseShortDeclarationNode(std::unique_ptr<Declaration> &node) {
    node->name = std::make_unique<Identifier>(currentToken, currentToken.Literal);
    getNextToken(2);

    if (currentTokenIs(LBRACKET)) {
        auto arr = parseArray();
        node->type = std::move(arr->type);
        node->value = std::move(arr);
    } else {
        node->value = parseRValue(LOWEST);
    }

    if (!node->type) {
        parseImplicitVariableType(node);
    }

    return std::move(node);
}

std::unique_ptr<Declaration> Parser::parseGroupedDeclarationNode(std::unique_ptr<Declaration> &node) {
    getNextToken();
    node->holdsMultipleValues = true;

    while (currentTokenIs(IDENTIFIER)) {
        auto newNode = std::make_unique<Declaration>();
        newNode->name = parseIdentifier();
        getNextToken();

        if (currentTokenIs(LBRACKET)) {
            newNode->type = parseType();
            if (checkNextTokenAndAdvance(ASSIGN)) {
                getNextToken();
                newNode->value = std::move(parseArray());
                node->multipleValues.push_back(std::move(newNode));
                getNextToken();
            } else {
                newNode->value = std::make_unique<Array>();
                newNode->value->holdsValue = false;
                node->multipleValues.push_back(std::move(newNode));
                getNextToken();
            }

        } else {
            if (tokenTypeIsTypeNode(currentToken.Type)) {
                newNode->type = parseType();
                getNextToken();
            }
            if(currentTokenIs(ASSIGN)) {
                getNextToken();
                newNode->value = parseRValue(LOWEST);
                if (!newNode->type) {
                    parseImplicitVariableType(newNode);
                }
                getNextToken();

            } else {
                newNode->value = std::make_unique<Integer>();
                newNode->value->holdsValue = false;
            }

        }

        node->multipleValues.push_back(std::move(newNode));
    }
    return std::move(node);
}

std::unique_ptr<Declaration> Parser::parseExplicitDeclarationNode(std::unique_ptr<Declaration> &node) {
    node->name = std::make_unique<Identifier>(currentToken, currentToken.Literal);

    if(nextTokenIs(LBRACKET)) {
        if (node->isConstant) throw std::runtime_error("Constant array!");
        getNextToken();
        node->type = parseType();
    } else if (startsWithType(nextToken.Type)) {
        getNextToken();
        node->type = parseType();
    }

    if (!nextTokenIs(ASSIGN)) {
        if (node->type) {
            node->value = std::make_unique<Integer>();
            node->value->holdsValue = false;
            return std::move(node);
        } else {
            throw std::runtime_error("Unknown declaration");
        }
    }

    getNextToken(2);

    if (currentTokenIs(LBRACKET)) {
        if (node->isConstant) throw std::runtime_error("Constant array!");
        auto arr = parseArray();
        node->type = std::move(arr->type);
        node->value = std::move(arr);
    } else {
        node->value = parseRValue(LOWEST);
    }

    if (!node->type) {
        parseImplicitVariableType(node);
    }

    return std::move(node);
}


void Parser::parseImplicitVariableType(std::unique_ptr<Declaration> &node) {
    if (dynamic_cast<Integer*>(node->value.get())) {
        node->type = std::make_unique<IntegerType>();
    } else if (dynamic_cast<String*>(node->value.get())) {
        node->type = std::make_unique<StringType>();
    } else if (dynamic_cast<Boolean*>(node->value.get())) {
        node->type = std::make_unique<BoolType>();
    } else if (auto arr = dynamic_cast<Array*>(node->value.get())) {
        auto arrType = std::make_unique<ArrayType>();
        if (dynamic_cast<Integer*>(arr->elements[0].get())) {
            arrType->subType = std::make_unique<IntegerType>();
        } else if (dynamic_cast<String*>(arr->elements[0].get())) {
            arrType->subType = std::make_unique<StringType>();
        } else if (dynamic_cast<Boolean*>(arr->elements[0].get())) {
            arrType->subType = std::make_unique<BoolType>();
        }
        node->type = std::move(arrType);
    } else {
        node->type = std::make_unique<NoType>();
    }
}

std::unique_ptr<TypeNode> Parser::parseType() {
    if (currentToken.Literal == LBRACKET) {
        getNextToken(3);
        auto type = parseType();
        return std::make_unique<ArrayType>(std::move(type));
    } else if (currentToken.Type == STRING_TYPE) {
        return std::make_unique<StringType>();
    } else if (currentToken.Type == BOOL_TYPE) {
        return std::make_unique<BoolType>();
    } else if (currentToken.Type == INT_TYPE) {
        return std::make_unique<IntegerType>();
    } else {
        throw std::runtime_error("incorrect subType: " + currentToken.Literal);
    }
}


// PARSING FUNCTION
std::unique_ptr<Function> Parser::parseFunctionDeclaration() {
    auto func = std::make_unique<Function>(currentToken);

    if(!nextTokenIs(IDENTIFIER)) {
        throw std::runtime_error("Unhandled function declaration: function name is missing");
    }

    getNextToken();

    if (currentTokenIs(IDENTIFIER)) {
        func->funcName = parseIdentifier()->string();
        getNextToken();
    }

    func->parameters = parseFunctionParameters();

    if (startsWithType(nextToken.Type)) {
        getNextToken();
        func->type = parseType();
    } else if (nextTokenIs(LBRACKET)) {
        getNextToken(3);
        func->type = std::make_unique<ArrayType>(parseType());
    }

    if(!checkNextTokenAndAdvance(LBRACE)) {
        return nullptr;
    }

    func->body = parseBlockNode();

    return func;
}

std::unique_ptr<CodeBlock> Parser::parseBlockNode() {
    auto block = std::make_unique<CodeBlock>(currentToken);
    getNextToken();


    while (!currentTokenIs(RBRACE) && !currentTokenIs(END_OF_FILE)) {
        auto node = parseNode();
        if (node) {
            block->nodes.emplace_back(std::move(node));
        }
        getNextToken();
    }

    return block;
}

std::vector<std::unique_ptr<Identifier>> Parser::parseFunctionParameters() {
    auto params = std::vector<std::unique_ptr<Identifier>>{};
    std::vector<std::unique_ptr<Identifier>> untypedParamVector{};
    if (nextTokenIs(RPAREN)) {
        getNextToken();
        return params;
    }

    getNextToken();

    auto param = std::make_unique<Identifier>(currentToken, currentToken.Literal);
    if (startsWithType(nextToken.Type) || nextTokenIs(LBRACKET)) {
        getNextToken();
        if (currentTokenIs(LBRACKET)) {
            getNextToken(2);
            param->type = std::make_unique<ArrayType>(parseType());
        } else {
            param->type = parseType();
        }
        params.emplace_back(std::move(param));
    } else {
        untypedParamVector.push_back(std::move(param));
    }

    while (nextTokenIs(COMMA)) {
        getNextToken(2);
        auto newParam = std::make_unique<Identifier>(currentToken, currentToken.Literal);
        if (startsWithType(nextToken.Type) || nextTokenIs(LBRACKET)) {
            getNextToken();
            std::unique_ptr<TypeNode> type;
            if (currentTokenIs(LBRACKET)) {
                getNextToken(2);
                type = std::make_unique<ArrayType>(parseType());
            } else {
                type = parseType();
            }
            auto extraTypeCopy = type->clone();
            newParam->type = std::move(type);
            params.emplace_back(std::move(newParam));
            if (!untypedParamVector.empty()) {
                for (auto& untypedParam : untypedParamVector) {
                    untypedParam->type = std::move(extraTypeCopy->clone());
                    params.emplace_back(std::move(untypedParam));
                }
                untypedParamVector.clear();
            }
        } else {
            untypedParamVector.push_back(std::move(newParam));
        }
    }

    if (!checkNextTokenAndAdvance(RPAREN)) {
        return std::vector<std::unique_ptr<Identifier>>{};
    }

    return params;
}

std::unique_ptr<FunctionCall> Parser::parseFunctionCall(std::unique_ptr<Node> funcName) {
    auto funcCall = std::make_unique<FunctionCall>(currentToken, funcName->string());
    funcCall->args = std::move(parseNodeList(RPAREN));
    return funcCall;
}

std::unique_ptr<ReturnNode> Parser::parseReturnNode() {
    auto node = std::make_unique<ReturnNode>(currentToken);

    if (!nextTokenIs(RBRACE)) {
        getNextToken();
        node->value = parseRValue(LOWEST);
    }

    return node;
}


//

std::unique_ptr<Node> Parser::parseRValueNode() {
    auto node = std::make_unique<RValue>(currentToken);

    node->value = parseRValue(LOWEST);

    return node;
}

std::unique_ptr<Node> Parser::parseRValue(const int &precedence) {
    auto prefix = prefixParseFns[currentToken.Type];

    if (!prefix) {
        return nullptr;
    }

    auto leftExp = prefix();
    std::unique_ptr<TypeNode> type;

    if (dynamic_cast<Integer*>(leftExp.get())) {
        type =  std::make_unique<IntegerType>();
    } else if (dynamic_cast<String*>(leftExp.get())) {
        type =  std::make_unique<StringType>();
    } else if (dynamic_cast<Boolean*>(leftExp.get())) {
        type =  std::make_unique<BoolType>();
    }

    while (precedence < peekPrecedence()) {
        auto infix = infixParseFns[nextToken.Type];
        if (!infix) {
            return nullptr;
        }

        getNextToken();
        leftExp = std::unique_ptr<Node>(infix(std::move(leftExp)));
        leftExp->type = std::move(type);
    };

    return leftExp;
}

Precedence Parser::peekPrecedence() const {
    auto it = precedences.find(nextToken.Type);
    if (it != precedences.end()) {
        return static_cast<Precedence>(it->second);
    }
    return Precedence::LOWEST;
}

Precedence Parser::currentPrecedence() const {
    auto it = precedences.find(currentToken.Type);
    if (it != precedences.end()) {
        return static_cast<Precedence>(it->second);
    }
    return Precedence::LOWEST;
}


std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();
    program->nodes = std::move(std::vector<std::unique_ptr<Node>>{});

    while (!currentTokenIs(END_OF_FILE)) {
        auto node = parseNode();

        if (node) {
            program->nodes.emplace_back(std::move(node));
        }
        getNextToken();
    }

    return program;
}

std::unique_ptr<Integer> Parser::parseIntegerLiteral() {
    auto intLit = std::make_unique<Integer>(currentToken);
    intLit->value = std::stoi(currentToken.Literal);
    return intLit;
}

std::unique_ptr<Node> Parser::parseNode() {
    if (currentToken.Type == CONST) {
        return parseDeclarationNode(CONST_DECL);
    } else if (currentToken.Type == VAR) {
        return parseDeclarationNode(VAR_DECL);
    } else if (currentToken.Type == RETURN) {
        return parseReturnNode();
    } else if (currentToken.Type == IF) {
        return parseIfNode();
    } else if (currentToken.Type == FUNCTION) {
        return parseFunctionDeclaration();
    } else {
        return parseRValueNode();
    }
}

void Parser::registerPrefix(const TokenType& tokenType, prefixParseFn fn) {
    prefixParseFns[tokenType] = std::move(fn);
}

void Parser::registerInfix(const TokenType& tokenType, infixParseFn fn) {
    infixParseFns[tokenType] = std::move(fn);
}

std::unique_ptr<Prefix> Parser::parsePrefixNode() {
    auto node = std::make_unique<Prefix>(currentToken, currentToken.Literal);
    getNextToken();
    node->right = parseRValue(PREFIX);
    return node;
}

std::unique_ptr<Infix> Parser::parseInfixNode(std::unique_ptr<Node> left) {
    auto node = std::make_unique<Infix>(currentToken, currentToken.Literal, std::move(left));
    auto precedence = currentPrecedence();
    getNextToken();
    node->right = parseRValue(precedence);
    return node;
}

std::unique_ptr<Node> Parser::parseGroupedNodes() {
    getNextToken();

    auto node = parseRValue(LOWEST);

    if (!checkNextTokenAndAdvance(RPAREN)) {return nullptr;}

    return node;
}

std::unique_ptr<IfElseNode> Parser::parseIfNode() {
    auto ifNode = std::make_unique<IfElseNode>(currentToken);

    if (!checkNextTokenAndAdvance(LPAREN)) {return nullptr;}

    getNextToken();
    ifNode->condition = std::move(parseRValue(LOWEST));

    if (!checkNextTokenAndAdvance(RPAREN)) {return nullptr;}
    if (!checkNextTokenAndAdvance(LBRACE)) {return nullptr;}

    ifNode->consequence = std::move(parseBlockNode());

    if (nextTokenIs(ELSE)) {
        getNextToken();

        if (!checkNextTokenAndAdvance(LBRACE)) {return nullptr;}

        ifNode->alternative = std::move(parseBlockNode());
    }

    return ifNode;
}

std::vector<std::unique_ptr<Node>> Parser::parseNodeList(TokenType end) {
    auto list = std::vector<std::unique_ptr<Node>>{};
    if (nextTokenIs(end)) {
        getNextToken();
        return list;
    }

    getNextToken();
    auto node = parseRValue(LOWEST);

    list.push_back(std::move(parseRValue(LOWEST)));

    while (nextTokenIs(COMMA)) {
        getNextToken(2);
        list.push_back(std::move(parseRValue(LOWEST)));
    }

    if (!checkNextTokenAndAdvance(end)) {
        return std::vector<std::unique_ptr<Node>>{};
    }

    return list;
}

std::unique_ptr<Array> Parser::parseArray() {
    auto array = std::make_unique<Array>(currentToken);
    if (nextToken.Type == VARIADIC || nextToken.Type == INT) {
        getNextToken(3);
    } else if (nextToken.Type == RBRACKET) {
        getNextToken(2);
    }
    auto arrType = std::make_unique<ArrayType>(std::move(parseType()));
    array->type = std::move(arrType);
    getNextToken();

    if (!currentTokenIs(LBRACE)) {
        array->elements = std::vector<std::unique_ptr<Node>>{};
        getNextToken();
    } else {
        array->elements = std::move(parseNodeList(RBRACE));
    }
    return array;
}

std::unique_ptr<Node> Parser::parseIndex(std::unique_ptr<Node> left) {
    auto indexNode = std::make_unique<Index>(currentToken, std::move(left));
    getNextToken();
    indexNode->index = std::move(parseRValue(LOWEST));

    if (!checkNextTokenAndAdvance(RBRACKET)) {
        return nullptr;
    }

    return indexNode;
}

Parser::Parser(Lexer* l) : lexer(l) {
    registerPrefix(IDENTIFIER, [this]() { return this->parseIdentifier(); });
    registerPrefix(INT, [this]() { return this->parseIntegerLiteral(); });
    registerPrefix(STRING, [this]() { return this->parseStringLiteral(); });
    registerPrefix(BANG, [this]() { return this->parsePrefixNode(); });
    registerPrefix(MINUS, [this]() { return this->parsePrefixNode(); });
    registerPrefix(TRUE, [this]() { return this->parseBoolean(); });
    registerPrefix(FALSE, [this]() { return this->parseBoolean(); });
    registerPrefix(LPAREN, [this]() { return this->parseGroupedNodes(); });
    registerPrefix(IF, [this]() { return this->parseIfNode(); });
    registerPrefix(FUNCTION, [this]() { return this->parseFunctionDeclaration(); });
    registerPrefix(LBRACKET, [this]() { return this->parseArray(); });

    registerInfix(PLUS, [this](std::unique_ptr<Node> left) { return this->parseInfixNode(std::move(left)); });
    registerInfix(MINUS, [this](std::unique_ptr<Node> left) { return this->parseInfixNode(std::move(left)); });
    registerInfix(SLASH, [this](std::unique_ptr<Node> left) { return this->parseInfixNode(std::move(left)); });
    registerInfix(ASTERISK, [this](std::unique_ptr<Node> left) { return this->parseInfixNode(std::move(left)); });
    registerInfix(EQ, [this](std::unique_ptr<Node> left) { return this->parseInfixNode(std::move(left)); });
    registerInfix(NOT_EQ, [this](std::unique_ptr<Node> left) { return this->parseInfixNode(std::move(left)); });
    registerInfix(LESS_THAN, [this](std::unique_ptr<Node> left) { return this->parseInfixNode(std::move(left)); });
    registerInfix(GREATER_THAN, [this](std::unique_ptr<Node> left) { return this->parseInfixNode(std::move(left)); });
    registerInfix(LPAREN, [this](std::unique_ptr<Node> left) { return this->parseFunctionCall(std::move(left)); });
    registerInfix(LBRACKET, [this](std::unique_ptr<Node> left) { return this->parseIndex(std::move(left)); });

    getNextToken(2);
}

std::unique_ptr<Node> Parser::parseIdentifier() {
    if (nextToken.Type == DECLARE) {
        return parseDeclarationNode(SHORT_DECL);
    }
    return std::make_unique<Identifier>(currentToken, currentToken.Literal);
}

