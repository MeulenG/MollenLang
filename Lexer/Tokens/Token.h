#pragma once
#include <string>
class Token
{
private:
    /* data */
public:
    // The lexer returns tokens [0-255] if it is an unknown character, otherwise one
    // of these for known things.
    enum Type {
        // Identifier Section
        TOKEN_ID = 256, 
        
        // Keyword Section
        TOKEN_BEHAVIOR = 257, TOKEN_ACTION = 258, TOKEN_DEF = 259, TOKEN_EXTERN = 260,
        TOKEN_OBJECT = 261, TOKEN_INHERITS = 262, TOKEN_IF = 263, TOKEN_ELSE = 264,
        TOKEN_RETURN = 265, TOKEN_INT = 266, TOKEN_STRING = 267, TOKEN_OBJ = 268,
        TOKEN_CONSOLE = 269, TOKEN_NULL = 270,

        // Separator/Punctuator Section
        TOKEN_LEFT_PARENTHESES = 271, TOKEN_RIGHT_PARENTHESES = 272, TOKEN_RIGHT_BRACKET = 273, TOKEN_LEFT_BRACKET = 274,
        TOKEN_LEFT_CURLYBRAC = 275, TOKEN_RIGHT_CURLYBRAC = 276, TOKEN_SEMICOLON = 277, TOKEN_COMMA = 278,

        // Operator Section
        TOKEN_PLUS = 279, TOKEN_MINUS = 280, TOKEN_TIMES = 281, TOKEN_DIVIDE = 282, TOKEN_SQUARE = 283,
        TOKEN_EQUAL = 284, TOKEN_LESSTHAN = 285, TOKEN_GREATERTHAN = 286, TOKEN_NOTEQUAL = 287, TOKEN_GREATEREQUAL = 288,
        TOKEN_LESSEQUAL = 289, TOKEN_DOT = 290,

        // Literal Section
        TOKEN_NUM = 291,
        TOKEN_STRING_LITERAL = 292,

        // Comment
        TOKEN_SLASH = 293,

        //EOF
        TOKEN_EOF = 294,
    };

    struct molToken
    {
        Token::Type type;
        std::string text;

        molToken(Token::Type type, std::string text) : type(type), text(std::move(text)) {}    
    };
    
    static std::string getTokenName(int token);
};