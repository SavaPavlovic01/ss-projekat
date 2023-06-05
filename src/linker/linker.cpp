#include "../../inc/symbTable.hpp"
#include "../../inc/sectionTable.hpp"
#include "../../inc/relocTable.hpp"
#include <iostream>
#include <unordered_set>
#include "../../inc/LPool.hpp"
#include <bits/stdc++.h>

// oduzmi 4 od velicine sekcije (offset)

char* readString(FILE* file,char* store){
  char c;
  int cnt=0;
  char* temp=store;
  c=fgetc(file);
  while(true){
    if(c=='\0' || c==EOF) break;
    *temp=c;
    temp++;
    cnt++;
    c=fgetc(file);
  }
  *temp='\0';
  store=(char*)realloc(store,sizeof(char)*(cnt+1));
  return store;
}

void readSec(FILE* file,int size,sectionTable* secTable,int ndx){
  int cnt=0;
  char data;
  int temp;
  fread(&temp,1,4,file);
  while(cnt<size-4){
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
    //printf("STUCK 1 %d\n",cnt);
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
    //printf("STUCK 2 %d\n",cnt1);
  }
  //syTable->printTable();

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
    secTable->insertSection(name,base,sizes[i+2]-4,ndx);
    //printf("STUCK 3 %d\n",cntSec);
  }
  //secTable->printTable();
  secTables->push_back(secTable);

  int secCount=cnt-2;
  for(int i=0;i<secCount;i++){
    readSec(file,sizes[i+2],secTable,i);
    readRelocL(file,i,secTable);
    //printf("STUCK 4 %d\n",secCount);
  }

}

void printCode(sectionTable* secTable){
  for(int i=0;i<secTable->getSectionCnt();i++){
    sectionTableItem* curSection=secTable->getSection(i);
    std::vector<cc*>* content=curSection->content;
    int cnt=0;
    for(int j=0;j<content->size();j++){
      if((cnt % 8)==0) printf("%05d ",cnt);
      printf("%02x ",(unsigned int) ((*content)[j]->byte) & (0x000000ff));
      cnt++;
      if((cnt % 8)==0) printf("\n");
    }
    printf("\n");
  }
  
}

void executeReloc(int data,int offset,sectionTableItem* section){
  std::vector<char> help;
  for(int i=0;i<4;i++){
    char temp=(data &(0x000000ff<<i*8))>>(i*8);
    help.push_back(temp);
  } 
  std::vector<cc*>* content=section->content; 
  for(int i=0;i<4;i++){
    (*content)[offset+i]->byte=help[i];  
  }
}

void checkOverlap(std::vector<sectionTableItem*>& vec){
  for(int i=0;i<vec.size()-1;i++){
    if((unsigned int)vec[i]->base+(unsigned int)vec[i]->len>(unsigned int)vec[i+1]->base){
      printf("%s and %s overlap\n",vec[i]->name,vec[i+1]->name);
      printf("%08x %08x %s",vec[i+1]->base,vec[i+1]->len,vec[i+1]->name);
      printf("\n");
      printf("%08x %s",vec[i]->base,vec[i]->name);
      exit(0);
    }
  }
}

void writeHex(FILE* file,std::vector<sectionTableItem*>& vec){
  int adr=0;
  int adrOld=0;
  int cnt=0;
  int zero=0;
  for(int i=0;i<vec.size();i++){
    fwrite(&(vec[i]->base),4,1,file); // adresa
    fwrite(&(vec[i]->len),4,1,file); // velicina sekcije
    std::vector<cc*>* content=vec[i]->content;
    for(int j=0;j<content->size();j++){
      fwrite(&((*content)[j]->byte),1,1,file);
    }
  }
}

void printHex(std::vector<sectionTableItem*>& vec){
  int adrOld=-2;
  int adrCur=0;
  bool jmp=false;
  for(int i=0;i<vec.size();i++){
    std::vector<cc*>* contents=vec[i]->content;
    adrCur=vec[i]->base;
    if(adrCur!=adrOld) {
      printf("\n%08x: ",adrCur);
      jmp=true;
    }
    for(int j=0;j<contents->size();j++){
      if(adrCur%8==0 && !jmp) printf("%08x: ",adrCur);
      printf("%02x ",(unsigned char)contents->operator[](j)->byte);
      adrCur++;
      if(adrCur%8==0) printf("\n");
      jmp=false;
    }
    adrOld=adrCur;
  }
}

bool compareSections(sectionTableItem* i1,sectionTableItem* i2){
  return ((unsigned int)i1->base<(unsigned int)i2->base);
}

int main(int argc,char** argv){

  std::map<std::string,unsigned int> sectionLocations;
  std::vector<char*> inputFile;
  char* outFile;
  char mode=-1;
  for(int i=1;i<argc;i++){
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

      sectionLocations[help]=(std::stol(help1,0,16));
      continue;
    }
    if(strcmp(argv[i],"-o")==0){
      if(i+1==argc) {
        printf("-o must be followed by file name\n");
        exit(0);
      }
      outFile=argv[i+1];
      i++;
      continue;
    }
    if(strcmp(argv[i],"-relocatable")==0){
      mode=1;
      continue;
    }
    if(strcmp(argv[i],"-hex")==0){
      mode=0;
      continue;
    }
    inputFile.push_back(argv[i]);

  }

  if(mode==-1) {
    printf("Missing -hex or -relocatable\n");
    exit(0);
  }
  std::vector<symbTable*> symbTables;
  std::vector<sectionTable*> secTables;

  for(int i=0;i<inputFile.size();i++){
    readAll(&symbTables,&secTables,inputFile[i]);
  }
  
  for(int i=0;i<secTables.size();i++){
    //secTables[i]->printTable();
  }
  
  
  // postavljamo pocetne adrese sekcija
  std::unordered_set<std::string> doneSections;
  std::vector<sectionTableItem*> outOrder;

  sectionTable mergedSections;
  
  int maxBase=0;
  int base=0;
  std::map<std::string,unsigned int>::iterator itr=sectionLocations.begin();
  for(;itr!=sectionLocations.end();itr++){
    if(mode==1) break;
    for(int i=0;i<secTables.size();i++){
      sectionTableItem* curSection=secTables[i]->getSection((char *)itr->first.c_str());
      if(doneSections.find(itr->first)==doneSections.end() && curSection) {
        
        base=itr->second;
        
        curSection->base=base;
       
        base+=curSection->len;
        if(base>maxBase) maxBase=base;
        doneSections.insert(itr->first);
        outOrder.push_back(curSection);
        mergedSections.insertSection(curSection->name,curSection->base,curSection->len);
        mergedSections.getSection(curSection->name)->base=curSection->base;

        for(int j=i+1;j<secTables.size();j++){
          for(int cnt1=0;cnt1<secTables[j]->getSectionCnt();cnt1++){
            
            sectionTableItem* item1=secTables[j]->getSection(curSection->name);
            if(item1){
              item1->base=base;
              
              base+=item1->len;
              if(base>maxBase) maxBase=base;
              outOrder.push_back(item1);
              mergedSections.setLen(curSection->name,curSection->len+item1->len);
              break;
            } 
          }
        }
      }else if(itr->second>maxBase) maxBase=itr->second;
    }  
  }

  base=maxBase;

  for(int i=0;i<secTables.size();i++){
    if (mode==1) break;
    for(int cnt=0;cnt<secTables[i]->getSectionCnt();cnt++){
      sectionTableItem* curSection=secTables[i]->getSection(cnt);
    
    
      std::string str(curSection->name);
      if(doneSections.find(str)==doneSections.end()) {
        std::map<std::string,unsigned int>::iterator itr=sectionLocations.find(str);
        if(itr!=sectionLocations.end()){
          base=itr->second;
        }
        curSection->base=base;
        base+=curSection->len;
        doneSections.insert(str);
        outOrder.push_back(curSection);
        mergedSections.insertSection(curSection->name,curSection->base,curSection->len);
        mergedSections.getSection(curSection->name)->base=curSection->base;

        for(int j=i+1;j<secTables.size();j++){
          for(int cnt1=0;cnt1<secTables[j]->getSectionCnt();cnt1++){
            
            sectionTableItem* item1=secTables[j]->getSection(curSection->name);
            if(item1){
              item1->base=base;
              base+=item1->len;
              outOrder.push_back(item1);
              mergedSections.setLen(curSection->name,curSection->len+item1->len);
              break;
            } 
          }
        }
      }
    }
  }

  if(mode==1){
    for(int i=0;i<secTables.size();i++){
      
      for(int cnt=0;cnt<secTables[i]->getSectionCnt();cnt++){
        sectionTableItem* curSection=secTables[i]->getSection(cnt);
        base=0;
        std::string str(curSection->name);
        if(doneSections.find(str)==doneSections.end()){
          base+=curSection->len;
          doneSections.insert(str);
          mergedSections.insertSection(curSection->name,0,0);
          sectionTable::mergeSections(mergedSections.getSection(curSection->name),curSection);
          relocTable::mergeReloc(mergedSections.getRelocTable(curSection->name),curSection->table);

          for(int j=i+1;j<secTables.size();j++){
            for(int cnt1=0;cnt1<secTables[j]->getSectionCnt();cnt1++){
            
              sectionTableItem* item1=secTables[j]->getSection(curSection->name);
              if(item1){
                
                base+=item1->len;
                sectionTable::mergeSections(mergedSections.getSection(curSection->name),item1);
                item1->table->updateTable(base-item1->len);
                relocTable::mergeReloc(mergedSections.getRelocTable(curSection->name),item1->table);
                //mergedSections.setLen(curSection->name,mergedSections.getSection(curSection->name)->len+item1->len);
                for(int h=0;h<secTables[j]->getSectionCnt();h++){
                  sectionTableItem* cs=secTables[j]->getSection(h);
                  relocTable* reloc=cs->table;
                  for(int hh=0;hh<reloc->getEntryCnt();hh++){
                    relocEntry* curRelocEntry=reloc->getEntry(hh);
                    if(curRelocEntry->symbol==item1->cnt){
                      curRelocEntry->addend+=base-item1->len;
                      //curRelocEntry->symbol=mergedSections.getSectionId(item1->name);
                    }
                  }
                }
                break;
              } 
            }
          }
        }
      }
    }
  }

  if(mode==1){
    for(int i=0;i<secTables.size();i++){
      sectionTable* curSectionTable=secTables[i];
      for(int cnt=0;cnt<curSectionTable->getSectionCnt();cnt++){
        sectionTableItem* curSection=curSectionTable->getSection(cnt);
        relocTable* curReloc=curSection->table;
        for(int cntReloc=0;cntReloc<curReloc->getEntryCnt();cntReloc++){
          char* name;
          relocEntry* curEntry=curReloc->getEntry(cntReloc);
          for(int help=0;help<curSectionTable->getSectionCnt();help++) {
            if(curSectionTable->getSection(help)->cnt==curEntry->symbol) name=curSectionTable->getSection(help)->name;
          } 
          curEntry->symbol=mergedSections.getSectionId(name);
        }
      }
    }
  }

  std::sort(outOrder.begin(),outOrder.end(),compareSections);
  checkOverlap(outOrder);
  //sortVector(outOrder);
  
  /*for(int i=0;i<symbTables.size();i++){
    (symbTables[i])->printTable();
  }*/

  // postavljamo vrednosti simbola
  std::map<std::string,int> globalSymbols;
  std::vector<symbTableItem*> unresolved;

  symbTable globalSymb;

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
        if(curSymbol->section==-1){
          if(curSymbol->globalDef){
            std::string str(curSymbol->name);
            globalSymb.insertSymb(curSymbol->name,-1,
              curSymbol->type,curSymbol->value,true,true);
            if(globalSymbols.find(str)!=globalSymbols.end()) {printf("Multiple definitions of %s\n",curSymbol->name);exit(0);}
            globalSymbols[str]=curSymbol->value;
          }
        }// simbol definisan sa equ, ne menjamo mu vrednost
        else {
          // mora da bude u istom fajlu
          sectionTableItem* k=secTables[i]->getSection(curSymbol->section);
          curSymbol->value=k->base+curSymbol->value;
          if(curSymbol->globalDef) {
            std::string str(curSymbol->name);
            globalSymb.insertSymb(curSymbol->name,mergedSections.getSectionId(secTables[i]->getSectionNameById(curSymbol->section)),
              curSymbol->type,curSymbol->value,true,true);
            if(globalSymbols.find(str)!=globalSymbols.end()) {printf("Multiple definitions of %s\n",curSymbol->name);exit(0);}
            globalSymbols[str]=curSymbol->value;
          }
        }
      }
      
    }
  }

  for(int i=0;i<unresolved.size();i++){
    if(mode==1) break;
    std::string str(unresolved[i]->name);
    std::map<std::string,int>::iterator itr=globalSymbols.find(str);
    if(itr==globalSymbols.end()){
      printf("Undefined symbol %s",unresolved[i]->name);
      exit(0);
    }
    unresolved[i]->value=itr->second;
  }

  for(int i=0;i<secTables.size();i++){
    //printCode(secTables[i]);
  }

  //printf("\n");printf("\n");printf("\n");

  // resavanje relokacionih zapisa
  for(int i =0; i<secTables.size();i++){
    if(mode==1) break;
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
        if(curEntry->symbol>=0){
          int data=curSecTable->getSection(curEntry->symbol)->base+curEntry->addend;
          executeReloc(data,curEntry->offset,curSection);  
        }
      }
    }
  }

  for(int i=0;i<secTables.size();i++){
    //(secTables[i])->printTable();
    //secTables[i]->printAllReloc();
  }

  //mergedSections.printAllReloc();
  
  
  for(int i=0;i<symbTables.size();i++){
    //(symbTables[i])->printTable();
  }

  for(int i=0;i<secTables.size();i++){
    //printCode(secTables[i]);
  }

  //mergedSections.printTable();
  //mergedSections.printAllReloc();
  //globalSymb.printTable();
  int tri=3;
  FILE* file=fopen(outFile,"wb");
  if(mode==0) {
    writeHex(file,outOrder);
  }
  if(mode==1) {
    int secCount=mergedSections.getSectionCnt()+2;
    fwrite(&secCount,4,1,file);// broj sekcija
    fwrite(&tri,4,1,file); //offset do tabele simbola
    int sz=globalSymb.getSizeOnDisk();
    fwrite(&sz,4,1,file);//velicina tabele simbola

    fwrite(&tri,4,1,file);// offset tabela sekcija
    sz=mergedSections.getSize();
    fwrite(&sz,4,1,file);// velicina tabele sekcija

    for(int i=0;i<secCount-2;i++){
      fwrite(&tri,4,1,file);
      sz=mergedSections.getSizeOnDiskNext(i);
      fwrite(&sz,4,1,file);
    }

    globalSymb.writeTable(file,&mergedSections);
    mergedSections.writeTable(file);

    for(int i=0;i<secCount-2;i++){
      fwrite(&tri,4,1,file);//offset
      mergedSections.writeNext(i,file);
      mergedSections.writeNextReloc(i,file);
    }

  }
  if(mode==0){
    FILE* alo=freopen(strcat(outFile,".txt"),"w+",stdout);
   
    printHex(outOrder);
    fclose(alo);
  }
  if(mode==1){
    FILE* o=freopen(strcat(outFile,".txt"),"w",stdout);
    printf("Symbol table\n");
    globalSymb.printTable();  
    printf("Section Table\n");
    mergedSections.printTable();
    printf("\n");
    mergedSections.printAllReloc();
    mergedSections.printAllCode();
    fclose(o);

  }

  
  
}