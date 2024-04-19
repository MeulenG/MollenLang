#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Token.h"
#include <vector>


class Scanner
{
private:
    /* data */
public:
    int lines = 0;

    int getFileSize(int fd_in);
    int openFileDescriptor(const char *file, const char *file2);
    std::vector<char> readAndRemoveWS(int fd_in, off_t fileSize);
    void copyFile(const char* file_out, const std::vector<char>& data);
};
