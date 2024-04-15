#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Token.h"
#pragma once


class Lexer
{
private:
public:
    int line = 1;
    char peek = ' ';
    std::string IdentifierStr; // Filled in if tok_identifier
    double NumVal;             // Filled in if tok_number
    int currentToken;
    // int getNextToken() { return currentToken = getToken(); }
    int readAndRemoveWS(int fd_in, int fd_out);
    int readFileDescriptor(const char *file, const char *file2);
    int createCopyFromMemory(int fd, const void *buf, size_t n);
    int Tokenize(const char *fileName);

};