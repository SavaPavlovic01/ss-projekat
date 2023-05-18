#include "../inc/codeGen.hpp"

int codeGen::generate(int opcode,int m,int a,int b,int c,int d){
  int ret=0;
  ret=ret | (opcode<<28) | (m<<24) | (a<<20) | (b<<16) | (c<<12) |d;
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

generated* codeGen::st(context* context){
  /*
  push %r1
  ld [pc + offset do bazena],%r1 ako je odrediste literal ili simbol
  st %rx,%r1
  */
}

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
        int tempReg=1;
        generated* head=new generated(0,nullptr);
        if(instr->arg1->val1==1) tempReg=2; 
        head->code=generate(0b1000,0b0001,14,0,tempReg,-4);// push %r1 ili r2;
        
        int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(instr->arg1->val2)-*(context->count)-4;
        head->next=new generated(generate(0b1001,0b0010,tempReg,15,0,pcOffset),nullptr); //ld [pc + offset do bazena],%r1
        generated* cur=head->next;
        cur->next=new generated(generate(0b1001,0b0010,instr->arg1->next->val1,instr->arg1->val1,tempReg,0),nullptr);// ld
        cur=cur->next;
        cur->next=new generated(generate(0b1001,0b0011,tempReg,14,0,4),nullptr);// pop %r1 ili r2
        return head;
      }else {
        // + simbol !!!!OVO NE RADI ZBOG PARSERA POPRAVI!!!!!!!!!!!!!!!!
        // isto kao plus literal samo treba da se procita vednost iz symbTable i relok zapis
      }
      
    }
  }

  if(instr->arg1->type==1){
    if(instr->arg1->mode==1){
      // $literal
      int pcOffset=context->secTable->getLPool(sectionTable::curSection)->getAdr(instr->arg1->val2)-*(context->count)-4;// -4 jer pc pokazuje na sledecu 
      generated* ret=new generated(generate(0b1001,0b0010,instr->arg1->next->val1,15,0,pcOffset),nullptr); //ld [pc + offset do bazena],%r1
      return ret;
    }  
  }
}