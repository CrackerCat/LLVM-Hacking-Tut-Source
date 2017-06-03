/*
 *  LLVM SymbolObfuscation Pass
 *  https://github.com/Naville
 *  GPL V3 Licensed
 *  This is designed to be a LTO pass so we have a global view of all the TUs
 *
 */

#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Obfuscation/SymbolObfuscation.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <string>
#include <iostream>
#include <cstdlib>
using namespace llvm;
using namespace std;
static string obfcharacters="qwertyuiopasdfghjklzxcvbnm1234567890";
const char *OCNAMEPLACEHOLDERS[] = {"OBJC_CLASS_$_", "OBJC_METACLASS_$_",
                                    "\01l_OBJC_CLASS_RO_$_","\01l_OBJC_$_CLASS_METHODS_",
                                    "\01l_OBJC_METACLASS_RO_$_"};
namespace llvm {
struct SymbolObfuscation : public ModulePass {
        static char ID;
        SymbolObfuscation() : ModulePass(ID) {
        }
        string randomString(int length){
                string name;
                name.resize(length);
                for(int i=0; i<length; i++) {
                        name[i]=obfcharacters[rand()%(obfcharacters.length()+1)];
                }
                return name;
        }
        bool runOnModule(Module &M) override {
                errs()<<"Do not go gentle into that good night\n";
                //Iterate all CLASSREF uses and replace with objc_getClass() call
                //Strings are encrypted in other passes
                for(auto G=M.global_begin(); G!=M.global_end(); G++) {
                        GlobalVariable &GV=*G;
                        if (GV.getName().str().find("OBJC_CLASSLIST_REFERENCES")==0) {
                                if(GV.hasInitializer()) {
                                        string className=GV.getInitializer ()->getName();
                                        className.replace(className.find("OBJC_CLASS_$_"),strlen("OBJC_CLASS_$_"),"");
                                        for(auto U=GV.user_begin (); U!=GV.user_end(); U++) {
                                                if (Instruction* I = dyn_cast<Instruction>(*U)) {
                                                        IRBuilder<> builder(I);
                                                        FunctionType *objc_getClass_type =FunctionType::get(I->getType(), {Type::getInt8PtrTy(M.getContext())}, false);
                                                        Function *objc_getClass_Func = cast<Function>(M.getOrInsertFunction("objc_getClass", objc_getClass_type ) );
                                                        Value* newClassName=builder.CreateGlobalStringPtr(StringRef(className));
                                                        CallInst* CI=builder.CreateCall(objc_getClass_Func,{newClassName});
                                                        I->replaceAllUsesWith(CI);
                                                        I->eraseFromParent ();
                                                }
                                        }
                                }
                        }
                        //Selector Convert
                        else if (GV.getName().str().find("OBJC_SELECTOR_REFERENCES")==0) {
                                if(GV.hasInitializer()) {
                                        ConstantExpr* CE=dyn_cast<ConstantExpr>(GV.getInitializer());
                                        Constant* C=CE->getOperand(0);
                                        GlobalVariable* SELNameGV=dyn_cast<GlobalVariable>(C);
                                        ConstantDataArray* CDA=dyn_cast<ConstantDataArray>(SELNameGV->getInitializer());
                                        StringRef SELName=CDA->getAsString();//This is REAL Selector Name
                                        for(auto U=GV.user_begin (); U!=GV.user_end(); U++) {
                                                if (Instruction* I = dyn_cast<Instruction>(*U)) {
                                                        IRBuilder<> builder(I);
                                                        FunctionType *sel_registerName_type =FunctionType::get(I->getType(), {Type::getInt8PtrTy(M.getContext())}, false);
                                                        Function *sel_registerName_Func = cast<Function>(M.getOrInsertFunction("sel_registerName",sel_registerName_type) );
                                                        Value* newGlobalSELName=builder.CreateGlobalStringPtr(SELName);
                                                        CallInst* CI=builder.CreateCall(sel_registerName_Func,{newGlobalSELName});
                                                        I->replaceAllUsesWith(CI);
                                                        I->eraseFromParent ();
                                                }
                                        }
                                }
                        }
                }
                return false;
        }
};
Pass * createSymbolObf() {
        return new SymbolObfuscation();
}
}

char SymbolObfuscation::ID = 0;
