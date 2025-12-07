#include "Token.h"
#include <map>
#include <string>
#include <cctype>

std::string Token::getTokenName(int token) {
    static const std::map<int, std::string> tokenNames = {
        {TOKEN_ID, "IDENTIFIER"},
        {TOKEN_BEHAVIOR, "behavior"},
        {TOKEN_ACTION, "action"},
        {TOKEN_DEF, "def"},
        {TOKEN_EXTERN, "extern"},
        {TOKEN_OBJECT, "object"},
        {TOKEN_INHERITS, "inherits"},
        {TOKEN_IF, "if"},
        {TOKEN_ELSE, "else"},
        {TOKEN_RETURN, "return"},
        {TOKEN_INT, "int"},
        {TOKEN_STRING, "string"},
        {TOKEN_OBJ, "obj"},
        {TOKEN_CONSOLE, "Console"},
        {TOKEN_NULL, "null"},
        {TOKEN_LEFT_PARENTHESES, "("},
        {TOKEN_RIGHT_PARENTHESES, ")"},
        {TOKEN_LEFT_BRACKET, "["},
        {TOKEN_RIGHT_BRACKET, "]"},
        {TOKEN_LEFT_CURLYBRAC, "{"},
        {TOKEN_RIGHT_CURLYBRAC, "}"},
        {TOKEN_SEMICOLON, ";"},
        {TOKEN_COMMA, ","},
        {TOKEN_PLUS, "+"},
        {TOKEN_MINUS, "-"},
        {TOKEN_TIMES, "*"},
        {TOKEN_DIVIDE, "/"},
        {TOKEN_EQUAL, "=="},
        {TOKEN_LESSTHAN, "<"},
        {TOKEN_GREATERTHAN, ">"},
        {TOKEN_NOTEQUAL, "!="},
        {TOKEN_GREATEREQUAL, ">="},
        {TOKEN_LESSEQUAL, "<="},
        {TOKEN_DOT, "."},
        {TOKEN_NUM, "NUMBER"},
        {TOKEN_STRING_LITERAL, "STRING_LITERAL"},
        {TOKEN_SLASH, "/"},
        {TOKEN_EOF, "EOF"}
    };
    
    auto it = tokenNames.find(token);
    if (it != tokenNames.end()) {
        return it->second;
    }
    
    // For ASCII characters, return the character
    if (token >= 0 && token < 256) {
        if (isprint(token)) {
            return std::string(1, static_cast<char>(token));
        } else {
            return "CHAR(" + std::to_string(token) + ")";
        }
    }
    
    return "UNKNOWN_TOKEN(" + std::to_string(token) + ")";
}

