#include "../inc/emulator.hpp"
#include <termios.h>
#include <pthread.h>

int main(int argc,char** argv){
  FILE* file=fopen(argv[1],"rb");
  if(!file){
    printf("Cant find file %s",argv[1]);
    exit(0);
  }

  Emulator emu;
  emu.initEmulator(file);
  emu.initTerminal();
  //emu.printMemory();
  pthread_t outThread;
  pthread_t inThread;
  pthread_t timerThread;
  pthread_create(&outThread,NULL,&(Emulator::writeTerminalThread),(void*)&emu);
  pthread_create(&inThread,NULL,&(Emulator::readTerminalThread),(void*)&emu);
  pthread_create(&timerThread,NULL,&(Emulator::myTimer),(void*)&emu);
  while(true){
    int inst=emu.readInstruction();
    //printf("%x\n",inst);
    
    instr* cur=emu.parseInstr(inst);
    //printf("%d %d %d %d %d %d",cur->oc,cur->m,cur->a,cur->b,cur->c,cur->disp);
    emu.executeInstr(cur);
    emu.checkInter();
    //emu.printRegs();
  }

}