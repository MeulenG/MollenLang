#include <string>
#pragma once

class TokenType
{
private:
    /* data */
public:
    // The lexer returns tokens [0-255] if it is an unknown character, otherwise one
    // of these for known things.
    enum Identifier {
        // Identifier Section
        TOKEN_ID = 256,
    };

    enum Keyword {
    // Keyword Section
    TOKEN_BEHAVIOR = 257, TOKEN_ACTION = 258, // TOKEN_IF = 257, // TOKEN_ELSE = 258, // TOKEN_WHILE = 259,
    // TOKEN_RETURN = 260, // TOKEN_DO = 261, // TOKEN_BREAK = 262,
    };
    
    enum Seperator {
    // Separator/Punctuator Section
    TOKEN_LEFT_PARENTHESES = 263, TOKEN_RIGHT_PARENTHESES = 264, TOKEN_RIGHT_BRACKET = 265, TOKEN_LEFT_BRACKET = 266,
    TOKEN_LEFT_CURLYBRAC = 267, TOKEN_RIGHT_CURLYBRAC = 268, TOKEN_SEMICOLON = 269, TOKEN_COMMA = 270,
    };
    
    enum Operator {
    // Operator Section
    TOKEN_PLUS = 271, TOKEN_MINUS = 272, TOKEN_TIMES = 273, TOKEN_DIVIDE = 274, TOKEN_SQUARE = 275,
    TOKEN_EQUAL = 276, TOKEN_LESSTHAN = 277, TOKEN_GREATERTHAN = 278, TOKEN_NOTEQUAL = 279, TOKEN_GREATEREQUAL = 280,
    TOKEN_LESSEQUAL = 281,
    };
    
    enum Literal {
    // // Literal Section
    // TOKEN_TRUE = 280,
    TOKEN_NUM = 282,
    // TOKEN_REAL = 282,
    };
    
    enum Control {
    //EOF
    TOKEN_EOF = 284,
    };

    struct Token
    {
        int type;
        std::string text;

        Token(int t, const std::string& s) : type(t), text(s) { }
    };
    
};