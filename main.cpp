#include "Lexer/Scanner/Scanner.h"
#include "Lexer/Lexer.h"
#include "Lexer/Helper/Helper.h"
#include "Parser/Parser.h"
#include "Lexer/Tokens/Token.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <vector>


int main(int argc, char* argv[]) {
    Lexer lex;
    Parser parser;
    // Install standard binary operators.
    // 1 is lowest precedence.
    parser.BinopPrecedence['<'] = 10;
    parser.BinopPrecedence['+'] = 20;
    parser.BinopPrecedence['-'] = 20;
    parser.BinopPrecedence['*'] = 40; // highest.
    fprintf(stderr, "ready> ");
    parser.currentToken = parser.getNextToken(); 
    // Make the module, which holds all the code.
    parser.InitializeModule();   
    /// top ::= definition | external | expression | ';'
    while (true) {
        fprintf(stderr, "ready> ");
        switch (parser.currentToken) {
            case Token::Type::TOKEN_EOF:
                return -1;
            case ';': // ignore top-level semicolons.
                parser.getNextToken();
                break;
            case Token::Type::TOKEN_DEF:
                parser.HandleDefinition();
                break;
            case Token::Type::TOKEN_EXTERN:
                parser.HandleExtern();
                break;
            default:
                parser.HandleTopLevelExpression();
                break;
        }
    }
    return 0;
}