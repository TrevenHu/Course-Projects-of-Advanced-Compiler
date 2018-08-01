#include "llvm/Pass.h" // writing a Pass
#include "llvm/IR/Function.h" //operating on Functions
#include "llvm/Support/raw_ostream.h" // doing some printing


#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h" //Insertion using an instance of IRBuilder
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/BasicBlock.h"

//the functions from the include files live in the llvm namespace
using namespace llvm; 
using namespace std;

//FunctionPass operates on a function at a time.
namespace {
	//This declares a “CDI” class that is a subclass of ModulePass.
	struct BranchBias : public ModulePass{
		static char ID;
		BranchBias() : ModulePass(ID) {}
		Function* printBB, *updateBB;
		virtual bool runOnModule(Module &M){
			LLVMContext &Context = M.getContext();
			// getFunction - Look up the specified function in the module symbol table.
			Constant* print_branch = M.getOrInsertFunction("printOutBranchInfo",
                                   Type::getVoidTy(Context),
            /* If it does not exist, return null.*/      NULL);
            vector <Type*> args_type;
            args_type.push_back(Type::getInt1Ty(Context));   /*bool taken */
            FunctionType* funcType = FunctionType::get(Type::getVoidTy(Context), args_type, false);
        	Constant* update_branch = M.getOrInsertFunction("updateBranchInfo",funcType);
        	//The cast<> operator is a “checked cast” operation.
        	// It converts a pointer or reference from a base class to a derived class
        	printBB = cast<Function>(print_branch);
            updateBB = cast<Function>(update_branch);
            //Module::iterator - Typedef for function list iterator
            for(Module::iterator FI = M.begin(), FE = M.end(); FI!= FE; ++FI){
            	//Iterating over the BasicBlock in a Function
            	for (Function::iterator BI = FI->begin(), BE = FI->end(); BI != BE; ++BI){
            		//Iterating over all instructions in a BasicBlock
            		IRBuilder<> Builder(&*BI);
            		//getTerminator():Returns a pointer to the terminator instruction that appears at the end of the BasicBlock. 
            		//If there is no terminator instruction, or if the last instruction in the block is not a terminator, 
            		//then a null pointer is returned

            		// The last instruction of BB
            		Instruction * EndI = BI->getTerminator();
            		//new instructions should be inserted into the end of the BB
                	Builder.SetInsertPoint(EndI);
                	// BranchInst - Conditional or Unconditional Branch instruction
                	// is a conditional branch
                	if (isa<BranchInst>(EndI)) {
                		BranchInst *bi = dyn_cast<BranchInst>(EndI);
                		if(bi->isConditional()){
                			vector<Value *> args;
                			args.push_back(bi->getCondition());
                			Builder.CreateCall(updateBB, args);
                		}

                	}
            		for (BasicBlock::iterator i = BI->begin(), e = BI->end(); i != e; ++i) {
	                    if ((string) i->getOpcodeName() == "ret") {
	                        Builder.SetInsertPoint(&*i);
	                        Builder.CreateCall(printBB);
	                    }
                	}
            	}
            }
            return false;
		}
	};
}
	char BranchBias::ID = 0;
    static RegisterPass<BranchBias> X("cse231-bb", "BranchBias",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);








