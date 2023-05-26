#include "../../inc/symbTable.hpp"
#include "../../inc/sectionTable.hpp"
#include "../../inc/relocTable.hpp"
#include <iostream>
#include <unordered_set>
#include "../../inc/LPool.hpp"

// oduzmi 4 od velicine sekcije (offset)

char* readString(FILE* file,char* store){
  char c;
  char* temp=store;
  c=fgetc(file);
  while(true){
    if(c=='\0' || c==EOF) break;
    *temp=c;
    temp++;
    c=fgetc(file);
  }
  *temp='\0';
  
  return store;
}

void readSec(FILE* file,int size,sectionTable* secTable,int ndx){
  int cnt=0;
  char data;
  while(cnt<size){
    fread(&data,1,1,file);
    secTable->addContent(ndx,data);
    cnt++;
  }
}

void readRelocL(FILE* file,int ndx,sectionTable* secTable){
  int cnt;
  int offset;
  int type;
  int symbol;
  int addend;
  
  fread(&cnt,4,1,file);
  
  for(int i=0;i<cnt;i++){
    fread(&offset,4,1,file);
    fread(&type,4,1,file);
    fread(&symbol,4,1,file);
    fread(&addend,4,1,file);
    if(symbol==-5) {
      char* name=(char*)malloc(sizeof(char)*256);
      readString(file,name);
      secTable->getRelocTable(ndx)->addEntry(offset,type,name,addend);
    }else secTable->getRelocTable(ndx)->addEntry(offset,type,symbol,addend);
  }  
}

void readAll(std::vector<symbTable*>* symbTables,std::vector<sectionTable*>* secTables,char* fileName){
  symbTable* syTable=new symbTable();
  sectionTable* secTable=new sectionTable();

  FILE* file=fopen(fileName,"rb");
  if(!file) {
    printf("Cant find file %s\n",fileName);
    exit(0);
  }
  std::vector<int> sizes;
  int cnt;
  int temp;
  fread(&cnt,4,1,file);
  for(int i=0;i<cnt;i++){
    fread(&temp,4,1,file); //offset  
    fread(&temp,4,1,file); // size sekcije
    sizes.push_back(temp);  
  }
  int cnt1;
  //citamo tabelu simbola
  fread(&cnt1,4,1,file);// broj ulaza
  int value;
  int type;
  int section;
  
  bool globalDef;
  for(int i=0;i<cnt1;i++){
    char* name=(char*)malloc(sizeof(char)*32);
    fread(&value,4,1,file);// value 
    fread(&globalDef,sizeof(bool),1,file); //isSection
    fread(&type,4,1,file); // type
    fread(&section,4,1,file); // section 
    readString(file,name);
    fread(&globalDef,sizeof(bool),1,file);
    syTable->insertSymb(name,section,type,value,globalDef,true);
  }

  symbTables->push_back(syTable);

  int cntSec;
  fread(&cntSec,4,1,file);

  int ndx;
  int base;
  int len;
  for(int i=0;i<cntSec;i++){
    char* name=(char*) malloc(sizeof(char)*30);
    fread(&ndx,4,1,file);
    fread(&base,4,1,file);
    fread(&len,4,1,file);
    readString(file,name);
    secTable->insertSection(name,base,sizes[i+2]-4);
  }
  secTables->push_back(secTable);

  int secCount=cnt-2;
  for(int i=0;i<secCount;i++){
    readSec(file,sizes[i+2],secTable,i);
    readRelocL(file,i,secTable);
  }

}

void executeReloc(int data,int offset,sectionTableItem* section){
  
}

int main(int argc,char** argv){

  std::map<std::string,int> sectionLocations;
  for(int i=0;i<argc;i++){
    std::string str(argv[i]);
    if(str.find(std::string("-place="))!=-1){
      std::string help;
      std::string::iterator it=str.begin();
      it+=7;
      while(*it!='@'){
        help+=(*it);
        it++;
      }
      it++;
      std::string help1;
      while(*it!='\0'){
        help1+=(*it);
        it++;
      }

      sectionLocations[help]=std::stoi(help1,0,16);
    }
  }

  std::vector<symbTable*> symbTables;
  std::vector<sectionTable*> secTables;

  readAll(&symbTables,&secTables,argv[1]);
  readAll(&symbTables,&secTables,argv[2]);
  //readAll(&symbTables,&secTables,argv[3]);
  
  
  
  // postavljamo pocetne adrese sekcija
  std::unordered_set<std::string> doneSections;

  int base=0;
  for(int i=0;i<secTables.size();i++){
    for(int cnt=0;cnt<secTables[i]->getSectionCnt();cnt++){
      sectionTableItem* curSection=secTables[i]->getSection(cnt);
    
    
      std::string str(curSection->name);
      if(doneSections.find(str)==doneSections.end()) {
        std::map<std::string,int>::iterator itr=sectionLocations.find(str);
        if(itr!=sectionLocations.end()){
          base=itr->second;
        }
        curSection->base=base;
        base+=curSection->len;
        doneSections.insert(str);

        for(int j=i+1;j<secTables.size();j++){
          for(int cnt1=0;cnt1<secTables[j]->getSectionCnt();cnt1++){
            
            sectionTableItem* item1=secTables[j]->getSection(curSection->name);
            if(item1){
              item1->base=base;
              base+=item1->len;
              break;
            } 
          }
        }
      }
    }
  }

  /*for(int i=0;i<symbTables.size();i++){
    (symbTables[i])->printTable();
  }*/

  // postavljamo vrednosti simbola
  std::map<std::string,int> globalSymbols;
  std::vector<symbTableItem*> unresolved;

  for(int i=0;i<symbTables.size();i++){
    symbTable* curTable=symbTables[i];
    for(int cnt=0;cnt<curTable->getSymbCnt();cnt++){
      symbTableItem* curSymbol=curTable->getSymb(cnt);
      if(!curSymbol) break;
      if(curSymbol->section==-2){
        // trazi ga u drugim fajlovima
        std::string str(curSymbol->name);
        std::map<std::string,int>::iterator itr=globalSymbols.find(str);
        if(itr==globalSymbols.end()) unresolved.push_back(curSymbol);
        else curSymbol->value=itr->second;
      }
      else {
        if(curSymbol->section==-1){}// simbol definisan sa equ, ne menjamo mu vrednost
        else {
          // mora da bude u istom fajlu
          sectionTableItem* k=secTables[i]->getSection(curSymbol->section);
          curSymbol->value=k->base+curSymbol->value;
          if(curSymbol->globalDef) {
            std::string str(curSymbol->name);
            if(globalSymbols.find(str)!=globalSymbols.end()) {printf("Multiple definitions of %s\n",curSymbol->name);exit(0);}
            globalSymbols[str]=curSymbol->value;
          }
        }
      }
      
    }
  }

  for(int i=0;i<unresolved.size();i++){
    std::string str(unresolved[i]->name);
    std::map<std::string,int>::iterator itr=globalSymbols.find(str);
    if(itr==globalSymbols.end()){
      printf("Undefined symbol %s",unresolved[i]->name);
      exit(0);
    }
    unresolved[i]->value=itr->second;
  }

  // resavanje relokacionih zapisa
  for(int i =0; i<secTables.size();i++){
    // iteriramo kroz sve tabele sekcija
    sectionTable* curSecTable=secTables[i];
    for(int cnt=0;cnt<curSecTable->getSectionCnt();cnt++){
      // iteriramo kroz sve sekcije
      sectionTableItem* curSection=curSecTable->getSection(cnt);
      relocTable* curRelocTable=curSection->table;
      for(int cntReloc=0;cntReloc<curRelocTable->getEntryCnt();cntReloc++){
        // kroz sve ulaze relok tabele
        relocEntry* curEntry=curRelocTable->getEntry(cntReloc);
        if(curEntry->symbol==-1){
          // equ
          executeReloc(curEntry->addend,curEntry->offset,curSection);
        }
        if(curEntry->symbol==-5){
          // extern
          std::string str(curEntry->name);
          std::map<std::string,int>::iterator itr=globalSymbols.find(str);
          if(itr==globalSymbols.end()){printf("au\n"); exit(0);}
          executeReloc(itr->second,curEntry->offset,curSection);
        }
        if(curEntry->symbol!=-1 && curEntry->symbol!=-5){
          int data=curSecTable->getSection(curEntry->symbol)->base+curEntry->addend;
          executeReloc(data,curEntry->offset,curSection);  
        }
      }
    }
  }

  for(int i=0;i<secTables.size();i++){
    //(secTables[i])->printTable();
    secTables[i]->printAllReloc();
  }

  
  for(int i=0;i<symbTables.size();i++){
    //(symbTables[i])->printTable();
  }
  
}