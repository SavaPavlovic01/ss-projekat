#include "../inc/emulator.hpp"
#include <stdio.h>
#include <unistd.h>
#include <time.h>

int Emulator::term_in=0;
int Emulator::term_out=0;
int Emulator::tim_cfg=0;
bool Emulator::in=false;

void Emulator::initEmulator(FILE* file){
  for(int i=0;i<15;i++) regFile.push_back(0);
  regFile.push_back(0x40000000);

  for(int i=0;i<4;i++) sysRegs.push_back(0);

  unsigned int curAdr;
  while(fread(&curAdr,4,1,file)>0){
    int sz;
    fread(&sz,4,1,file);
    for(int i=0;i<sz;i++){
      char byte;
      fread(&byte,1,1,file);
      std::map<int,unsigned char*>::iterator itr=memory.find(curAdr/256);
      if(itr==memory.end()){
        // alociraj sekciju
        memory[curAdr/256]=(unsigned char*)malloc(sizeof(char)*256);
        memory[curAdr/256][curAdr%256]=byte;
        curAdr++;
        continue;
      }
      itr->second[curAdr%256]=byte;
      curAdr++;
    }
  }
}

void Emulator::error(instr* instruction){
  restoreTerminal();
  exit(0);
  regFile[14]-=4;
  writeInt(regFile[14],sysRegs[0]);//push status
  regFile[14]-=4;
  writeInt(regFile[14],regFile[15]);//push pc
  sysRegs[2]=1; //cause <- 4
  sysRegs[0]=sysRegs[0] & ~(0x1);
  regFile[15]=sysRegs[1];
}

void Emulator::printMemory(){
  std::map<int,unsigned char*>::iterator itr=memory.begin();
  for(;itr!=memory.end();itr++){
    printf("%d: ",itr->first);
    for(int i=0;i<256;i++){
      printf("%02x",itr->second[i]);
    }
    printf("\n");
  }
}

char Emulator::readByte(int adr){
  std::map<int,unsigned char*>::iterator itr=memory.find(adr/256);
  if(itr==memory.end()){
    memory[adr/256]=(unsigned char*) malloc(sizeof(char)*256);
    return memory[adr/256][adr%256];
  }
  return itr->second[adr%256];
}

int Emulator::readInt(unsigned int adr){
  std::vector<unsigned char> temp;
  unsigned int curAdr=adr;
  if(adr==0xffffff00){
    return term_out; 
  }
  if(adr==0xffffff04){
    return term_in;  
  }
  if(adr==0xFFFFFF10){
    return tim_cfg;
  }
  for(int i=0;i<4;i++){
    std::map<int,unsigned char*>::iterator itr=memory.find(curAdr/256);  
    if(itr==memory.end()){
      memory[curAdr/256]=(unsigned char*) malloc(sizeof(char)*256);
      temp.push_back(memory[curAdr/256][curAdr%256]);
      curAdr++;
      continue;
    }
    temp.push_back(itr->second[curAdr%256]);
    curAdr++;
  }

  int ret= (temp[3])<<24 |(temp[2])<<16 | (temp[1])<<8 | temp[0];
  return ret;
}

void Emulator::writeInt(unsigned int adr,int data){
 
  unsigned int curAdr=adr;
  if(adr==0xffffff00){
    term_out=data; 
    in=true;
    return;
  }
  if(adr==0xffffff04){
    term_in=data;
    return;
  }
  if(adr==0xFFFFFF10){
    tim_cfg=data;
    return;
  }
  for(int i=0;i<4;i++){
    unsigned char in=(data>>i*8) & 0x000000ff;
    std::map<int,unsigned char*>::iterator itr=memory.find(curAdr/256);  
    if(itr==memory.end()){
      memory[curAdr/256]=(unsigned char*) malloc(sizeof(char)*256);
      memory[curAdr/256][curAdr%256]=in;
      curAdr++;
      continue;
    }
    itr->second[curAdr%256]=in;
    curAdr++;
  }
}

int Emulator::readReg(int num){
  if(num==0) return 0;
  else return regFile[num];
}

void Emulator::writeReg(int num,int data){
  if(num==0) return;
  regFile[num]=data;
}

int Emulator::readInstruction(){
  int instr=readInt(regFile[15]);
  regFile[15]+=4;
  return instr;
}

instr* Emulator::parseInstr(int instruction){
  instr* ins=new instr();
  if((instruction & 0b100000000000)==0x800){
    ins->disp=((instruction & 0x00000fff) << 20)>>20;
  }else ins->disp=instruction & 0x00000fff;
  ins->c=instruction>>12 & 0x0000000f;
  ins->b=instruction>>16 & 0x0000000f;
  ins->a=instruction>>20 & 0x0000000f;
  ins->m=instruction>>24 & 0x0000000f;
  ins->oc=instruction>>28 & 0x0000000f;
  return ins;
}

void Emulator::executeXchg(instr* instruction){
  if(instruction->m==0){
    int temp=readReg(instruction->b);
    writeReg(instruction->b,readReg(instruction->c));
    writeReg(instruction->c,temp);
  }else error(instruction);
}

void Emulator::printRegs(){
  printf("-----------------------------------------------------------------\n");
  printf("Emulated processor state:\n");
  for(int i=0;i<16;i++){
    printf("r%d=0x%08x\t",i,regFile[i]);
    
    if((i+1)%4==0) printf("\n");
  }  
}

void Emulator::executeInstr(instr* instruction){
  switch(instruction->oc){
    case 0:
      executeHalt(instruction);
      break;
    case 1:
      executeIntr(instruction);
      break;
    case 2:
      executeCall(instruction);
      break;
    case 3:
      executeJmp(instruction);
      break;
    case 4:
      executeXchg(instruction);
      break;
    case 5:
      executeAri(instruction);
      break;
    case 6:
      executeBit(instruction);
      break;
    case 7:
      executeShift(instruction);
      break;
    case 8:
      executeSt(instruction);
      break;
    case 9:
      executeLd(instruction);
      break;
    default:
      error(instruction);
  }
}

void Emulator::executeHalt(instr* instruction){
  restoreTerminal();
  printf("-----------------------------------------------------------------\n");
  printf("Emulated processor executed halt instruction\n");
  printf("Emulated processor state:\n");
  for(int i=0;i<16;i++){
    printf("r%d=0x%08x\t",i,regFile[i]);
    
    if((i+1)%4==0) printf("\n");
  }
  
  exit(0);
}

void Emulator::executeIntr(instr* instruction){
  regFile[14]-=4;
  writeInt(regFile[14],sysRegs[0]);//push status
  regFile[14]-=4;
  writeInt(regFile[14],regFile[15]);//push pc
  sysRegs[2]=4; //cause <- 4
  sysRegs[0]=sysRegs[0] & ~(0x1);
  regFile[15]=sysRegs[1];
}

void Emulator::executeCall(instr* instruction){
  if(instruction->m==0){
    regFile[14]-=4;
    writeInt(regFile[14],regFile[15]);//push pc
    regFile[15]=readReg(instruction->a)+readReg(instruction->b)+instruction->disp;
  }
  if(instruction->m=1){
    regFile[14]-=4;
    writeInt(regFile[14],regFile[15]);//push pc
    regFile[15]=readInt(readReg(instruction->a)+readReg(instruction->b)+instruction->disp);  
  }
  else {
    error(instruction);
  }
}

void Emulator::executeJmp(instr* instruction){
  switch (instruction->m)
  {
  case 0:
    regFile[15]=readReg(instruction->a)+instruction->disp;
    break;
  
  case 1:
    if(readReg(instruction->b)==readReg(instruction->c)) regFile[15]=readReg(instruction->a)+instruction->disp;
    break;
  
  case 2:
    if(readReg(instruction->b)!=readReg(instruction->c)) regFile[15]=readReg(instruction->a)+instruction->disp;
    break;

  case 3:
    if(readReg(instruction->b)>readReg(instruction->c)) regFile[15]=readReg(instruction->a)+instruction->disp;
    break;
  
  case 8:
    regFile[15]=readInt(readReg(instruction->a)+instruction->disp);
    break;
  
  case 9:
    if(readReg(instruction->b)==readReg(instruction->c)) regFile[15]=readInt(readReg(instruction->a)+instruction->disp);
    break;

  case 10:
    if(readReg(instruction->b)!=readReg(instruction->c)) regFile[15]=readInt(readReg(instruction->a)+instruction->disp);
    break;
  
  case 11:
    if(readReg(instruction->b)>readReg(instruction->c)) regFile[15]=readInt(readReg(instruction->a)+instruction->disp);
    break;
  
  default:
    error(instruction);
  }
}

void Emulator::executeAri(instr* instruction){
  switch (instruction->m)
  {
  case 0:
    writeReg(instruction->a,readReg(instruction->b)+readReg(instruction->c));
    break;
  
  case 1:
    writeReg(instruction->a,readReg(instruction->b)-readReg(instruction->c));
    break;

  case 2:
    writeReg(instruction->a,readReg(instruction->b)*readReg(instruction->c));
    break;
  
  case 3:
    writeReg(instruction->a,readReg(instruction->b)/readReg(instruction->c));
    break;

  default:
    error(instruction);
    break;
  }
}

void Emulator::executeBit(instr* instruction){
  switch (instruction->m)
  {
  case 0:
    writeReg(instruction->a,~readReg(instruction->b));
    break;
  
  case 1:
    writeReg(instruction->a,readReg(instruction->b) & readReg(instruction->c));
    break;

  case 2:
    writeReg(instruction->a,readReg(instruction->b) | readReg(instruction->c));
    break;
  
  case 3:
    writeReg(instruction->a,readReg(instruction->b) ^ readReg(instruction->c));
    break;

  default:
    error(instruction);
    break;
  }  
}

void Emulator::executeShift(instr* instruction){
  if(instruction->m==0) {
    writeReg(instruction->a,readReg(instruction->b)<<readReg(instruction->c));
    return;
  }
  if(instruction->m==1) {
    writeReg(instruction->a,readReg(instruction->b)>>readReg(instruction->c));
    return;
  }
  error(instruction);
}

void Emulator::executeSt(instr* instruction){
  switch (instruction->m)
  {
  case 0:
    writeInt(readReg(instruction->a)+readReg(instruction->b)+instruction->disp,readReg(instruction->c));
    
    break;
  
  case 2:
    writeInt(readInt(readReg(instruction->a)+readReg(instruction->b)+instruction->disp),readReg(instruction->c));
    break;

  case 1:
    writeReg(instruction->a,readReg(instruction->a)+instruction->disp);
    writeInt(readReg(instruction->a),readReg(instruction->c));
    break;

  default:
    error(instruction);
    break;
  }
}

void Emulator::executeLd(instr* instruction){
  switch (instruction->m)
  {
  case 0:
    writeReg(instruction->a,sysRegs[instruction->b]);
    break;
  
  case 1:
    writeReg(instruction->a,readReg(instruction->b)+instruction->disp);
    break;
  
  case 2:
    writeReg(instruction->a,readInt(readReg(instruction->b)+readReg(instruction->c)+instruction->disp));
    break;
  
  case 3:
    writeReg(instruction->a,readInt(readReg(instruction->b)));
    writeReg(instruction->b,readReg(instruction->b)+instruction->disp);
    break;

  case 4:
    sysRegs[instruction->a]=readReg(instruction->b);
    break;

  case 5:
    sysRegs[instruction->a]=sysRegs[instruction->b] | (instruction->disp & 0xfff);
    break;

  case 6:
    sysRegs[instruction->a]=readInt(readReg(instruction->b)+readReg(instruction->c)+instruction->disp);
    break;

  case 7:
    sysRegs[instruction->a]=readInt(readReg(instruction->b));
    writeReg(instruction->b,readReg(instruction->b)+instruction->disp);
    break;

  default:
    error(instruction);
    break;
  }
}

void* Emulator::readTerminalThread(void* arg){
  Emulator* emu=(Emulator*) arg;
  unsigned char key;
  while(true){
    while(read(0,&key,1)==0);
    if(key=='k') {emu->restoreTerminal();emu->printRegs();exit(0);}
    //printf("%c",key);
    //fflush(0);
    Emulator::term_in=key;
    emu->interrupt=1;
    emu->sysRegs[2]=3;
  }
}

void Emulator::checkInter(){
  //printf("Cause: %d",sysRegs[0]);
  //printf("Status: %d",sysRegs[2]);
  if(interrupt==0) return;
  if(sysRegs[0]&4==4) return;
  
  if(sysRegs[2]==3 && ((sysRegs[0] & 2)==0)){
    regFile[14]-=4;
    writeInt(regFile[14],sysRegs[0]);//push status
    regFile[14]-=4;
    writeInt(regFile[14],regFile[15]);//push pc
    sysRegs[2]=3; 
    interrupt=0;
    sysRegs[0]=sysRegs[0] & 0b0000;
    regFile[15]=sysRegs[1];  
  }

  if(sysRegs[2]==2 && ((sysRegs[0] & 1)==0)){
    regFile[14]-=4;
    writeInt(regFile[14],sysRegs[0]);//push status
    regFile[14]-=4;
    writeInt(regFile[14],regFile[15]);//push pc
    sysRegs[2]=2; 
    interrupt=0;
    sysRegs[0]=sysRegs[0] & 0b0000;
    regFile[15]=sysRegs[1];    
  }
}

void* Emulator::writeTerminalThread(void*){
 
  while(true){
    while(!in);
    printf("%c",term_out);
    fflush(0);
    in=false;
  }
}

void* Emulator::myTimer(void* arg){
  Emulator* emu=(Emulator*) arg;
  long msec=0;
  long sec=0;
  while(true){
    
    sec=0;
    struct timespec ts;
    switch (tim_cfg)
    {
    case 0:
      msec=500;
      break;
    case 1:
      msec=0;
      sec=1;
      break;
    case 2:
      msec=500;
      sec=1;
      break;
    case 3:
      msec=0;
      sec=2;
      break;
    case 4:
      msec=0;
      sec=5;
      break;
    case 5:
      msec=0;
      sec=10;
      break;
    case 6:
      msec=0;
      sec=30;
      break;
    case 7:
      msec=0;
      sec=60;
      break;
    default:
      msec=0;
      sec=0;
      break;
    }
    ts.tv_sec=sec;
    ts.tv_nsec=msec*1000000;
    nanosleep(&ts,&ts);
    emu->interrupt=1;
    emu->sysRegs[2]=2;
  }
}

void Emulator::initTerminal(){
  tcgetattr(0,&old);
  //cfmakeraw(&current);
  current.c_lflag &= ~ICANON;
  current.c_lflag &= ~ECHO;
  tcsetattr(0,TCSANOW,&current);  
}

void Emulator::restoreTerminal(){
  tcsetattr(0,TCSANOW,&old);  
}