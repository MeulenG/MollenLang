#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Token.h"
#include <vector>
#pragma once


class Lexer
{
private:
public:    
    int isTokenIdentifier(char c, FILE* fp, std::string sb);
    int isTokenNumber(char c, FILE* fp);
    int isTokenBinOp(char c, FILE* fp);
    int Tokenize(const char *fileName);
};