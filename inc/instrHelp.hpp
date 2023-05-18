#ifndef _instrHelp_hpp_
#define _instrHelp_hpp_
#include "instruction.h"
#include "codeGen.hpp"
#include <map>
#include <string>

typedef generated* (*codeGenerator)(context*);

/* argType 
* -1 nema argument 
* 0 operand(sve moze) 
* 1 gpr
* 2 sistemski reg 
* 3 simbol 
* 4 literal 
* 5 simbol ili literal
* 6 za skokove
*/
typedef struct instrInfo{
  int argCnt;
  int type;
  int size;
  int arg1Type;
  int arg2Type;
  int arg3Type;
  codeGenerator generateCode;
  instrInfo(int argCnt,int type,int size,int arg1Type,int arg2Type,int arg3Type,codeGenerator generateCode):argCnt(argCnt),type(type),size(size)
    ,arg1Type(arg1Type),arg2Type(arg2Type),arg3Type(arg3Type),generateCode(generateCode) {}
} instrInfo;

class instrHelp{

  
  
    

  public:

    static std::map<std::string,instrInfo*> instrMap;

    static int getInstrType(char* name);

    static bool isValid(instruction* instr);

    static int getInstrArgNum(char* name);

    static char* getSymbolsFromArg(argument* arg);

    static instrInfo* getAllInstrInfro(char* name);

    static int getInstrSize(instruction* instr);

    static int calcSize(instruction* instr);

    static generated* getCode(context*);

    static void initInstr();
};

#endif