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

    if (currentToken != ')' && currentToken != Token::Type::TOKEN_RIGHT_PARENTHESES)
    {
        return LogError("expected ')'");
    }
    getNextToken();
    return V;    
}

/// identifierexpr
///   ::= identifier
///   ::= identifier '(' expression* ')'
///   ::= identifier '.' identifier
std::unique_ptr<ExprAST> Parser::ParseIdentifierExpr() {
    std::string IdName = lex.IdentifierStr;

    getNextToken();  // eat identifier.

    // Handle member access: obj.field or obj.method()
    if (currentToken == Token::Type::TOKEN_DOT) {
        auto Object = std::make_unique<VariableExprAST>(IdName);
        return ParseMemberAccess(std::move(Object));
    }

    if (currentToken != '(' && currentToken != Token::Type::TOKEN_LEFT_PARENTHESES) {
        // Simple variable ref.
        return std::make_unique<VariableExprAST>(IdName);
    }

    // Call.
    getNextToken();  // eat (
    std::vector<std::unique_ptr<ExprAST>> Args;
    if (currentToken != ')' && currentToken != Token::Type::TOKEN_RIGHT_PARENTHESES) {
        while (true) {
            if (auto Arg = ParseExpression())
                Args.push_back(std::move(Arg));
            else
                return nullptr;

            if (currentToken == ')' || currentToken == Token::Type::TOKEN_RIGHT_PARENTHESES)
                break;

            if (currentToken != ',' && currentToken != Token::Type::TOKEN_COMMA)
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
///   ::= stringexpr
///   ::= parenexpr
///   ::= ifexpr
///   ::= returnexpr
///   ::= variabledecl
///   ::= obj expr
///   ::= null
std::unique_ptr<ExprAST> Parser::ParsePrimary() {
    switch (currentToken) {
    default: {
        std::string errorMsg = "unknown token '" + Token::getTokenName(currentToken) + 
                              "' when expecting an expression";
        return LogError(errorMsg.c_str());
    }
    case Token::Type::TOKEN_ID:
        return ParseIdentifierExpr();
    case Token::Type::TOKEN_NUM:
        return ParseNumberExpr();
    case Token::Type::TOKEN_STRING_LITERAL:
        return ParseStringExpr();
    case Token::Type::TOKEN_IF:
        return ParseIfExpr();
    case Token::Type::TOKEN_RETURN:
        return ParseReturnExpr();
    case Token::Type::TOKEN_DEF:
        return ParseVariableDecl();
    case Token::Type::TOKEN_OBJ:
        // Handle 'obj' keyword - access object member
        getNextToken(); // eat 'obj'
        if (currentToken != Token::Type::TOKEN_ID) {
            return LogError("expected identifier after 'obj'");
        }
        return ParseIdentifierExpr(); // Parse the identifier as a variable access
    case Token::Type::TOKEN_CONSOLE:
        getNextToken();
        return std::make_unique<VariableExprAST>("Console");
    case Token::Type::TOKEN_NULL:
        getNextToken();
        return std::make_unique<StringExprAST>(""); // null as empty string for now
    case '(':
    case Token::Type::TOKEN_LEFT_PARENTHESES:
        return ParseParenExpr();
    }
}

/// BinopPrecedence - This holds the precedence for each binary operator that is
/// defined.
std::map<char, int> BinopPrecedence;

/// GetTokPrecedence - Get the precedence of the pending binary operator token.
int Parser::GetTokPrecedence() {
    // Handle token types for operators
    char opChar = 0;
    switch (currentToken) {
        case Token::Type::TOKEN_PLUS:
            opChar = '+';
            break;
        case Token::Type::TOKEN_MINUS:
            opChar = '-';
            break;
        case Token::Type::TOKEN_TIMES:
            opChar = '*';
            break;
        case Token::Type::TOKEN_DIVIDE:
            opChar = '/';
            break;
        case Token::Type::TOKEN_LESSTHAN:
            opChar = '<';
            break;
        default:
            // For ASCII characters, use directly
            if (isascii(currentToken)) {
                opChar = static_cast<char>(currentToken);
            } else {
                return -1;
            }
            break;
    }
    
    // Make sure it's a declared binop.
    int TokPrec = BinopPrecedence[opChar];
    if (TokPrec <= 0) return -1;
    return TokPrec;
}

/// expression
///   ::= primary binoprhs
///
std::unique_ptr<ExprAST> Parser::ParseExpression() {
    auto LHS = ParsePrimary();
    if (!LHS) {
        return nullptr;
    }

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
    
    // Convert token to char for BinaryExprAST
    char Binop = 0;
    switch (currentToken) {
        case Token::Type::TOKEN_PLUS:
            Binop = '+';
            break;
        case Token::Type::TOKEN_MINUS:
            Binop = '-';
            break;
        case Token::Type::TOKEN_TIMES:
            Binop = '*';
            break;
        case Token::Type::TOKEN_DIVIDE:
            Binop = '/';
            break;
        case Token::Type::TOKEN_LESSTHAN:
            Binop = '<';
            break;
        default:
            if (isascii(currentToken)) {
                Binop = static_cast<char>(currentToken);
            } else {
                return LHS; // Not a binary operator
            }
            break;
    }
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


void Parser::HandleDefinition() {
    if (ParseDefinition()) {
        fprintf(stderr, "Parsed a function definition\n");
    } else {
        fprintf(stderr, "Failed to parse function definition (token: %s)\n",
                Token::getTokenName(currentToken).c_str());
        // Skip token for error recovery.
        getNextToken();
    }
    // Token already consumed by ParseDefinition
}

void Parser::HandleExtern() {
    if (ParseExtern()) {
        fprintf(stderr, "Parsed an extern declaration\n");
    } else {
        fprintf(stderr, "Failed to parse extern declaration (token: %s)\n",
                Token::getTokenName(currentToken).c_str());
        // Skip token for error recovery.
        getNextToken();
    }
    // Token already consumed by ParseExtern
}

void Parser::HandleTopLevelExpression() {
    // Evaluate a top-level expression into an anonymous function.
    if (ParseTopLevelExpr()) {
        fprintf(stderr, "Parsed a top-level expression\n");
    } else {
        fprintf(stderr, "Failed to parse top-level expression (token: %s)\n", 
                Token::getTokenName(currentToken).c_str());
        // Skip token for error recovery.
        getNextToken();
    }
    // Token already consumed by ParseTopLevelExpr
}

std::unique_ptr<ExprAST> Parser::ParseStringExpr() {
    auto Result = std::make_unique<StringExprAST>(lex.StringVal);
    getNextToken();
    return std::move(Result);
}

std::unique_ptr<ExprAST> Parser::ParseIfExpr() {
    getNextToken(); // eat 'if'
    
    if (currentToken != Token::Type::TOKEN_LEFT_PARENTHESES)
        return LogError("expected '(' after if");
    getNextToken(); // eat '('
    
    auto Cond = ParseExpression();
    if (!Cond)
        return nullptr;
    
    if (currentToken != Token::Type::TOKEN_RIGHT_PARENTHESES)
        return LogError("expected ')' after if condition");
    getNextToken(); // eat ')'
    
    if (currentToken != Token::Type::TOKEN_LEFT_CURLYBRAC)
        return LogError("expected '{' after if condition");
    getNextToken(); // eat '{'
    
    auto Then = ParseStatement();
    if (!Then)
        return nullptr;
    
    if (currentToken != Token::Type::TOKEN_RIGHT_CURLYBRAC)
        return LogError("expected '}' after if block");
    getNextToken(); // eat '}'
    
    std::unique_ptr<ExprAST> Else = nullptr;
    if (currentToken == Token::Type::TOKEN_ELSE) {
        getNextToken(); // eat 'else'
        if (currentToken != Token::Type::TOKEN_LEFT_CURLYBRAC)
            return LogError("expected '{' after else");
        getNextToken(); // eat '{'
        Else = ParseStatement();
        if (!Else)
            return nullptr;
        if (currentToken != Token::Type::TOKEN_RIGHT_CURLYBRAC)
            return LogError("expected '}' after else block");
        getNextToken(); // eat '}'
    }
    
    return std::make_unique<IfExprAST>(std::move(Cond), std::move(Then), std::move(Else));
}

std::unique_ptr<ExprAST> Parser::ParseReturnExpr() {
    getNextToken(); // eat 'return'
    
    if (currentToken == Token::Type::TOKEN_OBJ) {
        getNextToken(); // eat 'obj'
        if (currentToken != Token::Type::TOKEN_ID)
            return LogError("expected identifier after 'obj'");
        auto Value = ParseIdentifierExpr();
        if (!Value)
            return nullptr;
        return std::make_unique<ReturnExprAST>(std::move(Value));
    }
    
    auto Value = ParseExpression();
    if (!Value)
        return nullptr;
    return std::make_unique<ReturnExprAST>(std::move(Value));
}

std::unique_ptr<ExprAST> Parser::ParseVariableDecl() {
    getNextToken(); // eat 'def'
    
    std::string Type;
    if (currentToken == Token::Type::TOKEN_INT) {
        Type = "int";
        getNextToken(); // eat 'int'
    } else if (currentToken == Token::Type::TOKEN_STRING) {
        Type = "string";
        getNextToken(); // eat 'string'
    } else {
        return LogError("expected 'int' or 'string' after 'def'");
    }
    
    if (currentToken != Token::Type::TOKEN_ID)
        return LogError("expected identifier after type");
    std::string Name = lex.IdentifierStr;
    getNextToken(); // eat identifier
    
    std::unique_ptr<ExprAST> Init = nullptr;
    if (currentToken == '=') {
        getNextToken(); // eat '='
        Init = ParseExpression();
        if (!Init)
            return nullptr;
    }
    
    return std::make_unique<VariableDeclAST>(Name, Type, std::move(Init));
}

std::unique_ptr<ExprAST> Parser::ParseMemberAccess(std::unique_ptr<ExprAST> Object) {
    getNextToken(); // eat '.'
    
    if (currentToken != Token::Type::TOKEN_ID)
        return LogError("expected identifier after '.'");
    
    std::string Member = lex.IdentifierStr;
    getNextToken(); // eat identifier
    
    // Check if it's a method call
    if (currentToken == Token::Type::TOKEN_LEFT_PARENTHESES || currentToken == '(') {
        getNextToken(); // eat '('
        std::vector<std::unique_ptr<ExprAST>> Args;
        if (currentToken != Token::Type::TOKEN_RIGHT_PARENTHESES && currentToken != ')') {
            while (true) {
                if (auto Arg = ParseExpression())
                    Args.push_back(std::move(Arg));
                else
                    return nullptr;
                
                if (currentToken == Token::Type::TOKEN_RIGHT_PARENTHESES || currentToken == ')')
                    break;
                
                if (currentToken != Token::Type::TOKEN_COMMA && currentToken != ',')
                    return LogError("Expected ')' or ',' in argument list");
                getNextToken();
            }
        }
        getNextToken(); // eat ')'
        
        // Handle Console.print() specially
        if (auto VarExpr = dynamic_cast<VariableExprAST*>(Object.get())) {
            if (VarExpr->getName() == "Console" && Member == "print") {
                return std::make_unique<CallExprAST>("Console.print", std::move(Args));
            }
        }
        
        // Create a call expression with the member access
        std::string CallName = Member;
        return std::make_unique<CallExprAST>(CallName, std::move(Args));
    }
    
    auto MemberExpr = std::make_unique<MemberAccessExprAST>(std::move(Object), Member);
    return MemberExpr;
}

std::unique_ptr<ExprAST> Parser::ParseStatement() {
    // Handle variable declarations, return, if, expressions
    std::unique_ptr<ExprAST> Stmt = nullptr;
    
    if (currentToken == Token::Type::TOKEN_DEF) {
        Stmt = ParseVariableDecl();
    } else if (currentToken == Token::Type::TOKEN_RETURN) {
        Stmt = ParseReturnExpr();
    } else if (currentToken == Token::Type::TOKEN_IF) {
        Stmt = ParseIfExpr();
    } else {
        // Otherwise parse as expression
        Stmt = ParseExpression();
    }
    
    // Statements should end with semicolon (except if/else which have braces)
    if (Stmt && currentToken != Token::Type::TOKEN_RIGHT_CURLYBRAC && 
        currentToken != Token::Type::TOKEN_ELSE) {
        if (currentToken == Token::Type::TOKEN_SEMICOLON || currentToken == ';') {
            getNextToken(); // eat ';'
        }
    }
    
    return Stmt;
}

std::unique_ptr<ActionAST> Parser::ParseAction() {
    getNextToken(); // eat 'action'
    
    if (currentToken != Token::Type::TOKEN_ID) {
        return nullptr;
    }
    
    std::string Name = lex.IdentifierStr;
    getNextToken(); // eat action name
    
    if (currentToken != Token::Type::TOKEN_LEFT_PARENTHESES) {
        return nullptr;
    }
    getNextToken(); // eat '('
    
    std::vector<std::pair<std::string, std::string>> Params;
    while (currentToken != Token::Type::TOKEN_RIGHT_PARENTHESES) {
        std::string ParamType;
        if (currentToken == Token::Type::TOKEN_STRING) {
            ParamType = "string";
            getNextToken(); // eat 'string'
        } else if (currentToken == Token::Type::TOKEN_INT) {
            ParamType = "int";
            getNextToken(); // eat 'int'
        } else if (currentToken == Token::Type::TOKEN_DEF) {
            getNextToken(); // eat 'def'
            if (currentToken == Token::Type::TOKEN_INT) {
                ParamType = "int";
                getNextToken(); // eat 'int'
            } else if (currentToken == Token::Type::TOKEN_STRING) {
                ParamType = "string";
                getNextToken(); // eat 'string'
            } else {
                return nullptr;
            }
        } else {
            return nullptr;
        }
        
        if (currentToken != Token::Type::TOKEN_ID) {
            return nullptr;
        }
        std::string ParamName = lex.IdentifierStr;
        getNextToken(); // eat param name
        
        Params.push_back({ParamName, ParamType});
        
        if (currentToken == Token::Type::TOKEN_COMMA) {
            getNextToken(); // eat ','
        } else if (currentToken != Token::Type::TOKEN_RIGHT_PARENTHESES) {
            return nullptr;
        }
    }
    getNextToken(); // eat ')'
    
    if (currentToken != Token::Type::TOKEN_LEFT_CURLYBRAC) {
        return nullptr;
    }
    getNextToken(); // eat '{'
    
    // Parse body as a sequence of statements until closing brace
    std::vector<std::unique_ptr<ExprAST>> Statements;
    
    while (currentToken != Token::Type::TOKEN_RIGHT_CURLYBRAC && 
           currentToken != Token::Type::TOKEN_EOF) {
        auto Stmt = ParseStatement();
        if (!Stmt) {
            // If parsing fails, try to recover by skipping to next statement
            if (currentToken == Token::Type::TOKEN_SEMICOLON || currentToken == ';') {
                getNextToken(); // eat ';'
            } else if (currentToken != Token::Type::TOKEN_RIGHT_CURLYBRAC) {
                getNextToken(); // skip one token
            }
            continue;
        }
        Statements.push_back(std::move(Stmt));
    }
    
    if (currentToken != Token::Type::TOKEN_RIGHT_CURLYBRAC) {
        return nullptr;
    }
    getNextToken(); // eat '}'
    
    // Create a BlockAST to hold all statements
    std::unique_ptr<ExprAST> Body;
    if (Statements.empty()) {
        // Empty body - create a null expression
        Body = std::make_unique<StringExprAST>("");
    } else if (Statements.size() == 1) {
        // Single statement - use it directly
        Body = std::move(Statements[0]);
    } else {
        // Multiple statements - create a BlockAST
        Body = std::make_unique<BlockAST>(std::move(Statements));
    }
    
    return std::make_unique<ActionAST>(Name, std::move(Params), std::move(Body));
}

std::unique_ptr<BehaviorAST> Parser::ParseBehavior() {
    getNextToken(); // eat 'behavior'
    
    if (currentToken != Token::Type::TOKEN_ID) {
        return nullptr;
    }
    
    std::string Name = lex.IdentifierStr;
    getNextToken(); // eat behavior name
    std::cout << "Behavior Name: " + Name << std::endl;
    
    if (currentToken != Token::Type::TOKEN_LEFT_CURLYBRAC) {
        return nullptr;
    }
    getNextToken(); // eat '{'
    
    std::vector<std::unique_ptr<ActionAST>> Actions;
    while (currentToken == Token::Type::TOKEN_ACTION) {
        if (auto Action = ParseAction()) {
            Actions.push_back(std::move(Action));
        } else {
            return nullptr;
        }
    }
    
    if (currentToken != Token::Type::TOKEN_RIGHT_CURLYBRAC) {
        return nullptr;
    }
    getNextToken(); // eat '}'
    
    return std::make_unique<BehaviorAST>(Name, std::move(Actions));
}

std::unique_ptr<ObjectAST> Parser::ParseObject() {
    getNextToken(); // eat 'object'
    
    if (currentToken != Token::Type::TOKEN_ID)
        return nullptr;
    
    std::string Name = lex.IdentifierStr;
    getNextToken(); // eat object name
    
    std::vector<std::string> InheritedBehaviors;
    if (currentToken == Token::Type::TOKEN_INHERITS) {
        getNextToken(); // eat 'inherits'
        
        while (currentToken == Token::Type::TOKEN_ID) {
            InheritedBehaviors.push_back(lex.IdentifierStr);
            getNextToken(); // eat behavior name
            
            if (currentToken == Token::Type::TOKEN_COMMA) {
                getNextToken(); // eat ','
            } else {
                break;
            }
        }
    }
    
    // Semicolon is optional for object definitions
    if (currentToken == Token::Type::TOKEN_SEMICOLON || currentToken == ';') {
        getNextToken(); // eat ';'
    }
    
    return std::make_unique<ObjectAST>(Name, std::move(InheritedBehaviors));
}

void Parser::HandleBehavior() {
    if (auto Behavior = ParseBehavior()) {
        Behavior->codegen();
        fprintf(stderr, "Parsed a behavior definition: %s\n", Behavior->getName().c_str());
    } else {
        fprintf(stderr, "Failed to parse behavior definition (token: %s)\n",
                Token::getTokenName(currentToken).c_str());
        getNextToken();
    }
    // Token already consumed by ParseBehavior
}

void Parser::HandleObject() {
    if (auto Object = ParseObject()) {
        Object->codegen();
        fprintf(stderr, "Parsed an object definition\n");
    } else {
        fprintf(stderr, "Failed to parse object definition (token: %s)\n",
                Token::getTokenName(currentToken).c_str());
        getNextToken();
    }
    // Token already consumed by ParseObject
}