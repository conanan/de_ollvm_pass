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

#define DEBUG_TYPE "dflat"

STATISTIC(dflat, "Counts number of functions greeted");

namespace {
  // dflat - The first implementation, without getAnalysisUsage.
  struct dflat : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    dflat(): FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      Function *tmp=&F;
      BasicBlock *ob=NULL;
      bool flatOrNot=hasFlatten(*tmp,&ob);
      // errs() << "混淆block:" << *ob << '\n';
      if(flatOrNot){
        Instruction *var=getSwitchVar(*ob);
        // errs() << "getSwitchVar:" << *var << '\n';
        match(*tmp,*var);
      }
      return false;
    }
    bool hasFlatten(Function &F,BasicBlock **ob){
      Function *tmp=&F;
      bool flat=false;
      for (Function::iterator bb = tmp->begin(); bb != tmp->end(); ++bb) {
          BasicBlock *tmp=&*bb;        
          int numPre=0;
          pred_iterator PI = pred_begin(tmp), E = pred_end(tmp);
          for (;PI != E; ++PI) {
            numPre+=1;
          }
          if(numPre>=6){
            flat=true;
            static BasicBlock *obb=&*tmp;
            *ob=&*tmp; 
            // errs() << "混淆blockterm:" << *obb<< '\n';
          }
      }
      return flat;
    }

    Instruction* getSwitchVar(BasicBlock &ob){
        BasicBlock *tmp=&ob;
        BasicBlock *succ = tmp->getTerminator()->getSuccessor(0);
        // errs() << "混淆block succe:" << *succ << '\n';   
        BranchInst *br = NULL;
        br = cast<BranchInst>(succ->getTerminator());  
        if(br->isConditional()){  //收集所有case和phi控制点
            ICmpInst *ins=NULL;
            ins = cast<ICmpInst>(br->getOperand(0));
            Instruction *op0=NULL;
            op0 = cast<Instruction>(ins->getOperand(0));//op0就是switchvar
          return op0;
        }
        else{
          return NULL;
        }

    }
    void match(Function &F,Instruction& op){
      Function *tmp=&F;
      Instruction *op0=&op;
      vector<ICmpInst*>inst;
      vector<BasicBlock*>blk;
      vector<BasicBlock*>blk2;//真实块起点
      vector<Constant*>num;
      vector<int>predicate;
      PHINode* phi;
      for (User *U : op0->users()) {//这里保存所有switch控制块
        if (Instruction *Inst = dyn_cast<Instruction>(U)) {
          // errs() << Inst->getParent()->getName()<< "\n";
          if(ICmpInst *Inst = dyn_cast<ICmpInst>(U)){
            inst.push_back(Inst);
            blk.push_back(Inst->getParent());
            num.push_back(dyn_cast<Constant>(Inst->getOperand(1)));
            predicate.push_back(Inst->getPredicate());
            }
          else if(PHINode *Inst = dyn_cast<PHINode>(U)){
            phi=Inst;
            // errs() <<"phi:"<< phi->getParent()<< '\n';
            }
          }
        }
          // for(int i=0;i<inst.size();i++)
          // {
          //   errs() << "inst:" <<*inst[i]<< '\n';
          //   errs() << "blk:" <<*(blk[i]->getTerminator())->getOperand(0)<< '\n';
          //   errs() << "num:" <<*num[i]<< '\n';
          //   errs() << "predicate:" <<predicate[i]<< '\n';
          // }          

      for(int i=0;i<blk.size();i++){//这里从控制块往下走，直到找到真实块
          succ_iterator PI = succ_begin(blk[i]), E = succ_end(blk[i]);
          for (;PI != E; ++PI) {
            // errs() << *blk[i]<< '\n';
            // errs() << **PI<< '\n';//iterator指针指向block指针指向block
            // BasicBlock* blk_now=cast<BasicBlock*>(&(**PI));
            vector<BasicBlock*>::iterator it=find(blk.begin(), blk.end(), *PI);
            bool switch_=(*PI!=dyn_cast<BasicBlock>(phi->getParent()));
            if(switch_&&(it == blk.end())){
               blk2.push_back(*PI);
            }
          }
        }
        vector<BasicBlock*>blk_real;
        for(int i=0;i<blk2.size();i++){
          errs() <<"real block:"<< *blk2[i]<< '\n';
          int num=0;
          BasicBlock& BB = *blk2[i];
          for (Instruction &I : BB)num+=1;
          if(num>1){
            blk_real.push_back(blk2[i]);}
          }        
          // for(int i=0;i<blk_real.size();i++)
          // {
          //   errs() << "blk_real:" <<*blk_real[i]<< '\n';
          // }   
        vector<Constant*>phinum;
        vector<BasicBlock*>blk3;
        for(int i=0;i<phi->getNumIncomingValues();i++){//这里根据phi节点确定每条路径的SwitchVar值
          // blk3.push_back(phi->getIncomingBlock(i));
          Value* a=phi->getIncomingValue(i);
          // errs() << *a<< '\n';
          int j=0;
          while((!isa<Constant>(a))&&(j<5)){
            j+=1;
            a=cast<Instruction>(a)->getOperand(1);
          }
          if(dyn_cast<Constant>(a)){
            phinum.push_back(dyn_cast<Constant>(a));
            blk3.push_back(phi->getIncomingBlock(i));
            }
        }
          for(int i=0;i<phinum.size();i++)
          {
            // errs() << "blk3:" <<blk3[i]->getName()<< '\n';
            if(dyn_cast<ConstantExpr>(phinum[i])){
              ConstantExpr* exp=cast<ConstantExpr>(phinum[i]);
              ConstantInt *b=(ConstantInt *)ConstantInt::get(exp->getType(),0);
              Constant* cons=exp->getWithOperandReplaced(0,b);
              // errs() << "getAsInstruction:" <<*cons<< '\n';
              phinum[i]=cons;
            }
            // errs() << "num:" <<*phinum[i]<< '\n';
          }  
//开始  
      Constant* x1=dyn_cast<Constant>(dyn_cast<PHINode>(op0)->getIncomingValue(0));
      BasicBlock* blknow=(phi->getParent())->getSingleSuccessor();
      BasicBlock* rtn;
      // errs() << "blknow:" <<*blknow<< '\n';
      APInt a1=x1->getUniqueInteger();
      bool branch;int pre;
      int i=1;
      errs() << "switch_var:" <<a1<< '\n';
      vector<BasicBlock*>excute;
    loop:
        ICmpInst* inst1=cast<ICmpInst>(blknow->getTerminator()->getOperand(0));   
        // errs() << "inst1:" <<*inst1<< '\n';
        APInt b;     
        //b=dyn_cast<Constant>(inst1->getOperand(1))->getUniqueInteger(); 
        if(dyn_cast<ConstantExpr>(inst1->getOperand(1))){
          ConstantExpr* exp=cast<ConstantExpr>(inst1->getOperand(1));
          ConstantInt *bb=(ConstantInt *)ConstantInt::get(exp->getType(),0);
          Constant* cons=exp->getWithOperandReplaced(0,bb);
          b=dyn_cast<Constant>(cons)->getUniqueInteger();
        }
        else{
          b=dyn_cast<Constant>(inst1->getOperand(1))->getUniqueInteger(); 
        }
        pre=inst1->getPredicate();
        APInt rlt=a1-b;      
        int c=rlt.getSExtValue();
        succ_iterator SI = succ_begin(blknow);
        BasicBlock *PI = *SI;
        BasicBlock *E=*(++SI);
        if(c>0){ //这里enum Predicate定义在llvm/InstrTypes.h，不同版本可能不一样     
          if(pre==33||pre==34||pre==35||pre==38||pre==39){branch=true;}
          else{branch=false;}  
        }
        else if(c==0){
          if(pre==32||pre==35||pre==37||pre==39||pre==41){branch=true;}
          else{branch=false;}  
        }
        else if(c<0){
          if(pre==33||pre==36||pre==37||pre==40||pre==41){branch=true;}
          else{branch=false;} 
        }        
        // errs() << "branch:" <<branch<< '\n';
        if(branch==true){
          // errs() << "PI:" <<**PI<< '\n';
          blknow=PI;
        }
        else{
          // errs() << "E:" <<**E<< '\n';
          blknow=E;
        }
        errs() << "block_pass:" <<blknow->getName()<< '\n';
        vector<BasicBlock*>::iterator it=find(blk.begin(), blk.end(), blknow);
        vector<BasicBlock*>::iterator itt=find(blk3.begin(), blk3.end(), blknow);
        if((it != blk.end())&&(itt == blk3.end())){
          excute.push_back(blknow);
          goto loop;
        }
        else{
          if(isa<ReturnInst>(blknow->getTerminator())){
            errs() << "return_get:" <<blknow->getTerminator()<< '\n';
            rtn=blknow;
            goto stop;
          }
          errs() << "block_end:" <<blknow->getName()<< '\n';
          vector<BasicBlock*>::iterator it2=find(blk3.begin(), blk3.end(), blknow);
          int pos=std::distance(blk3.begin(), it2);
          errs() << "phi_into:" <<pos<< '\n';
          a1=phinum[pos]->getUniqueInteger();
          errs() << "switch_var:" <<a1<< '\n';
          if(pos<phinum.size()){
            i+=1;  
          }
          else{
            goto stop;
          }
          blknow=(phi->getParent())->getSingleSuccessor();
          goto loop;
        }
        stop:    
          errs() << "cycle_count:" <<i<< '\n';      
          errs() << "cycle_end:" <<blknow->getName()<< '\n';
    //结束
        vector<BasicBlock*>excute_real;
        for(int i=0;i<excute.size();i++){
          // errs() <<"excute block:"<< excute[i]->getName()<< '\n';
          vector<BasicBlock*>::iterator it=find(blk_real.begin(), blk_real.end(),excute[i]);
          if(it != blk.end()){
            excute_real.push_back(excute[i]);
          }
        }
        excute_real.push_back(rtn); 
        for(int i=0;i<excute_real.size();i++){
          inst_iterator  E = inst_end(excute_real[i]);
          BranchInst* br=BranchInst::Create(excute_real[i+1],E);          
        }
      }
  };
}

char dflat::ID = 0;
static RegisterPass<dflat> X("dflat", "dflat Pass");

