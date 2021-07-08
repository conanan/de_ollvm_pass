#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/Local.h" 
#include "llvm/Transforms/Utils/BasicBlockUtils.h"//辣鸡注释少个s，浪费我2个小时
#include "llvm/IR/BasicBlock.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/CFG.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Metadata.h"
#include <vector> 
#include <set>
#include <algorithm>
using namespace std;
using namespace llvm;

#define DEBUG_TYPE "dbcf"

STATISTIC(dbcfCounter, "Counts number of functions greeted");

namespace {
  // dbcf - The first implementation, without getAnalysisUsage.
  struct dbcf : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    dbcf(): FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
		Function *tmp=&F;
		//DEBUG_WITH_TYPE(DEBUG_TYPE, errs() << "charge function:" << tmp->getName() << "\n");
		errs() << "Function：" << F.getName() << '\n';
     for (Function::iterator bb = tmp->begin(); bb != tmp->end(); ++bb) {
		    //errs() << "block:" << *bb << '\n';
        for (BasicBlock::iterator inst = bb->begin(); inst != bb->end(); ++inst) {
			// errs() << "inst:" << inst->getOpcode() << '\n';
			// errs() << "inst:" << *inst << '\n';
		//if (inst->isBinaryOp()){
			// errs() << "inst:" << inst->getOpcode() << '\n';
			// errs() << "inst:" << *inst << '\n';
			ConstantInt *a=(ConstantInt *)ConstantInt::get(inst->getType(),-1);
			ConstantInt *b=(ConstantInt *)ConstantInt::get(inst->getType(),0);
			if(inst->getOpcode() == 17){
				errs() << "inst:" << *inst << '\n';

				BasicBlock::iterator instb=(--inst);
				++inst;
				// BasicBlock::iterator insta=(++inst);
				// --inst;
				if(instb->getOpcode() == 13){
					errs() << "inst:" << *instb << '\n';
					if(instb->getOperand(1) == a){
						if(inst->getOperand(1) == instb->getOperand(0)){
							// errs() << "inst:" << *insta << '\n';
							BinaryOperator *instt=cast<BinaryOperator>(inst);
							BinaryOperator *op=BinaryOperator::Create(Instruction::And,b,b,"",instt);
							
							instt->replaceAllUsesWith(op);
							errs() << "inst:" << *instt << '\n';
						}
					}
				}
			}
			
		//}
		}
   
		}
      return false;
    }
    

    
// opt -load /mnt/f/gitDownload/llvm-pass-skeleton/build/dbcf/libdbcfPass.so -dbcf 3.ll

  };
}

char dbcf::ID = 0;
static RegisterPass<dbcf> X("dbcf", "dbcf Pass");

// opt -sccp -ipsccp -simplifycfg -adce -mem2reg 
// opt -instcombine origin.ll -S >2.ll
// opt -load /mnt/f/gitDownload/llvm-pass-skeleton/build/dasm/libdasmPass.so -dasm 2.ll -S>dasm.ll 
// opt -load /mnt/f/gitDownload/llvm-pass-skeleton/build/dbcf/libdbcfPass.so -dbcf
// opt -sccp -ipsccp -simplifycfg -adce -instcombine -S dbcf.ll >3.ll
// opt -lowerswitch 3.ll -S > lowerswicth.ll


opt -dot-cfg 4.ll  
dot -Tpng -o $1.png .mmap.dot


opt -load /mnt/f/gitDownload/llvm-pass-skeleton/build/dflat/libdflatPass.so -dflat meta.ll 