//
// Created by oliver on 3/19/24.
//

#ifndef GO_TO_TS_SIMPLE_COMPILER_TOKEN_H
#define GO_TO_TS_SIMPLE_COMPILER_TOKEN_H

#include <string>
#include <unordered_map>
#include <utility>
#include <iostream>

using TokenType = std::string;

const TokenType ILLEGAL = "ILLEGAL";
const TokenType END_OF_FILE = "EOF";
const TokenType IDENTIFIER = "IDENTIFIER";

// Operators
const TokenType ASSIGN = "=";
const TokenType DECLARE = ":=";
const TokenType PLUS = "+";
const TokenType MINUS = "-";
const TokenType BANG = "!";
const TokenType ASTERISK = "*";
const TokenType SLASH = "/";

// Keywords
const TokenType FUNCTION = "FUNCTION";
const TokenType CONST = "CONST";
const TokenType TRUE = "TRUE";
const TokenType FALSE = "FALSE";
const TokenType IF = "IF";
const TokenType ELSE = "ELSE";
const TokenType RETURN = "RETURN";
const TokenType INT = "INT";
const TokenType STRING = "STRING";

// Other tokens
const TokenType EQ = "==";
const TokenType NOT_EQ = "!=";
const TokenType LESS_THAN = "<";
const TokenType GREATER_THAN = ">";
const TokenType COMMA = ",";
const TokenType SEMICOLON = ";";
const TokenType LPAREN = "(";
const TokenType RPAREN = ")";
const TokenType LBRACE = "{";
const TokenType RBRACE = "}";
const TokenType LBRACKET = "[";
const TokenType RBRACKET = "]";
const TokenType COLON = ":";

struct Token {
    Token(TokenType type, char literal) : Type(std::move(type)), Literal(std::string{literal}) {};
    Token(TokenType type, std::string literal) : Type(std::move(type)), Literal(std::move(literal)) {};
    Token() : Type(ILLEGAL) {};
    TokenType Type;
    std::string Literal;
    friend std::ostream& operator<<(std::ostream& os, const Token& token);
    friend bool operator==(Token &lhs, Token &rhs) { return lhs.Type == rhs.Type; }
};

extern std::unordered_map<std::string, TokenType> keywords;

TokenType LookupIdent(const std::string& ident);

#endif //GO_TO_TS_SIMPLE_COMPILER_TOKEN_H
