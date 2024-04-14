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
    lex.scanFile(argv[1], "testfile.txt", "newfile.txt");
    return 0;
}
