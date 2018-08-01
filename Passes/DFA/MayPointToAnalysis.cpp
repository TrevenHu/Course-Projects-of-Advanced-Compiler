#include <set>
#include <map>
#include <deque>
#include <vector>
#include <string>
#include <utility>
#include "231DFA.h"
#include "llvm/Pass.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/raw_ostream.h"

using namespace std;
using namespace llvm;

namespace llvm {
    class MayPointToInfo:public Info{
    	private:
    		 map<string, set<string>> MayPT_map;
    	public:
    		MayPointToInfo() {}
    		MayPointToInfo(map<string, set<string>> m){
    			MayPT_map = m;
        	}
        	~MayPointToInfo() {}
            // add new info into may_point_to map
        	void addToInfo(string pointer, string pointee){
            	MayPT_map[pointer].insert(pointee);
        	}
        	// get may_point_to map
        	map<string, set<string>> getInfoMap() {
				map<string, set<string>> tmp_map;
				for (auto it = MayPT_map.begin();it != MayPT_map.end();it++) {
	                for (auto &p: it->second)
	                    tmp_map[it->first].insert(p);
            	}
            	return tmp_map;
		    }
		    // get pointee(memory) set
		    set<string> getMemSet(string pointer){
			    set<string> tmp_set;
	            for (auto &it: MayPT_map[pointer])
	                tmp_set.insert(it);
	            return tmp_set;
		    }

		    /*
	     * Compare two pieces of information
	     *
	     * Direction:
	     *   In your subclass you need to implement this function.
	     */
	    	static bool equals(MayPointToInfo * info1, MayPointToInfo * info2){
	    		return info1->getInfoMap() == info2->getInfoMap();
	    	}

	    	/*
	     * Print out the information
	     *
	     * Direction:
	     *   In your subclass you should implement this function according to the project specifications.
	     */
		    void print(){
		    	for (auto it = MayPT_map.begin();it != MayPT_map.end(); it++){
		    		if(it -> second.size() == 0) continue;
		    		errs() << it->first << "->(";
		    		for (auto &p: it->second)
		    			errs() << p << "/";
		    		errs() << ")|";
		    	}
		    	errs() << "\n";
		    }

		 /*
	     * Join two pieces of information.
	     * The third parameter points to the result.
	     *
	     * Direction:
	     *   In your subclass you need to implement this function.
	     */
	    	static void join(MayPointToInfo * info1, MayPointToInfo * info2, MayPointToInfo * result){
	    		for (auto it = info1->MayPT_map.begin(); it != info1->MayPT_map.end(); it++){
					for (auto &p: it->second)
						result->MayPT_map[it->first].insert(p);
	    		}
		    	for (auto it = info2->MayPT_map.begin(); it != info2->MayPT_map.end(); it++){
		    		for (auto &p: it->second)
						result->MayPT_map[it->first].insert(p);
		    	}
	    	}
	};

    class MayPointToAnalysis : public DataFlowAnalysis<MayPointToInfo, true>{
    	public:
	        MayPointToAnalysis(MayPointToInfo &bottom, MayPointToInfo &initialState):
	        DataFlowAnalysis(bottom, initialState) {}
	        ~MayPointToAnalysis() {}
	        void flowfunction(Instruction * I, vector<unsigned> &IncomingEdges,
		 									vector<unsigned> &OutgoingEdges,
		 									vector<MayPointToInfo *> &Infos){

	        	 MayPointToInfo *res_info = new MayPointToInfo();

	        	 string opName = I->getOpcodeName();

                 // get private member by *get* functions
	        	 typedef pair<unsigned, unsigned> Edge;
				 map<Instruction*, unsigned> InstrToIndex = getInstrToIndex();
				 map<Edge, MayPointToInfo *> EdgeToInfo = getEdgeToInfo();
				 unsigned Ri = InstrToIndex[I];

				 //join the incoming info
				 for(unsigned i = 0; i < IncomingEdges.size(); i++){
				 	Edge in_edge = Edge(IncomingEdges[i], Ri);
					MayPointToInfo::join(res_info, EdgeToInfo[in_edge], res_info);
				 }
                 //alloca 
                 // out = in U {Ri -> Mi}
				 if(opName == "alloca")
				 	res_info->addToInfo("R"+ to_string(Ri), "M"+ to_string(Ri));
				 //bitcast
				 // out = in U {Ri-> X | Rv -> X}
				 else if (opName == "bitcast"){
				 	//The dyn_cast<> operator is a “checking cast” operation. 
			 		//It checks to see if the operand is of the specified type, and if so, 
			 		//returns a pointer to it (this operator does not work with references). 
			 		//If the operand is not of the correct type, a null pointer is returned.
			 		Instruction* v = dyn_cast<Instruction>(I -> getOperand(0));
			 		if(v){
			 			string Rv = "R" + to_string(InstrToIndex[v]);
			 			MayPointToInfo *tmp_info = new MayPointToInfo();
			 			// for all X that Rv may point to
			 			for (auto &X:res_info->getMemSet(Rv)){
			 				// Ri-> X
			 				tmp_info->addToInfo("R"+to_string(Ri), X);
			 			}
			 			MayPointToInfo::join(res_info, tmp_info, res_info);
			 		}
				 }
				 //getelementptr
				 // out = in U {Ri-> X | Rv -> X}
				 else if (opName == "getelementptr"){
				 	GetElementPtrInst * instr = cast<GetElementPtrInst> (I);
				 	Instruction* v = (Instruction *) instr->getPointerOperand();
			 		string Rv = "R" + to_string(InstrToIndex[v]);
			 		MayPointToInfo *tmp_info = new MayPointToInfo();
			 		// for all X that Rv may point to
		 			for (auto &X:res_info->getMemSet(Rv)){
		 				// Ri-> X
		 				tmp_info->addToInfo("R"+to_string(Ri), X);
		 			}
		 			MayPointToInfo::join(res_info, tmp_info, res_info);
				 }
				 //load
				 // out = in U {Ri-> Y | Rp -> X && X -> Y}
				 else if (opName == "load"){
				 	if(I->getType()->isPointerTy()){
				 		LoadInst * instr = cast<LoadInst> (I);
				 		Instruction* p = (Instruction *) instr->getPointerOperand();
			 			string Rp = "R" + to_string(InstrToIndex[p]);
			 			MayPointToInfo *tmp_info = new MayPointToInfo();
			 			// for all X that Rp may point to
			 			for (auto &X:res_info->getMemSet(Rp)){
			 				// for all Y that X may point to
			 				 for (auto &Y:res_info->getMemSet(X)){
			 				 	// Ri -> Y
			 				 	tmp_info->addToInfo("R"+to_string(Ri), Y);
			 				 }
			 			}
			 			MayPointToInfo::join(res_info, tmp_info, res_info);
				 	}
				 }
				 // store
				 //out = in U {Y -> X | Rv -> X && Rp -> Y}
				 else if (opName == "store"){
				 	StoreInst * instr = cast<StoreInst> (I);
				 	Instruction* v = (Instruction *) instr->getValueOperand(); // return getOperand(0);
				 	Instruction* p = (Instruction *) instr->getPointerOperand(); // return getOperand(1);
				 	string Rv = "R" + to_string(InstrToIndex[v]);
				 	string Rp = "R" + to_string(InstrToIndex[p]);
				 	MayPointToInfo *tmp_info = new MayPointToInfo();
				 	// for all X that Rv may point to
				 	for (auto &X:res_info->getMemSet(Rv)){
		 				// for all Y that Rp may point to
		 				for (auto &Y:res_info->getMemSet(Rp)){
		 				 	// Y -> X
		 				 	tmp_info->addToInfo(Y, X);
		 				}
			 		}
			 		MayPointToInfo::join(res_info, tmp_info, res_info);
				}
                //select
                //out = in U {Ri -> X | R1 -> X} U {Ri -> X | R2 -> X}
				else if (opName == "select"){
					SelectInst * instr = cast<SelectInst> (I);
					Instruction* r1 = (Instruction *) instr->getTrueValue(); // return Op<1>();
				 	Instruction* r2 = (Instruction *) instr->getFalseValue(); // return Op<2>();
				 	string R1 = "R" + to_string(InstrToIndex[r1]);
				 	string R2 = "R" + to_string(InstrToIndex[r2]);
                    MayPointToInfo *tmp_info = new MayPointToInfo();
                    // for all X that R1 may point to
		 			for (auto &X:res_info->getMemSet(R1)){
		 				// Ri-> X
		 				tmp_info->addToInfo("R"+to_string(Ri), X);
		 			}
		 			// for all X that R2 may point to
		 			for (auto &X:res_info->getMemSet(R2)){
		 				// Ri-> X
		 				tmp_info->addToInfo("R"+to_string(Ri), X);
		 			}
		 			MayPointToInfo::join(res_info, tmp_info, res_info);
				}
                //phi
                //out = in U {Ri -> X | R1 -> X} U ... U {Ri -> X | Rk -> X}
				else if (opName == "phi"){
					MayPointToInfo *tmp_info = new MayPointToInfo();

					BasicBlock* block = I->getParent();
					for (auto i = block->begin(), e = block->end(); i != e; i++){
						Instruction * instr = &*i;
						if(isa<PHINode>(instr)){
							for (unsigned j = 0; j < instr->getNumOperands(); j++){
								Instruction* k = (Instruction*)(instr->getOperand(j));
								string Rk = "R" + to_string(InstrToIndex[k]);
								// for all X that Rk may point to
					 			for (auto &X:res_info->getMemSet(Rk)){
					 				// Rk-> X
					 				tmp_info->addToInfo("R"+to_string(Ri), X);
					 			}
							}
						}
					}
					MayPointToInfo::join(res_info, tmp_info, res_info);
				}

				for(unsigned i = 0; i < OutgoingEdges.size(); i++)
					Infos.push_back(res_info);
				
				return;
	        }
    };

    struct MayPointToAnalysisPass : public FunctionPass {
    	static char ID;
    	MayPointToAnalysisPass() : FunctionPass(ID) {}
    	bool runOnFunction(Function &F) override {
    		MayPointToInfo bottom;
			MayPointToInfo initial_state;
		    MayPointToAnalysis MPT (bottom, initial_state);
		    MPT.runWorklistAlgorithm(&F);
			MPT.print();
		    return false;
    	}
    };
}

char MayPointToAnalysisPass::ID = 0;
static RegisterPass<MayPointToAnalysisPass> X("cse231-maypointto", "MayPointToAnalysis",
                         false /* Only looks at CFG */,
                         false /* Analysis Pass */);