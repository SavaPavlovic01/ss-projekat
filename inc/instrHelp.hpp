#ifndef _instrHelp_hpp_
#define _instrHelp_hpp_
#include "instruction.h"
#include <map>
#include <string>

typedef struct instrInfo{
  int argCnt;
  int type;
  int size;
  instrInfo(int argCnt,int type,int size):argCnt(argCnt),type(type),size(size) {}
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

    static void initInstr();
};

#endif