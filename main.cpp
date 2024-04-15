#include <iostream>
#include <fstream>
#include <string>
#include "Lexer/Lexer.h"


int main(int argc, char* argv[]) {
    // Check if a filename was passed
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " SOURCE_PROGRAM\n";
        return 1;
    }

    Lexer lex;
    Token tok;
    lex.readFileDescriptor(argv[1], "testfile.txt");
    for (int token = lex.Tokenize(argv[1]); token != tok.TOKEN_EOF; token++) // should be .getnext or smth
    {
        printf("%d \n", token);
    }
    
    return 0;
}