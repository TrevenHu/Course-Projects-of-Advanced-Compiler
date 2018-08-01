#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "231DFA.h"
#include "llvm/IR/InstIterator.h"
#include <set>

using namespace llvm;
using namespace std;

namespace llvm{
	class ReachingInfo: public Info{
		public: 
			ReachingInfo(): Info() {}
			~ReachingInfo(){}
			set<unsigned> info_set;
	    /*
	     * Compare two pieces of information
	     *
	     * Direction:
	     *   In your subclass you need to implement this function.
	     */
	    static bool equals(ReachingInfo * info1, ReachingInfo * info2){
	    	if (info1->info_set.size() != info2->info_set.size())
      			return false;
	    	vector<unsigned> v1,v2;
	    	for(auto it = info1->info_set.begin(); it != info1->info_set.end(); it++)
	    		v1.push_back(*it);
	    	for(auto it = info2->info_set.begin(); it != info2->info_set.end(); it++)
	    		v2.push_back(*it);
	    	sort(v1.begin(),v1.end());
	    	sort(v2.begin(),v2.end());
	    	for(int i = 0; i<v1.size(); i++)
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
				errs() << *it << '|';
			errs() << '\n';
	    }
	     /*
	     * Join two pieces of information.
	     * The third parameter points to the result.
	     *
	     * Direction:
	     *   In your subclass you need to implement this function.
	     */
	    static ReachingInfo* join(ReachingInfo * info1, ReachingInfo * info2, ReachingInfo * result){
	    	for (auto it = info1->info_set.begin(); it != info1->info_set.end(); it++)
				result->info_set.insert(*it);
	    	for (auto it = info2->info_set.begin(); it != info2->info_set.end(); it++)
	    		result->info_set.insert(*it);
	    	return nullptr;
	    }
	};
	class ReachingDefinitionAnalysis : public DataFlowAnalysis<ReachingInfo, true>{
		public:
			ReachingDefinitionAnalysis(ReachingInfo & bottom, ReachingInfo & initial_state):
			DataFlowAnalysis(bottom, initial_state) {}
			~ReachingDefinitionAnalysis() {}
			void flowfunction(Instruction * I, std::vector<unsigned> & IncomingEdges,
									   				vector<unsigned> & OutgoingEdges,
									   				vector<ReachingInfo *> & Infos){

				typedef pair<unsigned, unsigned> Edge;
				map<Instruction*, unsigned> InstrToIndex = getInstrToIndex();
				map<Edge, ReachingInfo *> EdgeToInfo = getEdgeToInfo();
				

				unsigned instr_index = InstrToIndex[I];
				
				ReachingInfo * out_info = new ReachingInfo();
				//join the incoming info
				for (int i = 0; i < IncomingEdges.size(); i++) {
					Edge in_edge = Edge(IncomingEdges[i], instr_index);
					ReachingInfo::join(out_info, EdgeToInfo[in_edge], out_info);
				}

				string opName = I->getOpcodeName();
				//First Category: IR instructions that return a value (defines a variable)
				//Second Category: IR instructions that do not return a value
				//Third Category: phi instructions
				int cat;
				if(Category.count(opName)) cat = Category[opName];
				else if(I->isBinaryOp()) cat = 1;//binary operations
				else cat = 2;
          
				if (cat == 1)
					out_info->info_set.insert(instr_index);
				else if (cat == 3) {
					 BasicBlock* block = I->getParent();
			          for (auto i = block->begin(), e = block->end(); i != e; ++i) {
			            Instruction * instr = &*i;
			            if (isa<PHINode>(instr)){
			              out_info->info_set.insert(InstrToIndex[instr]);
			            }
          			}
				}
				for(int i = 0 , size = OutgoingEdges.size() ; i < size ; i++)
			        Infos.push_back(out_info);
			}
		private:
			map<string, int> Category = {{"br", 2}, {"switch", 2}, {"alloca", 1}, {"load", 1}, {"store", 2},
				{"getelementptr", 1}, {"icmp", 1}, {"fcmp", 1}, {"phi", 3}, {"select", 1}};
	};
	struct ReachingDefinitionAnalysisPass : public FunctionPass{
		static char ID;
		ReachingDefinitionAnalysisPass() : FunctionPass(ID) {}
		bool runOnFunction(Function &F) override {
			ReachingInfo bottom;
			ReachingInfo initial_state;
			ReachingDefinitionAnalysis RDA(bottom, initial_state);
			RDA.runWorklistAlgorithm(&F);
			RDA.print();
			return false;

		}
	};
}

char ReachingDefinitionAnalysisPass::ID = 0;
static RegisterPass<ReachingDefinitionAnalysisPass> X("cse231-reaching", "ReachingDef",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);