#include "llvm/Pass.h" // writing a Pass
#include "llvm/IR/Function.h" //operating on Functions
#include "llvm/Support/raw_ostream.h" // doing some printing
#include <map>
#include <vector>

#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h" //Insertion using an instance of IRBuilder
#include "/lib231/lib231.cpp"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/GlobalVariable.h"

//the functions from the include files live in the llvm namespace
using namespace llvm; 
using namespace std;

//FunctionPass operates on a function at a time.
namespace {
	//This declares a “CDI” class that is a subclass of ModulePass.
	struct CountDynamicInstructions : public ModulePass{
		static char ID;
		CountDynamicInstructions() : ModulePass(ID) {}
        Function* printFunc, *updateFunc;
		virtual bool runOnModule(Module &M){
			LLVMContext &Context = M.getContext();
			// getFunction - Look up the specified function in the module symbol table.
			Constant* print_func = M.getOrInsertFunction("printOutInstrInfo",
                                   Type::getVoidTy(Context),
            /* If it does not exist, return null.*/      NULL);
            vector <Type*> args_type;
            args_type.push_back(Type::getInt32Ty(Context));   /*unsigned num */
            args_type.push_back(Type::getInt32PtrTy(Context));/*uint32_t * keys*/
            args_type.push_back(Type::getInt32PtrTy(Context));/*uint32_t * values*/ 
            FunctionType* funcType = FunctionType::get(Type::getVoidTy(Context), args_type, false);
        	Constant* update_func = M.getOrInsertFunction("updateInstrInfo",funcType);
        	//The cast<> operator is a “checked cast” operation.
        	// It converts a pointer or reference from a base class to a derived class
        	printFunc = cast<Function>(print_func);
            updateFunc = cast<Function>(update_func);
            //Module::iterator - Typedef for function list iterator
            for(Module::iterator FI = M.begin(), FE = M.end(); FI!= FE; ++FI){
            	//Iterating over the BasicBlock in a Function
            	for (Function::iterator BI = FI->begin(), BE = FI->end(); BI != BE; ++BI){
            		map<int,int> inst_count;
            		//Iterating over all instructions in a BasicBlock
            		for (BasicBlock::iterator I = BI->begin(), E = BI->end(); I != E; ++I) {
            			//unsigned getOpcode()
            			inst_count[I->getOpcode()]++;
            		}
            		IRBuilder<> Builder(&*BI);
            		//getTerminator():Returns a pointer to the terminator instruction that appears at the end of the BasicBlock. 
            		//If there is no terminator instruction, or if the last instruction in the block is not a terminator, 
            		//then a null pointer is returned

            		//new instructions should be inserted into the end of the BB
                	Builder.SetInsertPoint(BI->getTerminator());
                	int num = inst_count.size();
	                vector<Value *> args;
	                vector<Constant *> keys;/*uint32_t * keys*/
	                vector<Constant *> values;/*uint32_t * values*/
	                for (map<int, int>::iterator it = inst_count.begin(), end = inst_count.end(); it != end; ++it){
	                	keys.push_back(ConstantInt::get(Type::getInt32Ty(Context), it->first));
	                	values.push_back(ConstantInt::get(Type::getInt32Ty(Context), it->second));
	                }
	                ArrayType *arrayTy = ArrayType::get(Type::getInt32Ty(Context), num);
                	Value* IdxList[2] = {ConstantInt::get(Type::getInt32Ty(Context), 0), ConstantInt::get(Type::getInt32Ty(Context), 0)};

                	GlobalVariable *Globalkeys = new GlobalVariable(M, arrayTy, true, 
                    GlobalVariable::InternalLinkage, ConstantArray::get(arrayTy, keys), "key_global");
                	GlobalVariable *GlobalValues = new GlobalVariable(M, arrayTy, true, 
                    GlobalVariable::InternalLinkage, ConstantArray::get(arrayTy, values), "value_global");

                    args.push_back(ConstantInt::get(Type::getInt32Ty(Context), num));
	                args.push_back(Builder.CreateInBoundsGEP(Globalkeys, IdxList));
	                args.push_back(Builder.CreateInBoundsGEP(GlobalValues, IdxList));
                    
                    // Insert updateInstrInfo at the end of each BasicBlock
	                Builder.CreateCall(updateFunc, args);

	                for (BasicBlock::iterator i = BI->begin(), e = BI->end(); i != e; ++i) {
	                    if ((string) i->getOpcodeName() == "ret") {
	                        Builder.SetInsertPoint(&*i);
	                        Builder.CreateCall(printFunc);
	                    }
                	}
            	}
            }
            return false;
         }
	};
}

char CountDynamicInstructions::ID = 0;
static RegisterPass<CountDynamicInstructions> X("cse231-cdi", "CountDynamicInstructions",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

