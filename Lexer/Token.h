#include <string>
#pragma once

class Token
{
private:
    /* data */
public:
    // The lexer returns tokens [0-255] if it is an unknown character, otherwise one
    // of these for known things.
    enum tokenKind {
        // Identifier Section
        TOKEN_ID =  256,
        // // Keyword Section
        TOKEN_BEHAVIOR = 257,
        TOKEN_ACTION = 258,
        // TOKEN_IF = 257,
        // TOKEN_ELSE = 258,
        // TOKEN_WHILE = 259,
        // TOKEN_RETURN = 260,
        // TOKEN_DO = 261,
        // TOKEN_BREAK = 262,

        // Separator/Punctuator Section
        TOKEN_LEFT_PARENTHESES = 263,
        TOKEN_RIGHT_PARENTHESES = 264,
        TOKEN_LEFT_CURLYBRAC = 265,
        TOKEN_RIGHT_CURLYBRAC = 266,
        TOKEN_SEMICOLON = 267,
        TOKEN_COMMA = 268,

        // //Operator Section
        // TOKEN_PLUS = 269,
        // TOKEN_MINUS = 270,
        // TOKEN_TIMES = 271,
        TOKEN_DIVIDE = 272,
        // TOKEN_SQUARE = 273,
        // TOKEN_EQUAL = 274,
        // TOKEN_LESSTHAN = 275,
        // TOKEN_GREATERTHAN = 276,
        // TOKEN_NOTEQUAL = 277,
        // TOKEN_GREATEREQUAL = 278,
        // TOKEN_LESSEQUAL = 279,

        // // Literal Section
        // TOKEN_TRUE = 280,
        TOKEN_NUM = 281,
        // TOKEN_REAL = 282,

        // Comment
        TOKEN_SLASH = 283,
        // Whitespace

        //EOF
        TOKEN_EOF = 284,
    };
};