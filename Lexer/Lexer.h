#include "Token.h"


class Lexer
{
private:
public:    
    int isTokenIdentifier(char c, FILE* fp, std::string sb);
    int isTokenNumber(char c, FILE* fp);
    int isTokenBinOp(char c, FILE* fp);
    int Tokenize(const char *fileName);
};