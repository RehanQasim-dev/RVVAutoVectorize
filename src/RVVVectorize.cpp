#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Loop.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IntrinsicsRISCV.h"

using namespace llvm;

namespace {
struct RVVVectorize : public LoopPass {
    static char ID;
    RVVVectorize() : LoopPass(ID) {}

    bool runOnLoop(Loop *L, LPPassManager &LPM) override {
        if (!L->isLoopSimplifyForm() || !L->getSubLoops().empty()) return false;

        DependenceInfo &DI = getAnalysis<DependenceInfoWrapperPass>().getDI();
        bool canVectorize = true;
        for (auto *BB : L->blocks()) {
            for (auto &I : *BB) {
                if (auto *Store = dyn_cast<StoreInst>(&I)) {
                    for (auto *User : Store->users()) {
                        if (DI.depends(&I, User, true)) {
                            canVectorize = false; 
                            break;
                        }
                    }
                }
                if (!canVectorize) break;
            }
        }

        if (!canVectorize) return false;

        BasicBlock *Body = L->getHeader();
        Module *M = L->getHeader()->getParent()->getParent();
        LLVMContext &Ctx = M->getContext();
        IRBuilder<> Builder(Body->getFirstNonPHI());

        Type *VecTy = VectorType::get(Type::getInt32Ty(Ctx), 4, false); 
        Function *VLoad = Intrinsic::getDeclaration(M, Intrinsic::riscv_vload, {VecTy});
        Function *VAdd = Intrinsic::getDeclaration(M, Intrinsic::riscv_vadd, {VecTy});
        Function *VStore = Intrinsic::getDeclaration(M, Intrinsic::riscv_vstore, {VecTy});

        for (auto &I : *Body) {
            if (auto *Store = dyn_cast<StoreInst>(&I)) {
                Value *PtrA = Store->getPointerOperand();
                Value *ScalarVal = Store->getValueOperand();
                if (auto *Add = dyn_cast<BinaryOperator>(ScalarVal)) {
                    Value *PtrB = Add->getOperand(1); 
                    Value *VecA = Builder.CreateCall(VLoad, {PtrA});
                    Value *VecB = Builder.CreateCall(VLoad, {PtrB});
                    Value *VecSum = Builder.CreateCall(VAdd, {VecA, VecB, Builder.getInt64(4)});
                    Builder.CreateCall(VStore, {VecSum, PtrA});
                    Store->eraseFromParent();
                    break;  
                }
            }
        }

        if (Instruction *Term = L->getLoopLatch()->getTerminator()) {
            if (auto *Branch = dyn_cast<BranchInst>(Term)) {
                if (Value *IndVar = L->getCanonicalInductionVariable()) {
                    Instruction *Inc = cast<Instruction>(IndVar->user_back());
                    Inc->setOperand(0, ConstantInt::get(Type::getInt32Ty(Ctx), 4)); 
                }
            }
        }

        return true;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
        AU.addRequired<LoopInfoWrapperPass>();
        AU.addRequired<DependenceInfoWrapperPass>();
        AU.setPreservesCFG();
    }
};
}

char RVVVectorize::ID = 0;
static RegisterPass<RVVVectorize> X("rvv-vectorize", "Auto-Vectorize Loops for RISC-V RVV", false, false);