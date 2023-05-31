#include "../inc/emulator.hpp"

int main(int argc,char** argv){
  FILE* file=fopen(argv[1],"rb");
  if(!file){
    printf("Cant find file %s",argv[1]);
    exit(0);
  }
  Emulator emu;
  emu.initEmulator(file);

  emu.printMemory();
  
  while(true){
    int inst=emu.readInstruction();
    printf("%x\n",inst);
    
    instr* cur=emu.parseInstr(inst);
    //printf("%d %d %d %d %d %d",cur->oc,cur->m,cur->a,cur->b,cur->c,cur->disp);
    emu.executeInstr(cur);
    //emu.printRegs();
  }

}