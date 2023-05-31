#include "../inc/codeGen.hpp"

int codeGen::generate(int opcode,int m,int a,int b,int c,int d){
  int ret=0;
  ret=ret | (opcode<<28) | (m<<24) | (a<<20) | (b<<16) | (c<<12) | (d&0x000000000FFF);//proveri
  return ret;
}

generated* codeGen::add(context* context){
  instruction* instr=context->instr;
  int source=instr->arg1->val1;
  int dest=instr->arg1->next->val1;
  int ret= 0b01010000000000000000000000000000 | (dest<<20) | (dest<<16) | (source<<12);
  return new generated(ret,nullptr);
}

generated* codeGen::sub(context* context){
  instruction* instr=context->instr;
  int source=instr->arg1->val1;
  int dest=instr->arg1->next->val1;
  int ret= 0b01010001000000000000000000000000 | (dest<<20) | (dest<<16) | (source<<12);
  return new generated(ret,nullptr);
}

generated* codeGen::mul(context* context){
  instruction* instr=context->instr;
  int source=instr->arg1->val1;
  int dest=instr->arg1->next->val1;
  int ret= 0b01010010000000000000000000000000 | (dest<<20) | (dest<<16) | (source<<12);
  return new generated(ret,nullptr);
}

generated* codeGen::div(context* context){
  instruction* instr=context->instr;
  int source=instr->arg1->val1;
  int dest=instr->arg1->next->val1;
  int ret= 0b01010011000000000000000000000000 | (dest<<20) | (dest<<16) | (source<<12);
  return new generated(ret,nullptr);
}

generated* codeGen::bitAnd(context* context){
  instruction* instr=context->instr;
  int source=instr->arg1->val1;
  int dest=instr->arg1->next->val1;
  int ret= 0b01100001000000000000000000000000 | (dest<<20) | (dest<<16) | (source<<12);
  return new generated(ret,nullptr);  
}

generated* codeGen::bitOr(context* context){
  instruction* instr=context->instr;
  int source=instr->arg1->val1;
  int dest=instr->arg1->next->val1;
  int ret= 0b01100010000000000000000000000000 | (dest<<20) | (dest<<16) | (source<<12);
  return new generated(ret,nullptr);  
}

generated* codeGen::bitXor(context* context){
  instruction* instr=context->instr;
  int source=instr->arg1->val1;
  int dest=instr->arg1->next->val1;
  int ret= 0b01100011000000000000000000000000 | (dest<<20) | (dest<<16) | (source<<12);
  return new generated(ret,nullptr);  
}

generated* codeGen::bitNot(context* context){
  instruction* instr=context->instr;
  int source=instr->arg1->val1;
  int ret= 0b01100000000000000000000000000000 | (source<<20) | (source<<16);
  return new generated(ret,nullptr);  
}

generated* codeGen::shl(context* context){
  instruction* instr=context->instr;
  int source=instr->arg1->val1;
  int dest=instr->arg1->next->val1;
  int ret= 0b01110000000000000000000000000000 | (dest<<20) | (dest<<16) | (source<<12);
  return new generated(ret,nullptr);  
}

generated* codeGen::shr(context* context){
  instruction* instr=context->instr;
  int source=instr->arg1->val1;
  int dest=instr->arg1->next->val1;
  int ret= 0b01110001000000000000000000000000 | (dest<<20) | (dest<<16) | (source<<12);
  return new generated(ret,nullptr);  
}

generated* codeGen::halt(context* context){
  return new generated(0,nullptr);
}

generated* codeGen::interupt(context* context){
  return new generated(0x10000000,nullptr);
}

generated* codeGen::xchg(context* context){
  instruction* instr=context->instr;
  int source=instr->arg1->val1;
  int dest=instr->arg1->next->val1;
  int ret= 0b01000000000000000000000000000000 | (dest<<16) | (source<<12);
  return new generated(ret,nullptr);  
}

generated* codeGen::csrrd(context* context){
  instruction* instr=context->instr;
  return new generated(generate(0b1001,0,instr->arg1->next->val1,instr->arg1->val1,0,0),nullptr);
}

generated* codeGen::csrwr(context* context){
  instruction* instr=context->instr;
  return new generated(generate(0b1001,0b0100,instr->arg1->next->val1,instr->arg1->val1,0,0),nullptr);  
}

generated* codeGen::push(context* context){
  instruction* instr=context->instr;
  int reg=instr->arg1->val1;
  int ret= 0b10000001000000000000000000000000 | (14<<20) |  (reg<<12) | 0xffc;
  return new generated(ret,nullptr); 
}

generated* codeGen::pop(context* context){
  instruction* instr=context->instr;
  int reg=instr->arg1->val1;
  int ret= 0b10010011000000000000000000000000 | (reg<<20) |(14<<16) | 4;
  return new generated(ret,nullptr); 
}

generated* codeGen::bgt(context* context){
  instruction* instr=context->instr;
  argument* arg=instr->arg1->next->next;
  if(arg->type==1){
    // literal
    if(arg->mode==2){
      
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(arg->val1)-*(context->count)-4;// -4 jer pc pokazuje na sledecu 
      generated* ret=new generated(generate(0b0011,0b1011,15,instr->arg1->val1,instr->arg1->next->val1,pcOffset),nullptr); //ld [pc + offset do bazena],%pc

    }
  }
  if(arg->type==2){
    if(arg->mode==2){
      // ovo moze da se svede na jednu instrukciju ako ostajes u istoj sekciji
      if(context->secTable->getSectionId(sectionTable::curSection)==context->symbtable->getSection(arg->name)){
        int offset=context->symbtable->getValue(arg->name)-*(context->count)-4;
        if(offset<=1<<12) return new generated(generate(0b0011,0b0011,15,instr->arg1->val1,instr->arg1->next->val1,offset),nullptr);
      }
      
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(arg->name)-*(context->count)-4;// -4 jer pc pokazuje na sledecu 
      generated* ret=new generated(generate(0b0011,0b1011,15,instr->arg1->val1,instr->arg1->next->val1,pcOffset),nullptr); //ld [pc + offset do bazena],%pc
      
      int off=context->secTable->getLPool(sectionTable::curSection)->getAdr(arg->name);
      int section=context->symbtable->getSection(arg->name);
      int addend=context->symbtable->getValue(arg->name);
      if(section==-2) context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,instr->arg1->name,addend);
      else context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,section,addend);
      return ret;
    }
  }
}

generated* codeGen::bne(context* context){
  instruction* instr=context->instr;
  argument* arg=instr->arg1->next->next;
  if(arg->type==1){
    // literal
    if(arg->mode==2){
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(arg->val1)-*(context->count)-4;// -4 jer pc pokazuje na sledecu 
      generated* ret=new generated(generate(0b0011,0b1010,15,instr->arg1->val1,instr->arg1->next->val1,pcOffset),nullptr);  
    }
  }
  if(arg->type==2){
    if(arg->mode==2){
      // ovo moze da se svede na jednu instrukciju ako ostajes u istoj sekciji
      if(context->secTable->getSectionId(sectionTable::curSection)==context->symbtable->getSection(arg->name)){
        int offset=context->symbtable->getValue(arg->name)-*(context->count)-4;
        if(offset<=1<<12) return new generated(generate(0b0011,0b0010,15,instr->arg1->val1,instr->arg1->next->val1,offset),nullptr);
      }
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(arg->name)-*(context->count)-4;// -4 jer pc pokazuje na sledecu 
      generated* ret=new generated(generate(0b0011,0b1010,15,instr->arg1->val1,instr->arg1->next->val1,pcOffset),nullptr); //ld [pc + offset do bazena],%pc
      
      int off=context->secTable->getLPool(sectionTable::curSection)->getAdr(arg->name);
      int section=context->symbtable->getSection(arg->name);
      int addend=context->symbtable->getValue(arg->name);
      if(section==-2) context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,instr->arg1->name,addend);
      else context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,section,addend);
      return ret;
    }
  }  
}

generated* codeGen::beq(context* context){
  instruction* instr=context->instr;
  argument* arg=instr->arg1->next->next;
  if(arg->type==1){
    // literal
    if(arg->mode==2){
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(arg->val1)-*(context->count)-4;// -4 jer pc pokazuje na sledecu 
      generated* ret=new generated(generate(0b0011,0b1001,15,instr->arg1->val1,instr->arg1->next->val1,pcOffset),nullptr);  
      // relokacioni zapis 
    }
  }
  if(arg->type==2){
    if(arg->mode==2){
      // ovo moze da se svede na jednu instrukciju ako ostajes u istoj sekciji
      if(context->secTable->getSectionId(sectionTable::curSection)==context->symbtable->getSection(arg->name)){
        int offset=context->symbtable->getValue(arg->name)-*(context->count)-4;
        if(offset<=1<<12)return new generated(generate(0b0011,0b0001,15,instr->arg1->val1,instr->arg1->next->val1,offset),nullptr);
      }
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(arg->name)-*(context->count)-4;// -4 jer pc pokazuje na sledecu 
      generated* ret=new generated(generate(0b0011,0b1001,15,instr->arg1->val1,instr->arg1->next->val1,pcOffset),nullptr); //ld [pc + offset do bazena],%pc
      
      int off=context->secTable->getLPool(sectionTable::curSection)->getAdr(arg->name);
      int section=context->symbtable->getSection(arg->name);
      int addend=context->symbtable->getValue(arg->name);
      if(section==-2) context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,instr->arg1->name,addend);
      else context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,section,addend);
      return ret;
    }
  }
}

generated* codeGen::call(context* context){
  instruction* instr=context->instr;
  argument* arg=instr->arg1;
  if(arg->type==1){
    // literal
    if(arg->mode==2){
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(arg->val1)-*(context->count)-4;// -4 jer pc pokazuje na sledecu 
      generated* ret=new generated(generate(0b0010,0b0001,15,0,0,pcOffset),nullptr);  
      // relokacioni zapis
    }
  }
  if(arg->type==2){
    if(arg->mode==2){
      // ovo moze da se svede na jednu instrukciju ako ostajes u istoj sekciji
      if(context->secTable->getSectionId(sectionTable::curSection)==context->symbtable->getSection(arg->name)){
        int offset=context->symbtable->getValue(arg->name)-*(context->count)-4;
        if(offset<=1<<12) return new generated(generate(0b0010,0b0000,15,0,0,offset),nullptr);
      }
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(arg->name)-*(context->count)-4;// -4 jer pc pokazuje na sledecu 
      generated* ret=new generated(generate(0b0010,0b0001,15,0,0,pcOffset),nullptr); //ld [pc + offset do bazena],%pc
      
      int off=context->secTable->getLPool(sectionTable::curSection)->getAdr(arg->name);
      int section=context->symbtable->getSection(arg->name);
      int addend=context->symbtable->getValue(arg->name);
      if(section==-2) context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,instr->arg1->name,addend);
      else context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,section,addend);
      return ret;
    }
  }  
}

generated* codeGen::ret(context* context){
  return new generated(generate(0b1001,0b0011,15,14,0,4),nullptr);
}

generated* codeGen::iret(context* context){
  generated* head=new generated(generate(0b1001,0b0110,0,14,0,4),nullptr);
  head->next=new generated(generate(0b1001,0b0011,15,14,0,8),nullptr);
  return head;
}

generated* codeGen::jmp(context* context){
  instruction* instr=context->instr;
  argument* arg=instr->arg1;
  if(arg->type==1){
    // literal
    if(arg->mode==2){
      
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(instr->arg1->val1)-*(context->count)-4;// -4 jer pc pokazuje na sledecu 
      generated* ret=new generated(generate(0b1001,0b0010,15,15,0,pcOffset),nullptr); //ld [pc + offset do bazena],%pc

    }
  }
  if(arg->type==2){
    if(arg->mode==2){
      // ovo moze da se svede na jednu instrukciju ako ostajes u istoj sekciji
      if(context->secTable->getSectionId(sectionTable::curSection)==context->symbtable->getSection(arg->name)){
        int offset=context->symbtable->getValue(arg->name)-*(context->count)-4;
        return new generated(generate(0b0011,0b0000,15,0,0,offset),nullptr);
      }
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(instr->arg1->name)-*(context->count)-4;// -4 jer pc pokazuje na sledecu 
      generated* ret=new generated(generate(0b1001,0b0010,15,15,0,pcOffset),nullptr); //ld [pc + offset do bazena],%pc
      // takodje treba relokacioni zapis da prepravi vrednost u bazenu
      int off=context->secTable->getLPool(sectionTable::curSection)->getAdr(instr->arg1->name);
      int section=context->symbtable->getSection(instr->arg1->name);
      int addend=context->symbtable->getValue(instr->arg1->name);
      if(section==-2) context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,instr->arg1->name,addend);
      else context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,section,addend);
      return ret;
      //return new generated(generate(0b0011,0b0000,15,0,0,0),nullptr);
      // treba relokacioni zapis (ova zadnja nula mora da se promeni) 
    }
  }
}

/*
* OVO NISTA NIJE TESTIRANO
* I FALI [%reg+ symb]
*/
generated* codeGen::st(context* context){
  instruction* instr=context->instr;
  argument* arg2=instr->arg1->next;
  if(arg2->type==0){
    // registar
    if(arg2->mode==5){} // %rx ovo je greska
    if(arg2->mode==6){
      return new generated(generate(0b1000,0b0000,arg2->val1,0,instr->arg1->val1,0),nullptr);  
    }
    if(arg2->mode==7){
      // [reg + simbol ili literal]
      if(arg2->name==nullptr){
        // literal
        
        if(arg2->val2>(1<<11)-1 || arg2->val2<-(1<<11)) {printf("Greska");exit(0);}
        return new generated(generate(0b1000,0b0000,arg2->val1,0,instr->arg1->val1,arg2->val2),nullptr);
      }else {
        //simbol
        if(context->symbtable->getSymb(arg2->name)->section!=-1){printf("Greska dodaj pls!");exit(0);}
        int val=context->symbtable->getSymb(arg2->name)->value;
        if(val>(1<<11)-1 || val<-(1<<11)) {printf("Greska pls");exit(0);}
        return new generated(generate(0b1000,0b0000,arg2->val1,0,instr->arg1->val1,val),nullptr);
      }
    }
  }
  if(arg2->type==1){
    // literal
    if(arg2->mode==1){} // $literal greska
    if(arg2->mode==2){ // literal
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(arg2->val1)-*(context->count)-4;
      return new generated(generate(0b1000,0b0010,15,0,instr->arg1->val1,pcOffset),nullptr);
    }
  }
  if(arg2->type==2){
    if(arg2->mode==1){} // $symbol greska
    if(arg2->mode==2){  // symbol
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(arg2->name)-*(context->count)-4;
      generated* head= new generated(generate(0b1000,0b0010,15,0,instr->arg1->val1,pcOffset),nullptr);
      // i relokacioni zapis
      int off=context->secTable->getLPool(sectionTable::curSection)->getAdr(arg2->name);
      int section=context->symbtable->getSection(arg2->name);
      int addend=context->symbtable->getValue(arg2->name);
      if(section==-2) context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,arg2->name,addend);
      else context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,section,addend);
      return head;

    }
  }
}

/*
* OVO NISTA NIJE TESTIRANO
* I FALI [%reg+ symb]
*/
generated* codeGen::ld(context* context){
  instruction* instr=context->instr;
  if(instr->arg1->type==0){
    // registar
    if(instr->arg1->mode==5){
      // operand je %reg 
      int dest=instr->arg1->next->val1;
      int source=instr->arg1->val1;
      int ret=0b10010001000000000000000000000000 | (dest<<20) | (source<<16);
      return new generated(ret,nullptr);
    }
    if(instr->arg1->mode==6){
      // operand je [%reg]
      return new generated(generate(9,2,instr->arg1->next->val1,instr->arg1->val1,0,0),nullptr);
    }
    if(instr->arg1->mode==7){
      // operand je [%reg + simbol ili literal]
      if(instr->arg1->name==nullptr){
        //literal
        
        if(instr->arg1->val2>(1<<11)-1 || instr->arg1->val2<(-(1<<11))) {printf("Dodaj da lepo ispise");exit(0);}
        return new generated(generate(0b1001,0b0010,instr->arg1->next->val1,instr->arg1->val1,0,instr->arg1->val2),nullptr);
      }else {
        // symbol
        if(context->symbtable->getSymb(instr->arg1->name)->section!=-1){printf("Greska dodaj pls!");exit(0);}
        int val=context->symbtable->getSymb(instr->arg1->name)->value;
        if(val>(1<<11)-1 || val<-(1<<11)) {printf("Greska pls");exit(0);}
        return new generated(generate(0b1001,0b0010,instr->arg1->next->val1,instr->arg1->val1,0,val),nullptr);
      }
      
    }
  }

  if(instr->arg1->type==1){
    if(instr->arg1->mode==1){
      // $literal
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(instr->arg1->val1)-*(context->count)-4;// -4 jer pc pokazuje na sledecu 
      generated* ret=new generated(generate(0b1001,0b0010,instr->arg1->next->val1,15,0,pcOffset),nullptr); //ld [pc + offset do bazena],%r1
      return ret;
    } 
    if(instr->arg1->mode==2){
      // literal
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(instr->arg1->val1)-*(context->count)-4;
      generated* head=new generated(generate(0b1001,0b0010,instr->arg1->next->val1,15,0,pcOffset),nullptr);// ld literal u odredisni reg
      head->next=new generated(generate(0b1001,0b0010,instr->arg1->next->val1,instr->arg1->next->val1,0,0),nullptr);
      return head;
    } 
  }
  if(instr->arg1->type==2){
    if(instr->arg1->mode==1){
      // $symbol
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(instr->arg1->name)-*(context->count)-4;// -4 jer pc pokazuje na sledecu 
      generated* ret=new generated(generate(0b1001,0b0010,instr->arg1->next->val1,15,0,pcOffset),nullptr); //ld [pc + offset do bazena],%r1
      // takodje treba relokacioni zapis da prepravi vrednost u bazenu
      int off=context->secTable->getLPool(sectionTable::curSection)->getAdr(instr->arg1->name);
      int section=context->symbtable->getSection(instr->arg1->name);
      int addend=context->symbtable->getValue(instr->arg1->name);
      if(section==-2) context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,instr->arg1->name,addend);
      else context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,section,addend);
      return ret;
    }
    if(instr->arg1->mode==2){
      // symbol
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(instr->arg1->name)-*(context->count)-4;
      generated* head=new generated(generate(0b1001,0b0010,instr->arg1->next->val1,15,0,pcOffset),nullptr);// ld literal u odredisni reg
      head->next=new generated(generate(0b1001,0b0010,instr->arg1->next->val1,instr->arg1->next->val1,0,0),nullptr);
      // treba relokacioni zapis da prepravi vrednost u bazenu
      int off=context->secTable->getLPool(sectionTable::curSection)->getAdr(instr->arg1->name);
      int section=context->symbtable->getSection(instr->arg1->name);
      int addend=context->symbtable->getValue(instr->arg1->name);
      if(section==-2) context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,instr->arg1->name,addend);
      else context->secTable->getRelocTable(sectionTable::curSection)->addEntry(off,0,section,addend);
      return head;
    }
  }
}

generated* codeGen::section(context* context){
  int poolSize=context->secTable->getSection(sectionTable::curSection)->pool->getEntryCnt()*4;
  return new generated(generate(0b0011,0,15,0,0,poolSize),nullptr);
}