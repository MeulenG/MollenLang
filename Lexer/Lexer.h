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
    int scanFile(const char *fileName1, const char *fileName2, const char *fileName3);
    int Tokenize(const char *fileName);

};