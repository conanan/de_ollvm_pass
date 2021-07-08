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

using namespace llvm;

#define DEBUG_TYPE "replaceGlobal"

STATISTIC(replaceGlobalCounter, "Counts number of functions greeted");

namespace {
  // replaceGlobal - The first implementation, without getAnalysisUsage.
  struct replaceGlobal : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    replaceGlobal(): ModulePass(ID) {}

    bool runOnModule(Module &M) {
      //     for (Module::global_iterator gi = M.global_begin(), ge = M.global_end();gi != ge; ++gi) {
      //     GlobalVariable* gv = &(*gi);
      //     for (User *U : gv->users()) {
      //       errs() << "user:" <<U->getType()->isIntegerTy()<< '\n';

      //     }
       
      // }
      for (Module::global_iterator gi = M.global_begin(), ge = M.global_end();gi != ge; ++gi) {
          // errs() << "global:" << gi->getName()<< '\n';
          GlobalVariable* gv = &(*gi);
          std::vector<Value*>obj;
          std::vector<Constant*>obj2;
          for (User *U : gv->users()) {
            if(U->getType()->isIntegerTy()){
              for (Use &U1 : U->operands()) {
                Value *v = U1.get();
                errs() << "u:" << *U<< '\n';
                obj.push_back(v);
                errs() << "valueV:" <<*v<< '\n';
                if(cast<GlobalVariable>(v)){
                  Constant *initializer = gv->getInitializer();
                  errs() << "valueC:" <<*initializer<< '\n';
                  // if(cast<IntToPtrInst>(initializer)){}
                  obj2.push_back(initializer);
                }
              }
            }
          }
          for(int i=0;i<obj2.size();i++)
          {
            errs() << "obj:" <<*obj[i]<< '\n';
            errs() << "obj2:" <<*obj2[i]<< '\n'; 
            if(dyn_cast<ConstantInt>(obj2[i])){
              obj[i]->replaceAllUsesWith(obj2[i]);
            }
          }          
      }
      return false;
    }
// opt -load /mnt/f/gitDownload/llvm-pass-skeleton/build/replaceGlobal/libreplaceGlobalPass.so -replaceGlobal inst.ll  

  };
}

char replaceGlobal::ID = 0;
static RegisterPass<replaceGlobal> X("replaceGlobal", "replaceGlobal Pass");

