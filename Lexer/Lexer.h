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
    int getFileSize(int fd_in);
    int openFileDescriptor(const char *file, const char *file2);
    std::vector<char> readAndRemoveWS(int fd_in, off_t fileSize);
    void copyFile(const char* file, const char* file_out, const std::vector<char>& data);
    
    // Lets make this shit readable
    bool is_digit(char c) noexcept;
    bool is_identifier(char c) noexcept;
    int Tokenize(const char *fileName);
};