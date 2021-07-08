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

#define DEBUG_TYPE "dasm"

STATISTIC(dasmCounter, "Counts number of functions greeted");

namespace {
  // dasm - The first implementation, without getAnalysisUsage.
  struct dasm : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    dasm(): FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      Function *tmp=&F;
      for (Function::iterator bb = tmp->begin(); bb != tmp->end(); ++bb) {
        for (BasicBlock::iterator inst = bb->begin(); inst != bb->end(); ++inst) {
          if(isa<CallInst>(inst)){            
            if(inst->getOperand(0)->getName()=="__asm_it"){
              CallInst *instt=cast<CallInst>(inst);
              errs() << "getcall:" << *instt << '\n';
              MDNode *N = inst->getMetadata("insn.addr");
              Metadata *n1;
              n1=&*(N->getOperand(0));
              Value *v= cast< ValueAsMetadata >(n1)->getValue();
              ConstantInt *a=cast<ConstantInt>(v);
              TruncInst *t=new TruncInst(a,inst->getType(),"",instt);
              errs() << "t:" << *t<< '\n';
              instt->replaceAllUsesWith(t);
            //   instt->eraseFromParent();//这里不知道为啥CallInst没有被清除掉
           }
          }
        }
      }
      Function *tmp2=&*tmp;
    for (auto &B : *tmp2) {
        auto It = B.begin();
        // we modify B, so we must reevaluate end()
        while(It != B.end()) {
            auto &I = *It;
			    // errs() << "I" << I << '\n';
            if(isa<CallInst>(&I)&&(I.getOperand(0)->getName()=="__asm_it")){
              errs() << "getcall:" << I << '\n';
              // we continue with the next element
              It = I.eraseFromParent();
              }
              else {
                ++It;
              }
        }
     }
      return false;
    }
    

    
// opt -load /mnt/f/gitDownload/llvm-pass-skeleton/build/dasm/libdasmPass.so -dasm 3.ll

  };
}

char dasm::ID = 0;
static RegisterPass<dasm> X("dasm", "dasm Pass");