#pragma once
#include "../Ast/Ast.h"
#include "../Lexer/Lexer.h"
#include <iostream>
#include <map>


class Parser
{
private:
    Lexer lex;
public:
    int currentToken;
    
    int getNextToken() {
        currentToken = lex.getToken();
        std::cerr << "Fetched Token: " << currentToken << std::endl;
        return currentToken;
    }

    std::unique_ptr<ExprAST> LogError(const char *Str);
    std::unique_ptr<PrototypeAST> LogErrorP(const char *Str);

    std::unique_ptr<ExprAST> ParseNumberExpr();
    std::unique_ptr<ExprAST> ParseParenExpr();
    std::unique_ptr<ExprAST> ParseIdentifierExpr();
    std::unique_ptr<ExprAST> ParsePrimary();
    std::unique_ptr<ExprAST> ParseExpression();
    std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec,
                                              std::unique_ptr<ExprAST> LHS);

    std::unique_ptr<PrototypeAST> ParsePrototype();
    std::unique_ptr<FunctionAST> ParseDefinition();
    std::unique_ptr<PrototypeAST> ParseExtern();
    std::unique_ptr<FunctionAST> ParseTopLevelExpr();
    
    std::map<char, int> BinopPrecedence;

    int GetTokPrecedence();
    void InitializeModule();
    void HandleDefinition();
    void HandleExtern();
    void HandleTopLevelExpression();
};
