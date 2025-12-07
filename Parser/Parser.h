#pragma once
#include "../Ast/Ast.h"
#include "../Lexer/Lexer.h"
#include "../Lexer/Tokens/Token.h"
#include <iostream>
#include <map>


class Parser
{
private:
    Lexer& lex;
    Token& tok;
    std::string tokenInstance;
public:
    int currentToken;
    
    Parser(Lexer& lexer, Token& token)
        : lex(lexer), tok(token), currentToken(0) {}
    
    int getNextToken() {
        currentToken = lex.getToken();
        tokenInstance = tok.getTokenName(currentToken);
        std::cerr << "Fetched Token: " << tokenInstance
                  << " (" << currentToken << ")" << std::endl;
        return currentToken;
    }

    std::unique_ptr<ExprAST> LogError(const char *Str);
    std::unique_ptr<PrototypeAST> LogErrorP(const char *Str);

    std::unique_ptr<ExprAST> ParseNumberExpr();
    std::unique_ptr<ExprAST> ParseStringExpr();
    std::unique_ptr<ExprAST> ParseParenExpr();
    std::unique_ptr<ExprAST> ParseIdentifierExpr();
    std::unique_ptr<ExprAST> ParsePrimary();
    std::unique_ptr<ExprAST> ParseExpression();
    std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec,
                                              std::unique_ptr<ExprAST> LHS);
    std::unique_ptr<ExprAST> ParseIfExpr();
    std::unique_ptr<ExprAST> ParseReturnExpr();
    std::unique_ptr<ExprAST> ParseVariableDecl();
    std::unique_ptr<ExprAST> ParseMemberAccess(std::unique_ptr<ExprAST> Object);
    std::unique_ptr<ExprAST> ParseStatement();

    std::unique_ptr<PrototypeAST> ParsePrototype();
    std::unique_ptr<FunctionAST> ParseDefinition();
    std::unique_ptr<PrototypeAST> ParseExtern();
    std::unique_ptr<FunctionAST> ParseTopLevelExpr();
    
    std::unique_ptr<ActionAST> ParseAction();
    std::unique_ptr<BehaviorAST> ParseBehavior();
    std::unique_ptr<ObjectAST> ParseObject();
    
    std::map<char, int> BinopPrecedence;

    int GetTokPrecedence();
    void HandleDefinition();
    void HandleExtern();
    void HandleTopLevelExpression();
    void HandleBehavior();
    void HandleObject();
};
