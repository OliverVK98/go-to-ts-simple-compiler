//
// Created by oliver on 3/21/24.
//

#include "parser.h"
#include "../logger/logger.h"

#include <utility>

bool startsWithType(const std::string& str) {
    const std::string prefix = "type_";
    if (str.length() < prefix.length()) {
        return false; // String is too short to contain the prefix
    }
    // Check if the beginning of str matches prefix
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

//std::unique_ptr<ConstNode> Parser::parseConstNode() {
//    auto node = std::make_unique<ConstNode>(currentToken);
//
//    if (checkNextTokenAndAdvance(IDENTIFIER)) {
//        node->name = std::make_unique<Identifier>(currentToken, currentToken.Literal);
//
//        if (startsWithType(nextToken.Type)) {
//            getNextToken();
//            node->type = parseType();
//        }
//
//        if (!nextTokenIs(ASSIGN)) {
//            if (node->type) {
//                node->value = std::make_unique<Integer>();
//                node->value->holdsValue = false;
//                if (nextTokenIs(SEMICOLON)) {
//                    getNextToken();
//                }
//                return node;
//            } else {
//                throw std::runtime_error("Unknown const declaration");
//            }
//        }
//
//        getNextToken();
//        getNextToken();
//
//        node->value = parseRHValue(LOWEST);
//
//        if (!node->type) {
//            if (dynamic_cast<Integer*>(node->value.get())) {
//                node->type = std::make_unique<IntegerType>();
//            } else if (dynamic_cast<String*>(node->value.get())) {
//                node->type = std::make_unique<StringType>();
//            } else if (dynamic_cast<Boolean*>(node->value.get())) {
//                node->type = std::make_unique<BoolType>();
//            } else {
//                throw std::runtime_error("Unhandled const type");
//            }
//        }
//
//        if (auto fl = dynamic_cast<Function*>(node->value.get())) {
//            fl->funcName = node->name->value;
//        }
//
//    } else {
//        getNextToken();
//        if (currentTokenIs(LPAREN)) {
//            getNextToken();
//            node->holdsMultipleValues = true;
//            while (currentTokenIs(IDENTIFIER)) {
//                auto newNode = std::make_unique<ConstNode>();
//                newNode->name = std::move(parseIdentifier());
//                getNextToken();
//                if (tokenTypeIsTypeNode(currentToken.Type)) {
//                    newNode->type = parseType();
//                    getNextToken();
//                }
//                getNextToken();
//                newNode->value = parseRHValue(LOWEST);
//                if (!newNode->type) {
//                    if (dynamic_cast<Integer*>(newNode->value.get())) {
//                        newNode->type = std::make_unique<IntegerType>();
//                    } else if (dynamic_cast<String*>(newNode->value.get())) {
//                        newNode->type = std::make_unique<StringType>();
//                    } else if (dynamic_cast<Boolean*>(newNode->value.get())) {
//                        newNode->type = std::make_unique<BoolType>();
//                    } else {
//                        throw std::runtime_error("Unhandled const type");
//                    }
//                }
//
//                node->multipleValues.push_back(std::move(newNode));
//                getNextToken();
//            }
//
//        } else {
//            throw std::runtime_error("Unhandled const variable declaration");
//        }
//    }
//
//    if (nextTokenIs(SEMICOLON)) {
//        getNextToken();
//    }
//
//    return node;
//}

std::unique_ptr<Node> Parser::parseDeclarationNode(DeclarationType declType) {
    auto node = std::make_unique<Declaration>(currentToken);
    if (declType == CONST_DECL) node->isConstant = true;

    if (checkNextTokenAndAdvance(IDENTIFIER)) {
        // Parsing non-grouped declarations
        node->name = std::make_unique<Identifier>(currentToken, currentToken.Literal);

        if(nextTokenIs(LBRACKET)) {
            // Parsing var array types
            if (node->isConstant) throw std::runtime_error("Constant array!");
            getNextToken();
            node->type = parseType();
        } else if (startsWithType(nextToken.Type)) {
            // Parsing explicit type decl for const/var
            getNextToken();
            node->type = parseType();
        }

        if (!nextTokenIs(ASSIGN)) {
            // Parsing vars with no value and checking that they have types
            if (node->type) {
                node->value = std::make_unique<Integer>();
                node->value->holdsValue = false;
                if (nextTokenIs(SEMICOLON)) {
                    getNextToken();
                }
                return node;
            } else {
                throw std::runtime_error("Unknown declaration");
            }
        }

        getNextToken();
        getNextToken();

        if (currentTokenIs(LBRACKET)) {
            // Parsing var array values
            if (node->isConstant) throw std::runtime_error("Constant array!");
            auto arr = parseArray();
            node->type = std::move(arr->type);
            node->value = std::move(arr);
        } else {
            // Parsing non-array const/var values
            node->value = parseRHValue(LOWEST);
        }

        if (!node->type) {
            // Inferring type if not explicit
            if (dynamic_cast<Integer*>(node->value.get())) {
                node->type = std::make_unique<IntegerType>();
            } else if (dynamic_cast<String*>(node->value.get())) {
                node->type = std::make_unique<StringType>();
            } else if (dynamic_cast<Boolean*>(node->value.get())) {
                node->type = std::make_unique<BoolType>();
            } else {
                throw std::runtime_error("Unhandled var type");
            }
        }

    } else if (checkNextTokenAndAdvance(LPAREN)) {
        getNextToken();
        node->holdsMultipleValues = true;

        while (currentTokenIs(IDENTIFIER)) {
            auto newNode = std::make_unique<Declaration>();
            if (declType == CONST_DECL) newNode->isConstant = true;
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
                // Parsing non - arrays
                if (tokenTypeIsTypeNode(currentToken.Type)) {
                    newNode->type = parseType();
                    getNextToken();
                }
                if(currentTokenIs(ASSIGN)) {
                    getNextToken();
                    newNode->value = parseRHValue(LOWEST);
                    if (!newNode->type) {
                        if (dynamic_cast<Integer*>(newNode->value.get())) {
                            newNode->type = std::make_unique<IntegerType>();
                        } else if (dynamic_cast<String*>(newNode->value.get())) {
                            newNode->type = std::make_unique<StringType>();
                        } else if (dynamic_cast<Boolean*>(newNode->value.get())) {
                            newNode->type = std::make_unique<BoolType>();
                        } else if (auto arr = dynamic_cast<Array*>(newNode->value.get())) {
                            auto arrType = std::make_unique<ArrayType>();

                            if (dynamic_cast<Integer*>(arr->elements[0].get())) {
                                arrType->subType = std::make_unique<IntegerType>();
                            } else if (dynamic_cast<String*>(arr->elements[0].get())) {
                                arrType->subType = std::make_unique<StringType>();
                            } else if (dynamic_cast<Boolean*>(arr->elements[0].get())) {
                                arrType->subType = std::make_unique<BoolType>();
                            }

                            newNode->type = std::move(arrType);
                        } else {
                            throw std::runtime_error("Unhandled var type");
                        }
                    }
                    getNextToken();

                } else {
                    newNode->value = std::make_unique<Integer>();
                    newNode->value->holdsValue = false;
                }

            }

            node->multipleValues.push_back(std::move(newNode));
        }
    } else {
        throw std::runtime_error("Unknown variable declaration");
    }

    if (nextTokenIs(SEMICOLON)) {
        getNextToken();
    }

    return node;
}

//std::unique_ptr<VarNode> Parser::parseVarNode() {
//    auto node = std::make_unique<VarNode>(currentToken);
//    if (checkNextTokenAndAdvance(IDENTIFIER)) {
//        node->name = std::make_unique<Identifier>(currentToken, currentToken.Literal);
//
//        if(nextTokenIs(LBRACKET)) {
//            // Parsing var arrays
//            getNextToken();
//            node->type = parseType();
//        } else if (startsWithType(nextToken.Type)) {
//            // Parsing explicit type decl
//            getNextToken();
//            node->type = parseType();
//        }
//
//        if (!nextTokenIs(ASSIGN)) {
//            // Parsing vars with no value
//            if (node->type) {
//                node->value = std::make_unique<Integer>();
//                node->value->holdsValue = false;
//                if (nextTokenIs(SEMICOLON)) {
//                    getNextToken();
//                }
//                return node;
//            } else {
//                throw std::runtime_error("Unknown var declaration");
//            }
//        }
//
//        getNextToken();
//        getNextToken();
//
//        if (currentTokenIs(LBRACKET)) {
//            // Parsing array values
//            auto arr = parseArray();
//            node->type = std::move(arr->type);
//            node->value = std::move(arr);
//        } else {
//            // Parsing non-array values
//            node->value = parseRHValue(LOWEST);
//        }
//
//        if (!node->type) {
//            // Inferring type if not explicit
//            if (dynamic_cast<Integer*>(node->value.get())) {
//                node->type = std::make_unique<IntegerType>();
//            } else if (dynamic_cast<String*>(node->value.get())) {
//                node->type = std::make_unique<StringType>();
//            } else if (dynamic_cast<Boolean*>(node->value.get())) {
//                node->type = std::make_unique<BoolType>();
//            } else {
//                throw std::runtime_error("Unhandled var type");
//            }
//        }
//
//        if (auto fl = dynamic_cast<Function*>(node->value.get())) {
//            fl->funcName = node->name->value;
//        }
//
//    } else {
//        getNextToken();
//        if (currentTokenIs(LPAREN)) {
//            getNextToken();
//            node->holdsMultipleValues = true;
//
//            while (currentTokenIs(IDENTIFIER)) {
//                auto newNode = std::make_unique<VarNode>();
//                newNode->name = parseIdentifier();
//                getNextToken();
//                if (currentTokenIs(LBRACKET)) {
//                    newNode->type = parseType();
//                    if (checkNextTokenAndAdvance(ASSIGN)) {
//                        getNextToken();
//                        newNode->value = std::move(parseArray());
//                        node->multipleValues.push_back(std::move(newNode));
//                        getNextToken();
//                    } else {
//                        newNode->value = std::make_unique<Array>();
//                        newNode->value->holdsValue = false;
//                        node->multipleValues.push_back(std::move(newNode));
//                    }
//
//                } else {
//                    // Parsing non - arrays
//                    if (tokenTypeIsTypeNode(currentToken.Type)) {
//                        newNode->type = parseType();
//                        getNextToken();
//                    }
//                    if(currentTokenIs(ASSIGN)) {
//                        getNextToken();
//                        newNode->value = parseRHValue(LOWEST);
//                        if (!newNode->type) {
//                            if (dynamic_cast<Integer*>(newNode->value.get())) {
//                                newNode->type = std::make_unique<IntegerType>();
//                            } else if (dynamic_cast<String*>(newNode->value.get())) {
//                                newNode->type = std::make_unique<StringType>();
//                            } else if (dynamic_cast<Boolean*>(newNode->value.get())) {
//                                newNode->type = std::make_unique<BoolType>();
//                            } else if (auto arr = dynamic_cast<Array*>(newNode->value.get())) {
//                                auto arrType = std::make_unique<ArrayType>();
//
//                                if (dynamic_cast<Integer*>(arr->elements[0].get())) {
//                                    arrType->subType = std::make_unique<IntegerType>();
//                                } else if (dynamic_cast<String*>(arr->elements[0].get())) {
//                                    arrType->subType = std::make_unique<StringType>();
//                                } else if (dynamic_cast<Boolean*>(arr->elements[0].get())) {
//                                    arrType->subType = std::make_unique<BoolType>();
//                                }
//
//                                newNode->type = std::move(arrType);
//                            } else {
//                                throw std::runtime_error("Unhandled var type");
//                            }
//                        }
//                        getNextToken();
//
//                    } else {
//
//                        newNode->value = std::make_unique<Integer>();
//                        newNode->value->holdsValue = false;
//                        node->multipleValues.push_back(std::move(newNode));
//                    }
//
//                }
//
//                node->multipleValues.push_back(std::move(newNode));
//            }
//            getNextToken();
//        } else {
//            // TODO: add array support;
//        }
//    }
//
//    if (nextTokenIs(SEMICOLON)) {
//        getNextToken();
//    }
//    return node;
//}

std::unique_ptr<ReturnNode> Parser::parseReturnNode() {
    auto node = std::make_unique<ReturnNode>(currentToken);

    getNextToken();

    node->value = parseRHValue(LOWEST);

    if (nextTokenIs(SEMICOLON)) {
        getNextToken();
    }

    return node;
}

std::unique_ptr<Node> Parser::parseRHValueNode() {
    auto node = std::make_unique<RHValue>(currentToken);

    node->value = parseRHValue(LOWEST);

    if (nextTokenIs(SEMICOLON)) {
        getNextToken();
    }

    return node;
}

std::unique_ptr<Node> Parser::parseRHValue(const int &precedence) {
    auto prefix = prefixParseFns[currentToken.Type];

    if (!prefix) {
        return nullptr;
    }

    auto leftExp = prefix();

    while (!nextTokenIs(SEMICOLON) && precedence < peekPrecedence()) {
        auto infix = infixParseFns[nextToken.Type];
        if (!infix) {
            return nullptr;
        }

        getNextToken();
        leftExp = std::unique_ptr<Node>(infix(std::move(leftExp)));
    };

    logger console;
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
        return parseRHValueNode();
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
    node->right = parseRHValue(PREFIX);
    return node;
}

std::unique_ptr<Infix> Parser::parseInfixNode(std::unique_ptr<Node> left) {
    auto node = std::make_unique<Infix>(currentToken, currentToken.Literal, std::move(left));
    auto precedence = currentPrecedence();
    getNextToken();
    node->right = parseRHValue(precedence);
    return node;
}

std::unique_ptr<CodeBlock> Parser::parseBlockNode() {
    auto block = std::make_unique<CodeBlock>(currentToken);
    block->nodes = std::move(std::vector<std::unique_ptr<Node>>{});

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

std::unique_ptr<Function> Parser::parseFunctionDeclaration() {
    auto func = std::make_unique<Function>(currentToken);

    if(!nextTokenIs(LPAREN) && !nextTokenIs(IDENTIFIER)) {
        return nullptr;
    }

    getNextToken();

    if (currentTokenIs(IDENTIFIER)) {
        func->funcName = parseIdentifier()->testString();
        getNextToken();
    }

    func->parameters = parseFunctionParameters();

    if(!checkNextTokenAndAdvance(LBRACE)) {
        return nullptr;
    }

    func->body = parseBlockNode();

    return func;
}

std::vector<std::unique_ptr<Identifier>> Parser::parseFunctionParameters() {
    auto params = std::vector<std::unique_ptr<Identifier>>{};

    if (nextTokenIs(RPAREN)) {
        getNextToken();
        return params;
    }

    getNextToken();

    auto param = std::make_unique<Identifier>(currentToken, currentToken.Literal);
    params.emplace_back(std::move(param));

    while (nextTokenIs(COMMA)) {
        getNextToken();
        getNextToken();
        auto newParam = std::make_unique<Identifier>(currentToken, currentToken.Literal);
        params.emplace_back(std::move(newParam));
    }

    if (!checkNextTokenAndAdvance(RPAREN)) {
        return std::vector<std::unique_ptr<Identifier>>{};
    }

    return params;
}

std::unique_ptr<Node> Parser::parseGroupedNodes() {
    getNextToken();

    auto node = parseRHValue(LOWEST);

    if (!checkNextTokenAndAdvance(RPAREN)) {return nullptr;}

    return node;
}

std::unique_ptr<IfElseNode> Parser::parseIfNode() {
    auto ifNode = std::make_unique<IfElseNode>(currentToken);

    if (!checkNextTokenAndAdvance(LPAREN)) {return nullptr;}

    getNextToken();
    ifNode->condition = std::move(parseRHValue(LOWEST));

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

std::unique_ptr<FunctionCall> Parser::parseFunctionCall(std::unique_ptr<Node> func) {
    auto funcCall = std::make_unique<FunctionCall>(currentToken, std::move(func));
    funcCall->args = std::move(parseNodeList(RPAREN));

    return funcCall;
}

std::vector<std::unique_ptr<Node>> Parser::parseNodeList(TokenType end) {
    auto list = std::vector<std::unique_ptr<Node>>{};
    if (nextTokenIs(end)) {
        getNextToken();
        return list;
    }

    getNextToken();
    auto node = parseRHValue(LOWEST);

    list.push_back(std::move(parseRHValue(LOWEST)));

    while (nextTokenIs(COMMA)) {
        getNextToken();
        getNextToken();
        list.push_back(std::move(parseRHValue(LOWEST)));
    }

    if (!checkNextTokenAndAdvance(end)) {
        return std::vector<std::unique_ptr<Node>>{};
    }

    return list;
}

std::unique_ptr<Array> Parser::parseArray() {
    auto array = std::make_unique<Array>(currentToken);
    if (nextToken.Type == VARIADIC) {
        array->size = -1;
    } else {
        auto intSize = std::stoi(nextToken.Literal);
        array->size = intSize;
    }
    getNextToken();
    getNextToken();
    getNextToken();
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
    indexNode->index = std::move(parseRHValue(LOWEST));

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

    getNextToken();
    getNextToken();
}

std::unique_ptr<TypeNode> Parser::parseType() {
    if (currentToken.Literal == LBRACKET) {
        getNextToken();
        getNextToken();
        getNextToken();
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
