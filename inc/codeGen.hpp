#ifndef _codeGen_hpp_
#define _codeGen_hpp_

#include "instruction.h"
#include "sectionTable.hpp"
#include "symbTable.hpp"
#include "relocTable.hpp"

typedef struct generated{
  int code;
  generated* next;

  generated(int code,generated* next):code(code),next(next) {}
}generated;

typedef struct context{
  sectionTable* secTable;
  symbTable* symbtable;
  instruction* instr;
  int* count;
  relocTable* rTable;
  context(sectionTable* secTable,symbTable* symbtable,instruction* instr,int* count,relocTable* rTable):secTable(secTable),symbtable(symbtable),
    instr(instr),count(count),rTable(rTable) {}
}context;

class codeGen{

  public:
    static int generate(int opcode,int m, int a,int b,int c,int d);

    static generated* add(context*);

    static generated* sub(context*);

    static generated* mul(context*);

    static generated* div(context*);

    static generated* bitNot(context*);

    static generated* bitAnd(context*);

    static generated* bitOr(context*);

    static generated* bitXor(context*);

    static generated* shl(context*);
    
    static generated* shr(context*);

    static generated* halt(context*);

    static generated* interupt(context*);

    static generated* push(context*);

    static generated* pop(context*);

    static generated* iret(context*);

    static generated* call(context*);

    static generated* ret(context*);  

    static generated* jmp(context*);  

    static generated* beq(context*);

    static generated* bne(context*);

    static generated* bgt(context*);

    static generated* xchg(context*);

    static generated* ld(context*);

    static generated* st(context*);

    static generated* csrrd(context*);

    static generated* csrwr(context*);
};

#endif