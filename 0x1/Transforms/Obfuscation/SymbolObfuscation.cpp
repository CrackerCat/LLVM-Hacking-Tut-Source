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
#include "llvm/Transforms/Obfuscation/SymbolObfuscation.h"
#include <string>
#include <iostream>
#include <cstdlib>
using namespace llvm;
using namespace std;
static string obfcharacters="qwertyuiopasdfghjklzxcvbnm1234567890";
const char *OCNAMEPLACEHOLDERS[] = {"OBJC_CLASS_$_", "OBJC_METACLASS_$_",
                                    "\01l_OBJC_CLASS_RO_$_","\01l_OBJC_$_CLASS_METHODS_",
                                    "\01l_OBJC_METACLASS_RO_$_"
                                  };
namespace llvm{
  struct SymbolObfuscation : public ModulePass {
    static char ID;
    SymbolObfuscation() : ModulePass(ID) {}
    string randomString(int length){
      string name;
      name.resize(length);
      for(int i=0;i<length;i++){
        name[i]=obfcharacters[rand()%(obfcharacters.length()+1)];
      }
      return name;
    }
    bool runOnModule(Module &M) override {
      errs()<<"Do not go gentle into that good night\n";
      srand (time(NULL));
      //Objective-C Method ClassName Stuff
      
      //This was legacy code for testing 
      //Now I'm discarding ObjectiveC SELs in general and dynamically create methods
      /*for(auto G=M.getGlobalList().begin();G!=M.getGlobalList().end();G++){
        GlobalVariable &GL=*G;
        if (GL.getName().str().find("OBJC_METH_VAR_NAME_")==0||GL.getName().str().find("OBJC_CLASS_NAME_")==0){
          ConstantDataArray* Initializer=dyn_cast<ConstantDataArray>(GL.getInitializer ());
          string newName=randomString(16);
          errs()<<"Selector Name"<<Initializer->getAsCString ()<<" Obfuscated To: "<<newName<<"\n";
          Constant *Value =ConstantDataArray::getString(Initializer->getContext(),newName,true);
          //GL.setInitializer(Value);
          Initializer->replaceAllUsesWith(Value);
        }
      }
*/
      //Normal Symbols
      for(Module::iterator Fun=M.begin();Fun!=M.end();Fun++){
        Function &F=*Fun;
        if (F.getName().str().compare("main")==0){
          errs()<<"Skipping main\n";
        }
        else if(F.empty()==false){
          //Rename
          errs()<<"Renaming Function: "<<F.getName()<<"\n";
          F.setName(randomString(16));
        }
        else{
          errs()<<"Skipping External Function: "<<F.getName()<<"\n";
        }
      }
      return true;
    }
  };
  Pass * createSymbolObf() {return new SymbolObfuscation();}
}

char SymbolObfuscation::ID = 0;
