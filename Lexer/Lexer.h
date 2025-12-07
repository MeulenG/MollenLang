#pragma once
#include <string>
#include <fstream>

class Lexer
{
private:
    std::ifstream inputFile;
    int LastChar;
    bool usingFile;

public:
    std::string IdentifierStr;
    std::string StringVal;  // For string literals
    double NumVal;

    Lexer() : LastChar(' '), usingFile(false) {}
    ~Lexer() { if (inputFile.is_open()) inputFile.close(); }
    
    bool setInputFile(const std::string& filename);
    int getChar();
    int getToken();
    
    std::string splitFile(const char* file);
};