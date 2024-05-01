#pragma once
#include <string>

class Lexer
{
private:
public:
    std::string IdentifierStr;
    double NumVal;

    int getToken();
    
    std::string splitFile(const char* file);
};