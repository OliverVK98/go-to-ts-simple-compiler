//
// Created by oliver on 3/19/24.
//

#include "token.h"

std::unordered_map<std::string, TokenType> keywords = {
        {"func",     FUNCTION},
        {"const",    CONST},
        {"true",   TRUE},
        {"false",  FALSE},
        {"if",     IF},
        {"else",   ELSE},
        {"return", RETURN},
};

TokenType LookupIdent(const std::string& ident) {
    auto it = keywords.find(ident);
    if (it != keywords.end()) {
        return it->second;
    }
    return IDENTIFIER;
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token(Type: " << token.Type << ", Literal: " << token.Literal << ")";
    return os;
}