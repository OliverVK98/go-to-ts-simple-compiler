//
// Created by oliver on 3/19/24.
//

#ifndef GO_TO_TS_SIMPLE_COMPILER_LEXER_H
#define GO_TO_TS_SIMPLE_COMPILER_LEXER_H


#include <string>
#include <utility>
#include "../token/token.h"

class Lexer {
public:
    std::string input;
    int position{};
    int nextPosition{};
    char ch{};

    explicit Lexer(std::string input) : input(std::move(input)) {
        readChar();
    }

    void readChar();
    Token nextToken();
    std::string readIdentifierOrType();
    std::string readNumber();
    void skipWhitespace();
    char peekChar();
    std::string peekTwo();
    std::string readString();
};

Token newToken(const TokenType& tokenType, char ch);
inline bool isLetter(char ch) { return 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == '_'; }
inline bool isDigit(char ch) { return '0' <= ch && ch <= '9'; }

#endif //GO_TO_TS_SIMPLE_COMPILER_LEXER_H
