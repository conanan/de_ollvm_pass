//===- dbcf.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "dbcf World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

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

#include "llvm/ADT/Statistic.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
using namespace llvm;

#define DEBUG_TYPE "dbcf"

STATISTIC(dbcf, "去除bcf");

namespace {
  // dbcf - The first implementation, without getAnalysisUsage.
  struct dbcf : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    dbcf() : FunctionPass(ID) {}

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
			if(inst->getOpcode() == 15){
				errs() << "inst:" << *inst << '\n';

				BasicBlock::iterator instb=(--inst);
				++inst;
				BasicBlock::iterator insta=(++inst);
				--inst;
				if(instb->getOpcode() == 11){
					errs() << "inst:" << *instb << '\n';
					if(instb->getOperand(1) == a){
						if(inst->getOperand(1) == instb->getOperand(0)){
							errs() << "inst:" << *insta << '\n';
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
  };
}

char dbcf::ID = 0;
static RegisterPass<dbcf> X("dbcf", "dbcf World Pass");

