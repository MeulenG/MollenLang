#pragma once
#include <string>
#include <memory>
#include <vector>

// Forward declarations
namespace llvm {
	class Value;
	class Function;
}

/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:
    virtual ~ExprAST() = default;

    virtual llvm::Value *codegen() = 0;
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
    double Val;

public:
    NumberExprAST(double Val) : Val(Val) {}

    llvm::Value *codegen() override;
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
    std::string Name;

public:
    VariableExprAST(const std::string &Name) : Name(Name) {}

    llvm::Value *codegen() override;
    const std::string &getName() const { return Name; }
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
    BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
                    std::unique_ptr<ExprAST> RHS)
        : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

    llvm::Value *codegen() override;
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

public:
    CallExprAST(const std::string &Callee,
                std::vector<std::unique_ptr<ExprAST>> Args)
        : Callee(Callee), Args(std::move(Args)) {}

    llvm::Value *codegen() override;
};

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;

public:
    PrototypeAST(const std::string &Name, std::vector<std::string> Args)
        : Name(Name), Args(std::move(Args)) {}

    llvm::Function *codegen();
    const std::string &getName() const { return Name; }
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;

public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::unique_ptr<ExprAST> Body)
        : Proto(std::move(Proto)), Body(std::move(Body)) {}

    llvm::Function *codegen();
};

/// StringExprAST - Expression class for string literals.
class StringExprAST : public ExprAST {
    std::string Val;

public:
    StringExprAST(const std::string &Val) : Val(Val) {}

    llvm::Value *codegen() override;
};

/// MemberAccessExprAST - Expression for obj.field or object.method()
class MemberAccessExprAST : public ExprAST {
    std::unique_ptr<ExprAST> Object;
    std::string Member;

public:
    MemberAccessExprAST(std::unique_ptr<ExprAST> Object, const std::string &Member)
        : Object(std::move(Object)), Member(Member) {}

    llvm::Value *codegen() override;
};

/// IfExprAST - Expression for if/else statements.
class IfExprAST : public ExprAST {
    std::unique_ptr<ExprAST> Cond;
    std::unique_ptr<ExprAST> Then;
    std::unique_ptr<ExprAST> Else;

public:
    IfExprAST(std::unique_ptr<ExprAST> Cond,
              std::unique_ptr<ExprAST> Then,
              std::unique_ptr<ExprAST> Else)
        : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}

    llvm::Value *codegen() override;
};

/// ReturnExprAST - Expression for return statements.
class ReturnExprAST : public ExprAST {
    std::unique_ptr<ExprAST> Value;

public:
    ReturnExprAST(std::unique_ptr<ExprAST> Value) : Value(std::move(Value)) {}

    llvm::Value *codegen() override;
};

/// VariableDeclAST - Declaration for def int/string variables.
class VariableDeclAST : public ExprAST {
    std::string Name;
    std::string Type;  // "int" or "string"
    std::unique_ptr<ExprAST> Init;

public:
    VariableDeclAST(const std::string &Name, const std::string &Type,
                    std::unique_ptr<ExprAST> Init)
        : Name(Name), Type(Type), Init(std::move(Init)) {}

    llvm::Value *codegen() override;
};

/// BlockAST - Represents a sequence of statements.
class BlockAST : public ExprAST {
    std::vector<std::unique_ptr<ExprAST>> Statements;

public:
    BlockAST(std::vector<std::unique_ptr<ExprAST>> Statements)
        : Statements(std::move(Statements)) {}

    llvm::Value *codegen() override;
};

/// ActionAST - Represents an action (method) within a behavior.
class ActionAST {
    std::string Name;
    std::vector<std::pair<std::string, std::string>> Params;  // (name, type)
    std::unique_ptr<ExprAST> Body;

public:
    ActionAST(const std::string &Name,
              std::vector<std::pair<std::string, std::string>> Params,
              std::unique_ptr<ExprAST> Body)
        : Name(Name), Params(std::move(Params)), Body(std::move(Body)) {}

    llvm::Function *codegen(const std::string &BehaviorName);
    const std::string &getName() const { return Name; }
};

/// BehaviorAST - Represents a behavior definition.
class BehaviorAST {
    std::string Name;
    std::vector<std::unique_ptr<ActionAST>> Actions;

public:
    BehaviorAST(const std::string &Name,
                std::vector<std::unique_ptr<ActionAST>> Actions)
        : Name(Name), Actions(std::move(Actions)) {}

    void codegen();
    const std::string &getName() const { return Name; }
};

/// ObjectAST - Represents an object declaration with inheritance.
class ObjectAST {
    std::string Name;
    std::vector<std::string> InheritedBehaviors;

public:
    ObjectAST(const std::string &Name,
              std::vector<std::string> InheritedBehaviors)
        : Name(Name), InheritedBehaviors(std::move(InheritedBehaviors)) {}

    void codegen();
};

