#include "llvm/Pass.h" // writing a Pass
#include "llvm/IR/Function.h" //operating on Functions
#include "llvm/Support/raw_ostream.h" // doing some printing
#include "llvm/IR/InstIterator.h"
#include <map>
//the functions from the include files live in the llvm namespace
using namespace llvm; 
using namespace std;

//FunctionPass operates on a function at a time.
namespace {
	//This declares a “CSI” class that is a subclass of FunctionPass.
	struct CountStaticInstructions : public FunctionPass {
		static char ID;
		CountStaticInstructions() : FunctionPass(ID) {}
        // iterate over a Function’s BasicBlocks and then that BasicBlock’s Instructions
		bool runOnFunction(Function &F) override {
			map<string, int> inst_count;
			for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I){
				inst_count[I->getOpcodeName()]++;
			}
			for (map<string, int>::iterator I = inst_count.begin(), E = inst_count.end(); I != E; ++I) {
                errs() << I->first << '\t' << I->second << '\n';
            }

            return false;

		}
	};
}

char CountStaticInstructions::ID = 0;
static RegisterPass<CountStaticInstructions> X("cse231-csi", "CountInstStatically",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

