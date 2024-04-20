#include "Token.h"
#include <optional>
#include <fstream>


class Lexer
{
private:
public:    
    std::optional<TokenType::Token> isTokenIdentifier(std::ifstream& stream);
    int isTokenNumber(char c, FILE* fp);
    int isTokenBinOp(char c, FILE* fp);
    int Tokenize(const char *fileName);
};