#include "Parser.h"
#include "../Ast/Ast.h"
#include "../CodeGen/Codegen.h"
#include "../Lexer/Lexer.h"
#include "../Lexer/Tokens/Token.h"
#include "../Lexer/Helper/Helper.h"
#include <optional>
#include <fstream>
#include <cstring>
#include <map>
#include <unordered_map>
#include <vector>
#include <iostream>

using namespace llvm;


/// LogError* - These are little helper functions for error handling.
std::unique_ptr<ExprAST> Parser::LogError(const char *Str) {
	fprintf(stderr, "Error: %s\n", Str);
	return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::LogErrorP(const char *Str) {
	LogError(Str);
	return nullptr;
}


std::unique_ptr<ExprAST> Parser::ParseNumberExpr() {
    auto Result = std::make_unique<NumberExprAST>(lex.NumVal);
    getNextToken();
    return std::move(Result);
}

std::unique_ptr<ExprAST> Parser::ParseParenExpr() {
    getNextToken();
    auto V = ParseExpression();
    if (!V)
    {
        return nullptr;
    }

    if (currentToken != ')')
    {
        return LogError("expected ')'");
    }
    getNextToken();
    return V;    
}

/// identifierexpr
///   ::= identifier
///   ::= identifier '(' expression* ')'
std::unique_ptr<ExprAST> Parser::ParseIdentifierExpr() {
    std::string IdName = lex.IdentifierStr;

    getNextToken();  // eat identifier.

    if (currentToken != '(') // Simple variable ref.
        return std::make_unique<VariableExprAST>(IdName);

    // Call.
    getNextToken();  // eat (
    std::vector<std::unique_ptr<ExprAST>> Args;
    if (currentToken != ')') {
        while (true) {
            if (auto Arg = ParseExpression())
                Args.push_back(std::move(Arg));
            else
                return nullptr;

            if (currentToken == ')')
                break;

            if (currentToken != ',')
                return LogError("Expected ')' or ',' in argument list");
            getNextToken();
        }
    }

    // Eat the ')'.
    getNextToken();

    return std::make_unique<CallExprAST>(IdName, std::move(Args));
}

/// primary
///   ::= identifierexpr
///   ::= numberexpr
///   ::= parenexpr
std::unique_ptr<ExprAST> Parser::ParsePrimary() {
    switch (currentToken) {
    default:
        return LogError("unknown token when expecting an expression");
    case Token::Type::TOKEN_ID:
        return ParseIdentifierExpr();
    case Token::Type::TOKEN_NUM:
        return ParseNumberExpr();
    case '(':
        return ParseParenExpr();
    }
}

/// BinopPrecedence - This holds the precedence for each binary operator that is
/// defined.
std::map<char, int> BinopPrecedence;

/// GetTokPrecedence - Get the precedence of the pending binary operator token.
int Parser::GetTokPrecedence() {
    if (!isascii(currentToken))
        return -1;

    // Make sure it's a declared binop.
    int TokPrec = BinopPrecedence[currentToken];
    if (TokPrec <= 0) return -1;
    return TokPrec;
}

/// expression
///   ::= primary binoprhs
///
std::unique_ptr<ExprAST> Parser::ParseExpression() {
    auto LHS = ParsePrimary();
    if (!LHS)
        return nullptr;

    return ParseBinOpRHS(0, std::move(LHS));
}

/// binoprhs
///   ::= ('+' primary)*
std::unique_ptr<ExprAST> Parser::ParseBinOpRHS(int ExprPrec,
                                              std::unique_ptr<ExprAST> LHS) {
  // If this is a binop, find its precedence.
  while (true) {
    int TokPrec = GetTokPrecedence();

    // If this is a binop that binds at least as tightly as the current binop,
    // consume it, otherwise we are done.
    if (TokPrec < ExprPrec)
      return LHS;
    
    int Binop = currentToken;
    getNextToken();

    auto RHS = ParsePrimary();
    if (!RHS)
    {
        return nullptr;
    }

    int NextPrec = GetTokPrecedence();
    if (TokPrec < NextPrec)
    {
        RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
        if (!RHS)
        {
            return nullptr;
        }
    }
    // Merge LHS/RHS
    LHS = std::make_unique<BinaryExprAST>(Binop, std::move(LHS), 
                                                std::move(RHS));
  }
}

/// prototype
///   ::= id '(' id* ')'
std::unique_ptr<PrototypeAST> Parser::ParsePrototype() {
    if (currentToken != Token::Type::TOKEN_ID) {
        return LogErrorP("Expected function name in prototype");
    }
    std::string FnName = lex.IdentifierStr;
    std::cerr << "After Setting FnName: " << currentToken << std::endl;
    getNextToken();
    std::cerr << "After getting next token: " << currentToken << std::endl;
    if (currentToken != '(') {
        return LogErrorP("Expected '(' in prototype");
    }

    // Read the list of argument names.
    std::vector<std::string> ArgNames;
    while (getNextToken() == Token::Type::TOKEN_ID)
        ArgNames.push_back(lex.IdentifierStr);
    if (currentToken != ')')
        return LogErrorP("Expected ')' in prototype");

    // success.
    getNextToken();  // eat ')'.

    return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}

/// definition ::= 'def' prototype expression
std::unique_ptr<FunctionAST> Parser::ParseDefinition() {
    getNextToken();  // eat def.
    auto Proto = ParsePrototype();
    if (!Proto) return nullptr;

    if (auto E = ParseExpression())
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    return nullptr;
}

/// external ::= 'extern' prototype
std::unique_ptr<PrototypeAST> Parser::ParseExtern() {
    getNextToken();  // eat extern.
    return ParsePrototype();
}

/// toplevelexpr ::= expression
std::unique_ptr<FunctionAST> Parser::ParseTopLevelExpr() {
    if (auto E = ParseExpression()) {
        // Make an anonymous proto.
        auto Proto = std::make_unique<PrototypeAST>("__anon_expr",
                                                    std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    }
    return nullptr;
}

void InitializeModule() {
    // Open a new context and module.
    TheContext = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>("my cool jit", *TheContext);

    // Create a new builder for the module.
    Builder = std::make_unique<IRBuilder<>>(*TheContext);
}

void Parser::HandleDefinition() {
    if (ParseDefinition()) {
        fprintf(stderr, "Parsed a function definition.\n");
    } else {
        // Skip token for error recovery.
        getNextToken();
    }
}

void Parser::HandleExtern() {
    if (ParseExtern()) {
        fprintf(stderr, "Parsed an extern\n");
    } else {
        // Skip token for error recovery.
        getNextToken();
    }
}

void Parser::HandleTopLevelExpression() {
    // Evaluate a top-level expression into an anonymous function.
    if (ParseTopLevelExpr()) {
        fprintf(stderr, "Parsed a top-level expr\n");
    } else {
        // Skip token for error recovery.
        getNextToken();
    }
}