#include <string>
#pragma once

class Token
{
private:
    /* data */
public:
    enum class Type {
        TOKEN_KEYWORD,
        TOKEN_IDENTIFIER,
        TOKEN_FUNCTION,
        TOKEN_LITERAL,
        TOKEN_OPERATOR,
        TOKEN_UNKNOWN
    };

    struct molToken
    {
        Token::Type type;
        std::string text;

        molToken(Token::Type type, std::string text) : type(type), text(std::move(text)) {}    
    };

};


