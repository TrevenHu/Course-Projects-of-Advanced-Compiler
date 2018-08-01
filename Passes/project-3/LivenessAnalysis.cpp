#include <set>
#include <map>
#include <deque>
#include <vector>
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

namespace llvm{
	class LivenessInfo: public Info {
		private:
			set<unsigned> info_set;
		public: 
			LivenessInfo(): Info() {}
			// LivenessInfo(LivenessInfo& AInfo){
	  //           for (auto &it: AInfo.getInfoSet())
	  //               info_set.insert(it);
	  //       }
            // get info_set
			set<unsigned> getInfoSet(){
				set<unsigned> tmp_set;
	            for (auto it = info_set.begin();it!=info_set.end();it++)
	                tmp_set.insert(*it);
	            return tmp_set;
	        }
			
			~LivenessInfo(){}
		/*
	     * Compare two pieces of information
	     *
	     * Direction:
	     *   In your subclass you need to implement this function.
	     */
			static bool equals(LivenessInfo * info1, LivenessInfo * info2){
				if (info1->info_set.size() != info2->info_set.size())
      			return false;
		    	vector<unsigned> v1,v2;
		    	for(auto it = info1->info_set.begin(); it != info1->info_set.end(); it++)
		    		v1.push_back(*it);
		    	for(auto it = info2->info_set.begin(); it != info2->info_set.end(); it++)
		    		v2.push_back(*it);
		    	sort(v1.begin(),v1.end());
		    	sort(v2.begin(),v2.end());
		    	for(unsigned i = 0; i<v1.size(); i++)
		    		if(v1[i]!=v2[i]) return false;
		    	return true;
			}
		/*
	     * Print out the information
	     *
	     * Direction:
	     *   In your subclass you should implement this function according to the project specifications.
	     */
			void print(){
			   for (auto it = info_set.begin(); it != info_set.end(); it++)
			      errs() << *it << "|";
			    errs() << "\n";
			 }
        /*
	     * Join two pieces of information.
	     * The third parameter points to the result.
	     *
	     * Direction:
	     *   In your subclass you need to implement this function.
	     */
			static void join(LivenessInfo * info1, LivenessInfo * info2, LivenessInfo * result){
			 	for (auto it = info1->info_set.begin(); it != info1->info_set.end(); it++)
					result->info_set.insert(*it);
		    	for (auto it = info2->info_set.begin(); it != info2->info_set.end(); it++)
		    		result->info_set.insert(*it);
			}

			void addAInfo(unsigned n){
	            info_set.insert(n);
	        }

	        void deleteAInfo(unsigned n) {
	            info_set.erase(n);
	        }
         
	};
	class LivenessAnalysis: public DataFlowAnalysis<LivenessInfo, false>{
		public:
			LivenessAnalysis(LivenessInfo &bottom, LivenessInfo &initialState):
			DataFlowAnalysis(bottom, initialState) {}
			~LivenessAnalysis() {}
			void flowfunction(Instruction * I, vector<unsigned> &IncomingEdges,
			 									vector<unsigned> &OutgoingEdges,
			 									vector<LivenessInfo *> &Infos){
			 	LivenessInfo *res_info = new LivenessInfo();
                Infos.resize(OutgoingEdges.size());
                // get private member by *get* functions
			 	typedef pair<unsigned, unsigned> Edge;
			 	map<unsigned, Instruction *> IndexToInstr = getIndexToInstr();
				map<Instruction*, unsigned> InstrToIndex = getInstrToIndex();
				map<Edge, LivenessInfo *> EdgeToInfo = getEdgeToInfo();

			 	unsigned instr_index = InstrToIndex[I];

			 	//join the incoming info
				for (unsigned i = 0; i < IncomingEdges.size(); i++) {
					Edge in_edge = Edge(IncomingEdges[i], instr_index);
					LivenessInfo::join(res_info, EdgeToInfo[in_edge], res_info);
				}

				string opName = I->getOpcodeName();

				unsigned NumOp = I->getNumOperands();
			    
				
				//First Category: IR instructions that return a value (defines a variable)
				//Second Category: IR instructions that do not return a value  !!!CALL
				//Third Category: phi instructions
				int cat;
				if(Category.count(opName)) cat = Category[opName];
				else if(I->isBinaryOp()) cat = 1;//binary operations
				else cat = 2;

                //alloca, load, getelementptr, icmp, fcmp, select
                //index is the index of the IR instruction, 
                //which corresponds to the variable <result> being defined.
				if (cat == 1){
					//set<unsigned> index = {instr_index};
					// join all operands
					// the set of variables used (and therefore needed to be live) in the body of the instruction
					for (unsigned i = 0; i < NumOp; i++) {
						auto *tmp = I->getOperand(i);

						if (isa<Instruction>(tmp))
                        	res_info->addAInfo(InstrToIndex[(Instruction*)tmp]);
					}
					// remove {index}
					//LivenessInfo::substract(res_info, new LivenessInfo(index), res_info);
					res_info->deleteAInfo(instr_index);
				}
				// treat call as the second catgory
				else if (cat == 2) {
					// join all operands
                    for (unsigned i = 0; i < NumOp; i++) {
						auto *tmp = I->getOperand(i);

						if (isa<Instruction>(tmp))
                        	res_info->addAInfo(InstrToIndex[(Instruction*)tmp]);
					}
				}
                // phi 
				else{
					BasicBlock* block = I->getParent();
					for (auto i = block->begin(), e = block->end(); i != e; i++){
						Instruction * instr = &*i;
						if(isa<PHINode>(instr)){
							//exclude variables defined at each phi instruction
							res_info->deleteAInfo(InstrToIndex[instr]);
						}
					}
				}

				for (unsigned i = 0; i < OutgoingEdges.size(); i++){
	                LivenessInfo *tmp_info = new LivenessInfo();
	                for (auto it:res_info->getInfoSet())
	                    tmp_info->addAInfo(it);
	                Infos[i] = tmp_info;
		        }

                // phi
				if(cat == 3){
					BasicBlock* block = I->getParent();
					for (auto i = block->begin(), e = block->end(); i != e; i++){
						Instruction * instr = &*i;
						// for each phi_i
						if(isa<PHINode>(instr)){
							PHINode *phi_i = (PHINode *)instr;
                            // for each value_ij in phi_i node.
							for (unsigned j = 0; j < phi_i->getNumIncomingValues(); j++){
								// for each out_edge_k
								for (unsigned k = 0; k < OutgoingEdges.size(); k++){
									
									auto *label_ij = phi_i->getIncomingBlock(j);
									auto *label_k = IndexToInstr[OutgoingEdges[k]]->getParent();
									// if value[ij]'s label == label that out[k] head to
									if (label_ij == label_k){
										// update out[k]
										auto *value_ij = phi_i->getIncomingValue(j);
										unsigned idx_to_add = InstrToIndex[(Instruction*) value_ij];
										if (idx_to_add != 0)
                                    		Infos[k]->addAInfo(idx_to_add);
									}
								}
							}
						}
					}
				}
			 return;	
		}
		private:
			map<string, int> Category = {{"call",2}, {"br", 2}, {"switch", 2}, {"alloca", 1}, {"load", 1}, {"store", 2},
				{"getelementptr", 1}, {"icmp", 1}, {"fcmp", 1}, {"phi", 3}, {"select", 1}};
	};

	struct LivenessAnalysisPass : public FunctionPass {
		static char ID;
		LivenessAnalysisPass() : FunctionPass(ID) {}
		bool runOnFunction(Function &F) override {
			LivenessInfo bottom;
			LivenessInfo initial_state;
		    LivenessAnalysis LA (bottom, initial_state);
		    LA.runWorklistAlgorithm(&F);
			LA.print();
		    return false;
		}
	};
}

char LivenessAnalysisPass::ID = 0;
static RegisterPass<LivenessAnalysisPass> X("cse231-liveness", "LivenessAnalysis",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);