#ifndef _Emulator_hpp_
#define _Emulator_hpp_
#include <map>
#include <vector>
#include <termios.h>

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
  static int term_out;
  static int term_in;

  static int tim_cfg;

  struct termios old;
  struct termios current;

  static bool in;
  
  int interrupt=0;
  int interTimer=0;
  int interTerm=0;
  public:

    void initTerminal();

    void restoreTerminal();

    void checkInter();

    static void* readTerminalThread(void*);

    static void* writeTerminalThread(void*);

    static void* myTimer(void*);

    void initEmulator(FILE* file);

    void printMemory();

    void error(instr*);

    void printRegs();

    char readByte(unsigned int adr);

    int readInt(unsigned int adr);

    int readReg(int num);

    void writeReg(int num,int data);

    void writeSysReg(int num,int data){sysRegs[num]=data;}

    void writeInt(unsigned int adr,int data);

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