#pragma once
#include <memory>
#include "../../Ast/Ast.h"

class Helper
{
private:
    /* data */
public:
    bool is_digit(char c) noexcept;
    bool is_identifier(char c) noexcept;
    
    std::unique_ptr<ExprAST> LogError(const char *Str);
    std::unique_ptr<PrototypeAST> LogErrorP(const char *Str);
};