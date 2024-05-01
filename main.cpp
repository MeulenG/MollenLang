#include "Lexer/Scanner/Scanner.h"
#include "Lexer/Lexer.h"
#include "Lexer/Helper/Helper.h"
#include "Parser/Parser.h"
#include "Lexer/Tokens/Token.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>


int main(int argc, char* argv[]) {
    // // Check if a filename was passed
    // if (argc < 2) {
    //     std::cerr << "Usage: " << argv[0] << " SOURCE_PROGRAM\n";
    //     return 1;
    // }

    // Scanner scan;
    // int source_fd = open(argv[1], O_RDWR, 0);
	// if (source_fd < 0)
	// {
	// 	perror("open source");
	// 	return EXIT_FAILURE;
	// }
    // off_t fileSize = (scan.getFileSize(source_fd));
    // std::vector<char> fileData = scan.readAndRemoveWS(source_fd, fileSize);
    // scan.copyFile("testfile.txt", fileData);

    // close(source_fd);

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