#ifndef _Events_hpp_
#define _Events_hpp_

#include "sectionTable.hpp"
#include "symbTable.hpp"
#include "instruction.h"

class Events{


  public:
    static void sectionFP(sectionTable* secTable,symbTable* symbTable,char* name,int* adrCnt);

    static void equFP(sectionTable* secTable,symbTable* symbTable,argument* arg);

    static void globalFP(symbTable* table,argument* arg);
    
    static void externFP(symbTable* table,char* name);

    static void endFP(sectionTable* secTable,int cnt);

    static void sectionSP(char* name,int* cnt);

    static void wordSP(sectionTable* secTable,symbTable* symbTable,instruction* instr,int* cnt);

    static void skipSP(sectionTable* secTable,instruction* instr);

    static void asciiSP(sectionTable* secTable,instruction* instr);
};

#endif