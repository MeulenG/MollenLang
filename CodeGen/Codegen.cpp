#include "Codegen.h"
#include <cstdio>
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
using llvm::Value;
// Define static members
std::unique_ptr<LLVMContext> Codegen::TheContext;
std::unique_ptr<Module> Codegen::TheModule;
std::unique_ptr<IRBuilder<>> Codegen::Builder;
std::map<std::string, Value *> Codegen::NamedValues;

Value *LogErrorV(const char *Str) {
	fprintf(stderr, "Error: %s\n", Str);
	return nullptr;
}

void Codegen::InitializeModule() {
	// Open a new context and module.
	TheContext = std::make_unique<LLVMContext>();
	TheModule = std::make_unique<Module>("my cool jit", *TheContext);

	// Create a new builder for the module.
	Builder = std::make_unique<IRBuilder<>>(*TheContext);
}

Value *NumberExprAST::codegen() {
	return ConstantFP::get(*Codegen::TheContext, APFloat(Val));
}

Value *VariableExprAST::codegen() {
	// Look this variable up in the function.
	Value *V = Codegen::NamedValues[Name];
	if (!V) {
		fprintf(stderr, "Error: Unknown variable name '%s'\n", Name.c_str());
		return LogErrorV("Unknown variable name");
	}
	
	// If it's an alloca (variable declaration), load from it
	if (isa<AllocaInst>(V)) {
		return Codegen::Builder->CreateLoad(V->getType()->getPointerElementType(), V, Name.c_str());
	}
	
	return V;
}

Value *BinaryExprAST::codegen() {
	Value *L = LHS->codegen();
	Value *R = RHS->codegen();
	if (!L || !R)
		return nullptr;

	// Check if we're doing string concatenation
	bool IsStringOp = (L->getType()->isPointerTy() && R->getType()->isPointerTy());
	
	switch (Op) {
		case '+':
			if (IsStringOp) {
				// String concatenation - for now, just return first string
				// TODO: Implement proper string concatenation
				return L;
			}
			return Codegen::Builder->CreateFAdd(L, R, "addtmp");
		case '-':
			return Codegen::Builder->CreateFSub(L, R, "subtmp");
		case '*':
			return Codegen::Builder->CreateFMul(L, R, "multmp");
		case '/':
			return Codegen::Builder->CreateFDiv(L, R, "divtmp");
		case '<':
			L = Codegen::Builder->CreateFCmpULT(L, R, "cmptmp");
			// Convert bool 0/1 to double 0.0 or 1.0
			return Codegen::Builder->CreateUIToFP(L, Type::getDoubleTy(*Codegen::TheContext), "booltmp");
		default:
			return LogErrorV("invalid binary operator");
	}
}

Value *CallExprAST::codegen() {
	// Handle Console.print() specially
	if (Callee == "print" || Callee == "Console.print") {
		// Create printf declaration if it doesn't exist
		Function *PrintfF = Codegen::TheModule->getFunction("printf");
		if (!PrintfF) {
			std::vector<Type *> PrintfArgs;
			PrintfArgs.push_back(Type::getInt8PtrTy(*Codegen::TheContext));
			FunctionType *FT = FunctionType::get(Type::getInt32Ty(*Codegen::TheContext), PrintfArgs, true);
			PrintfF = Function::Create(FT, Function::ExternalLinkage, "printf", Codegen::TheModule.get());
		}
		
		std::vector<Value *> ArgsV;
		for (unsigned i = 0, e = Args.size(); i != e; ++i) {
			ArgsV.push_back(Args[i]->codegen());
			if (!ArgsV.back())
				return nullptr;
		}
		
		// For now, just call printf with first argument
		if (!ArgsV.empty()) {
			return Codegen::Builder->CreateCall(PrintfF, ArgsV, "printftmp");
		}
		return ConstantInt::get(Type::getInt32Ty(*Codegen::TheContext), 0);
	}
	
	// Look up the name in the global module table.
	Function *CalleeF = Codegen::TheModule->getFunction(Callee);
	if (!CalleeF)
		return LogErrorV("Unknown function referenced");

	// If argument mismatch error.
	if (CalleeF->arg_size() != Args.size())
		return LogErrorV("Incorrect # arguments passed");

	std::vector<Value *> ArgsV;
	for (unsigned i = 0, e = Args.size(); i != e; ++i) {
		ArgsV.push_back(Args[i]->codegen());
		if (!ArgsV.back())
		return nullptr;
	}

	return Codegen::Builder->CreateCall(CalleeF, ArgsV, "calltmp");
}

Function *PrototypeAST::codegen() {
	// Make the function type:  double(double,double) etc.
	std::vector<Type *> Doubles(Args.size(), Type::getDoubleTy(*Codegen::TheContext));
	FunctionType *FT =
		FunctionType::get(Type::getDoubleTy(*Codegen::TheContext), Doubles, false);

	Function *F =
		Function::Create(FT, Function::ExternalLinkage, Name, Codegen::TheModule.get());

	// Set names for all arguments.
	unsigned Idx = 0;
	for (auto &Arg : F->args())
		Arg.setName(Args[Idx++]);

	return F;
}

Function *FunctionAST::codegen() {
	// First, check for an existing function from a previous 'extern' declaration.
	Function *TheFunction = Codegen::TheModule->getFunction(Proto->getName());

	if (!TheFunction)
		TheFunction = Proto->codegen();

	if (!TheFunction)
		return nullptr;

	// Create a new basic block to start insertion into.
	BasicBlock *BB = BasicBlock::Create(*Codegen::TheContext, "entry", TheFunction);
	Codegen::Builder->SetInsertPoint(BB);

	// Record the function arguments in the NamedValues map.
	Codegen::NamedValues.clear();
	for (auto &Arg : TheFunction->args())
		Codegen::NamedValues[std::string(Arg.getName())] = &Arg;

	if (Value *RetVal = Body->codegen()) {
		// Finish off the function.
		Codegen::Builder->CreateRet(RetVal);

		// Validate the generated code, checking for consistency.
		verifyFunction(*TheFunction);

		return TheFunction;
	}

	// Error reading body, remove function.
	TheFunction->eraseFromParent();
	return nullptr;
}

Value *StringExprAST::codegen() {
	// Create a global string constant
	Constant *StrConstant = ConstantDataArray::getString(*Codegen::TheContext, Val);
	GlobalVariable *GV = new GlobalVariable(
		*Codegen::TheModule, StrConstant->getType(), true,
		GlobalValue::PrivateLinkage, StrConstant, "str");
	
	// Get pointer to the string
	Constant *Zero = Constant::getNullValue(IntegerType::getInt32Ty(*Codegen::TheContext));
	Constant *Indices[] = {Zero, Zero};
	Constant *StrPtr = ConstantExpr::getGetElementPtr(GV->getValueType(), GV, Indices);
	
	return StrPtr;
}

Value *MemberAccessExprAST::codegen() {
	// For now, just return the member name as a string
	// TODO: this would look up the member in the object
	Value *Obj = Object->codegen();
	if (!Obj)
		return nullptr;
	
	// TODO: Implement proper member access
	// For now, create a function call with the member name
	return LogErrorV("Member access not fully implemented");
}

Value *IfExprAST::codegen() {
	Value *CondV = Cond->codegen();
	if (!CondV)
		return nullptr;
	
	// Convert condition to a bool by comparing non-equal to 0.0
	CondV = Codegen::Builder->CreateFCmpONE(
		CondV, ConstantFP::get(*Codegen::TheContext, APFloat(0.0)), "ifcond");
	
	Function *TheFunction = Codegen::Builder->GetInsertBlock()->getParent();
	
	// Create blocks for the then and else cases. Insert the 'then' block at the end of the function.
	BasicBlock *ThenBB = BasicBlock::Create(*Codegen::TheContext, "then", TheFunction);
	BasicBlock *ElseBB = BasicBlock::Create(*Codegen::TheContext, "else");
	BasicBlock *MergeBB = BasicBlock::Create(*Codegen::TheContext, "ifcont");
	
	Codegen::Builder->CreateCondBr(CondV, ThenBB, ElseBB);
	
	// Emit then value.
	Codegen::Builder->SetInsertPoint(ThenBB);
	Value *ThenV = Then->codegen();
	if (!ThenV)
		return nullptr;
	
	Codegen::Builder->CreateBr(MergeBB);
	ThenBB = Codegen::Builder->GetInsertBlock();
	
	// Emit else block.
	TheFunction->getBasicBlockList().push_back(ElseBB);
	Codegen::Builder->SetInsertPoint(ElseBB);
	
	Value *ElseV = nullptr;
	if (Else) {
		ElseV = Else->codegen();
		if (!ElseV)
			return nullptr;
	} else {
		ElseV = ConstantFP::get(*Codegen::TheContext, APFloat(0.0));
	}
	
	Codegen::Builder->CreateBr(MergeBB);
	ElseBB = Codegen::Builder->GetInsertBlock();
	
	// Emit merge block.
	TheFunction->getBasicBlockList().push_back(MergeBB);
	Codegen::Builder->SetInsertPoint(MergeBB);
	PHINode *PN = Codegen::Builder->CreatePHI(Type::getDoubleTy(*Codegen::TheContext), 2, "iftmp");
	
	PN->addIncoming(ThenV, ThenBB);
	PN->addIncoming(ElseV, ElseBB);
	return PN;
}

llvm::Value *ReturnExprAST::codegen() {
	if (Value) {
		if (llvm::Value *RetVal = Value->codegen()) {
			Codegen::Builder->CreateRet(RetVal);
			return RetVal;
		}
	} else {
		// Return void (null return)
		Codegen::Builder->CreateRet(ConstantFP::get(*Codegen::TheContext, APFloat(0.0)));
		return nullptr;
	}
	return nullptr;
}

Value *VariableDeclAST::codegen() {
	// Allocate variable on stack
	AllocaInst *Alloca = nullptr;
	
	if (Type == "int") {
		Alloca = Codegen::Builder->CreateAlloca(Type::getDoubleTy(*Codegen::TheContext), nullptr, Name);
	} else if (Type == "string") {
		Alloca = Codegen::Builder->CreateAlloca(Type::getInt8PtrTy(*Codegen::TheContext), nullptr, Name);
	} else {
		return LogErrorV("Unknown variable type");
	}
	
	// Store initial value if provided
	if (Init) {
		Value *InitVal = Init->codegen();
		if (!InitVal)
			return nullptr;
		Codegen::Builder->CreateStore(InitVal, Alloca);
	}
	
	// Register variable in symbol table
	Codegen::NamedValues[Name] = Alloca;
	
	return Alloca;
}

Value *BlockAST::codegen() {
	// Generate code for all statements sequentially
	Value *LastValue = nullptr;
	for (auto &Stmt : Statements) {
		LastValue = Stmt->codegen();
		if (!LastValue) {
			// Check if this is a return statement (which may return void)
			// For now, continue even if codegen returns nullptr
			// Return statements handle their own return values
		}
	}
	// Return the last value (or nullptr if no statements or last was void return)
	return LastValue;
}

Function *ActionAST::codegen(const std::string &BehaviorName) {
	// Create function name: BehaviorName_ActionName
	std::string FuncName = BehaviorName + "_" + Name;
	
	// Check if function already exists
	Function *TheFunction = Codegen::TheModule->getFunction(FuncName);
	if (TheFunction)
		return TheFunction;
	
	// Build function type
	std::vector<Type *> ArgTypes;
	for (const auto &Param : Params) {
		if (Param.second == "int") {
			ArgTypes.push_back(Type::getDoubleTy(*Codegen::TheContext));
		} else if (Param.second == "string") {
			ArgTypes.push_back(Type::getInt8PtrTy(*Codegen::TheContext));
		}
	}
	
	Type *RetType = Type::getDoubleTy(*Codegen::TheContext); // Default return type
	FunctionType *FT = FunctionType::get(RetType, ArgTypes, false);
	
	Function *F = Function::Create(FT, Function::ExternalLinkage, FuncName, Codegen::TheModule.get());
	
	// Set argument names
	unsigned Idx = 0;
	for (auto &Arg : F->args()) {
		Arg.setName(Params[Idx++].first);
	}
	
	// Create entry block
	BasicBlock *BB = BasicBlock::Create(*Codegen::TheContext, "entry", F);
	Codegen::Builder->SetInsertPoint(BB);
	
	// Register parameters in symbol table
	Codegen::NamedValues.clear();
	for (auto &Arg : F->args()) {
		AllocaInst *Alloca = Codegen::Builder->CreateAlloca(Arg.getType(), nullptr, Arg.getName());
		Codegen::Builder->CreateStore(&Arg, Alloca);
		Codegen::NamedValues[std::string(Arg.getName())] = Alloca;
	}
	
	// Generate body
	Value *RetVal = Body->codegen();
	// If body is a BlockAST or ReturnExprAST, it may have already created a return
	// Check if the function already has a return instruction
	if (RetVal) {
		// Only create return if we don't already have one (ReturnExprAST creates its own)
		if (!Codegen::Builder->GetInsertBlock()->getTerminator()) {
			Codegen::Builder->CreateRet(RetVal);
		}
		verifyFunction(*F);
		return F;
	} else {
		// If no return value and no terminator, add a default return
		if (!Codegen::Builder->GetInsertBlock()->getTerminator()) {
			Codegen::Builder->CreateRet(ConstantFP::get(*Codegen::TheContext, APFloat(0.0)));
		}
		verifyFunction(*F);
		return F;
	}
	
	F->eraseFromParent();
	return nullptr;
}

void BehaviorAST::codegen() {
	// Generate code for all actions
	for (auto &Action : Actions) {
		Action->codegen(Name);
	}
}

void ObjectAST::codegen() {
	// For now, just register the object
	// TODO: this would create a vtable and handle inheritance
	fprintf(stderr, "Object %s inherits from: ", Name.c_str());
	for (const auto &Behavior : InheritedBehaviors) {
		fprintf(stderr, "%s ", Behavior.c_str());
	}
	fprintf(stderr, "\n");
}
