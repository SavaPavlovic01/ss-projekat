#ifndef _Emulator_hpp_
#define _Emulator_hpp_
#include <map>
#include <vector>

typedef struct instr{
  int oc;
  int m;
  int a;
  int b;
  int c;
  int disp;

  instr(){}

} instr;

// jedan segment 256B=2^8B 2^32/2^8=2^24
// adresa x-> x/256=broj segmenta
// x%256=pomeraj unutar sekcije 

class Emulator{
  std::map<int,unsigned char*> memory;
  std::vector<int> regFile;
  std::vector<int> sysRegs;

  public:

    void initEmulator(FILE* file);

    void printMemory();

    void error(instr*);

    void printRegs();

    char readByte(int adr);

    int readInt(int adr);

    int readReg(int num);

    void writeReg(int num,int data);

    void writeInt(int adr,int data);

    int readInstruction();

    void executeInstr(instr*);

    instr* parseInstr(int instr);

    void executeHalt(instr*);

    void executeIntr(instr*);

    void executeCall(instr*);

    void executeJmp(instr*);

    void executeXchg(instr*);

    void executeAri(instr*);

    void executeBit(instr*);

    void executeShift(instr*);

    void executeSt(instr*);

    void executeLd(instr*);

};

#endif