#include "../inc/Events.hpp"

void Events::sectionFP(sectionTable* secTable,symbTable* symbTable,char* name,int* adrCnt){
  //if(sectionTable::curSection) 
  secTable->setLen(sectionTable::curSection,*adrCnt);
  sectionTable::curSection=name;
  secTable->insertSection(name,0,0);
  symbTable->insertSymb(name,-1,-1,0,false,true);
  *adrCnt=0;
  // u .end mora da se stavi duzina zadnje sekcije
}

void Events::equFP(sectionTable* secTable,symbTable* table,argument* arg,Tns* tns){
  //table->insertSymb(arg->name,-1,0,arg->next->val1,false,true);
  tns->addEntry(arg->name,arg);
}

void Events::globalFP(symbTable* table,argument* arg){
  for(argument* cur=arg;cur;cur=cur->next) table->setGlobalDef(cur->name,true);
}

void Events::externFP(symbTable* table,char* name){
  table->insertSymb(name,-2,1,0,0,true);
}

void Events::endFP(sectionTable* table,int cnt){
  table->setLen(sectionTable::curSection,cnt);
}

void Events::sectionSP(char* name,int* cnt){
  sectionTable::curSection=name;
  *cnt=0;
}

void Events::wordSP(sectionTable* secTable,symbTable* symbTable,instruction* instr,int* cnt){
  argument* arg=instr->arg1;
  for(;arg;arg=arg->next){
    if(arg->type==1){
      secTable->addContent(sectionTable::curSection,arg->val1);
    }
    if(arg->type==2){
      secTable->addContent(sectionTable::curSection,0);
      int offset=*cnt;
      int addend=symbTable->getValue(arg->name);
      int section=symbTable->getSection(arg->name);
      if(section==-2) secTable->getRelocTable(sectionTable::curSection)->addEntry(offset,0,arg->name,0);
      else secTable->getRelocTable(sectionTable::curSection)->addEntry(offset,0,section,addend);
    }
  }
}

void Events::skipSP(sectionTable* secTable,instruction* instr){
  for(int i=0;i<instr->arg1->val1;i++){
    secTable->addContent(sectionTable::curSection,(char)0);  
  }
}

void Events::asciiSP(sectionTable* secTable,instruction* instr){
  char* str=instr->arg1->name;
  str++;
  while(*str!='\"'){
    if(*str==92){
      str++;
      if(*str=='\"') {secTable->addContent(sectionTable::curSection,(char)92);break;}
      if(*str=='n'){secTable->addContent(sectionTable::curSection,'\n');str++; continue;}
      if(*str=='t'){secTable->addContent(sectionTable::curSection,'\t');str++; continue;}
      if(*str=='r'){secTable->addContent(sectionTable::curSection,'\r');str++; continue;}
    }
    secTable->addContent(sectionTable::curSection,*str);
    str++;
  }
 
 
}