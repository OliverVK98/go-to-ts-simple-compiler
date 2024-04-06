//
// Created by oliver on 3/19/24.
//

#include "lexer.h"

#include <utility>
#include "../logger/logger.h"

void Lexer::readChar() {
    if (nextPosition >= input.length()) {
        ch = '\0';
    } else {
        ch = input[nextPosition];
    }

    position = nextPosition;
    nextPosition++;
}

Token newToken(const TokenType& tokenType, char ch) {
    return Token{tokenType, ch};
}

Token newToken(const TokenType& tokenType, std::string ch) {
    return Token{tokenType, std::move(ch)};
}

std::string Lexer::readIdentifierOrType() {
    auto startPosition = position;
    while (isLetter(ch)) {
        readChar();
    }

    return input.substr(startPosition, position - startPosition);
}

std::string Lexer::readNumber() {
    auto startPosition = position;
    while (isDigit(ch)) {
        readChar();
    }

    return input.substr(startPosition, position - startPosition);
}

void Lexer::skipWhitespace() {
    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
        readChar();
    }
}

char Lexer::peekChar() {
    if (nextPosition >= input.length()) {
        return '0';
    } else {
        return input[nextPosition];
    }
}

std::string Lexer::peekTwo() {
    if (nextPosition >= input.length() && nextPosition + 1 >= input.length()) {
        return "";
    } else {
        return std::string(1, input[nextPosition]) + std::string(1, input[nextPosition + 1]);
    }
}

std::string Lexer::readString() {
    auto startPosition = position + 1 ;
    while (true) {
        readChar();
        if (ch == '"' || ch == 0) {
            break;
        }
    }

    return input.substr(startPosition, position-startPosition);
}

Token Lexer::nextToken() {
    Token tok{};

    skipWhitespace();

    switch (ch) {
        case '=':
            if (peekChar() == '=') {
                auto prevCh = ch;
                readChar();
                std::string literal = std::string{prevCh} + std::string{ch};
                tok = Token{EQ, literal};
            } else {
                tok = newToken(ASSIGN, ch);
            }
            break;
        case '+':
            tok = newToken(PLUS, ch);
            break;
        case '-':
            tok = newToken(MINUS, ch);
            break;
        case '!':
            if (peekChar() == '=') {
                auto prevCh = ch;
                readChar();
                std::string literal = std::string{prevCh} + std::string{ch};
                tok = Token{NOT_EQ, literal};
            } else {
                tok = newToken(BANG, ch);
            }
            break;
        case '/':
            tok = newToken(SLASH, ch);
            break;
        case '.':
            if (peekTwo() == "..") {
                tok = newToken(VARIADIC, "...");
                readChar();
                readChar();
                break;
            }
        case '*':
            tok = newToken(ASTERISK, ch);
            break;
        case '<':
            tok = newToken(LESS_THAN, ch);
            break;
        case '>':
            tok = newToken(GREATER_THAN, ch);
            break;
        case '(':
            tok = newToken(LPAREN, ch);
            break;
        case ')':
            tok = newToken(RPAREN, ch);
            break;
        case ',':
            tok = newToken(COMMA, ch);
            break;
        case '{':
            tok = newToken(LBRACE, ch);
            break;
        case '}':
            tok = newToken(RBRACE, ch);
            break;
        case '"':
            tok.Type = STRING;
            tok.Literal = readString();
            break;
        case '[':
            tok = newToken(LBRACKET, ch);
            break;
        case ']':
            tok = newToken(RBRACKET, ch);
            break;
        case ':':
            if (peekChar() == '=') {
                auto prevCh = ch;
                readChar();
                std::string literal = std::string{prevCh} + std::string{ch};
                tok = Token{DECLARE, literal};
            } else {
                tok = newToken(COLON,  ch);
            }
            break;
        case '\0':
            tok.Literal = "";
            tok.Type = END_OF_FILE;
            break;
        default:
            if (isLetter(ch)) {
                tok.Literal = readIdentifierOrType();
                if (LookupType(tok.Literal) != NOTYPE_TYPE) {
                    tok.Type = LookupType(tok.Literal);
                    return tok;
                }
                tok.Type = LookupIdent(tok.Literal);
                return tok;
            } else if (isDigit(ch)) {
                tok.Type = INT;
                tok.Literal = readNumber();
                return tok;
            } else {
                tok = newToken(ILLEGAL, ch);
                break;
            }
    }

    readChar();
    return tok;
}