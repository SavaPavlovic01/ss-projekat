#include "../inc/Events.hpp"

void Events::sectionFP(sectionTable* secTable,symbTable* symbTable,char* name,int* adrCnt){
  if(sectionTable::curSection) secTable->setLen(sectionTable::curSection,*adrCnt);
  sectionTable::curSection=name;
  secTable->insertSection(name,0,0);
  symbTable->insertSymb(name,-1,-1,0,false,true);
  *adrCnt=0;
  // u .end mora da se stavi duzina zadnje sekcije
}

void Events::equFP(sectionTable* secTable,symbTable* table,argument* arg){
  table->insertSymb(arg->name,secTable->getSectionId(sectionTable::curSection),0,arg->next->val1,false,true);
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