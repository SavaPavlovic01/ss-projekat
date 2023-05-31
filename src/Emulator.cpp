#include "../inc/emulator.hpp"

void Emulator::initEmulator(FILE* file){
  for(int i=0;i<15;i++) regFile.push_back(0);
  regFile.push_back(0x40000000);

  for(int i=0;i<4;i++) sysRegs.push_back(0);

  int curAdr;
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

int Emulator::readInt(int adr){
  std::vector<unsigned char> temp;
  int curAdr=adr;
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

void Emulator::writeInt(int adr,int data){
 
  int curAdr=adr;
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
    printf("STORE NA ADRESI %x UPISAO %x",readReg(instruction->a)+readReg(instruction->b)+instruction->disp,readInt(readReg(instruction->a)+readReg(instruction->b)+instruction->disp));
    break;
  
  case 2:
    writeInt(readInt(readReg(instruction->a)+readReg(instruction->b)+instruction->disp),readReg(instruction->c));
    printf("STORE NA ADRESI %x UPISAO %x",readInt(readReg(instruction->a)+readReg(instruction->b)+instruction->disp),readInt(readInt(readReg(instruction->a)+readReg(instruction->b)+instruction->disp)));
    break;

  case 1:
    writeReg(instruction->a,readReg(instruction->a)+instruction->disp);
    writeInt(readReg(instruction->a),readReg(instruction->c));
    printf("STORE NA ADRESI %x UPISAO %x",readReg(instruction->a),readInt(readReg(instruction->a)));
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