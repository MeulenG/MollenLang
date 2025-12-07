#include "Lexer/Scanner/Scanner.h"
#include "Lexer/Lexer.h"
#include "Lexer/Helper/Helper.h"
#include "Parser/Parser.h"
#include "Lexer/Tokens/Token.h"
#include "CodeGen/Codegen.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <vector>


int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file.mol>\n", argv[0]);
        return 1;
    }

    Lexer lex;
    Token tok;
    if (!lex.setInputFile(argv[1])) {
        fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    Parser parser(lex, tok);
    // Install standard binary operators.
    // 1 is lowest precedence.
    parser.BinopPrecedence['<'] = 10;
    parser.BinopPrecedence['+'] = 20;
    parser.BinopPrecedence['-'] = 20;
    parser.BinopPrecedence['*'] = 40; // highest.
    
    // Make the module, which holds all the code.
    Codegen::InitializeModule();   
    
    parser.currentToken = parser.getNextToken(); 
    /// top ::= definition | external | behavior | object | expression | ';'
    while (parser.currentToken != Token::Type::TOKEN_EOF) {
        switch (parser.currentToken) {
            case Token::Type::TOKEN_SEMICOLON: // ignore top-level semicolons.
            case ';': // also handle ASCII semicolon
                parser.getNextToken();
                break;
            case Token::Type::TOKEN_DEF:
                parser.HandleDefinition();
                // ParseDefinition consumes tokens, advance to next
                parser.getNextToken();
                break;
            case Token::Type::TOKEN_EXTERN:
                parser.HandleExtern();
                // ParseExtern consumes tokens, advance to next
                parser.getNextToken();
                break;
            case Token::Type::TOKEN_BEHAVIOR:
                parser.HandleBehavior();
                // ParseBehavior consumes all tokens including closing brace
                // currentToken is already the next token, no need to advance
                break;
            case Token::Type::TOKEN_OBJECT:
                parser.HandleObject();
                // ParseObject consumes all tokens including semicolon
                // currentToken is already the next token, no need to advance
                break;
            default:
                parser.HandleTopLevelExpression();
                // ParseTopLevelExpr consumes expression tokens
                // Advance to next token
                parser.getNextToken();
                break;
        }
    }
    return 0;
}