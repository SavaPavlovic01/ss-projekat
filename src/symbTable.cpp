#include "../inc/symbTable.hpp"
#include <iterator>
#include <string.h>

bool symbTable::insertSymb(symbTableItem* item){
  if(getSymb(item->name)) return false;  
  std::string str(item->name);
  table[str]=item;
  return true;
}
/**
* returns false only if symbol is in table and isDef==true
* else returns true 
* if the symbol is in the table, it gets updated
* baci pogled da l je ovo dobro
* isDef je true ako hoces da definises simbol
* ako ga nadjes kao argument onda isDef=false
*/
bool symbTable::insertSymb(char* name,int section,int type,int value,bool globalDef,bool isDef){
  if(name[0]=='\"') return true; // samo zbog ascii; veoma hacky nadji nesto bolje plssssss
  symbTableItem* i=getSymb(name);
  if(i){
    if(isDef && i->isDef) return false;
    
    if(!(i->isDef) && isDef){
      i->section=section;
      i->type=type;
      i->value=value;
      
      i->isDef=isDef;
      
    }
    return true;
  } 
  
  std::string str(name);
  table[str]=new symbTableItem(name,section,type,value,globalDef,isDef);  
  return true;
}

symbTableItem* symbTable::getSymb(char* name){
  
  std::string str(name);
  std::unordered_map<std::string,symbTableItem*>::iterator itr=table.find(str);
  if(itr==table.end()) return nullptr;
  return itr->second;
}

void symbTable::printTable(){
  std::unordered_map<std::string,symbTableItem*>::iterator itr=table.begin();
  printf("Name    Section   type    value   globalDef   isDef\n");
  for(;itr!=table.end();itr++){
    printf("%s    %d    %d    %d    %d    %d\n",itr->second->name,itr->second->section,
      itr->second->type,itr->second->value,itr->second->globalDef,itr->second->isDef);
  }
}

int symbTable::setGlobalDef(char* name,bool val){
  symbTableItem* item=this->getSymb(name);
  if(!item) return -1;
  item->globalDef=val;
  item->type=1; //globalno vezivanje??? idk
  return 0;
}

int symbTable::getSection(char* name){
  symbTableItem* item=this->getSymb(name);  
  if(!item) return -5;
  return item->section;
}

int symbTable::getValue(char* name){
  symbTableItem* item=this->getSymb(name);  
  if(!item) return -5;
  return item->value;  
}

void symbTable::writeTable(FILE* file,sectionTable* secTable){
  std::unordered_map<std::string,symbTableItem*>::iterator itr=table.begin();
  int sz=table.size();
  fwrite(&sz,4,1,file);
  char out=0;
  for(;itr!=table.end();itr++){
    fwrite(&(itr->second->value),4,1,file);
    if(secTable->getSection(itr->second->name)) out=1;
    else out=0;
    fwrite(&out,sizeof(char),1,file);
    fwrite(&(itr->second->type),4,1,file);
    fwrite(&(itr->second->section),4,1,file);
    fwrite(itr->second->name,sizeof(char),strlen(itr->second->name)+1,file);
    fwrite(&(itr->second->globalDef),sizeof(bool),1,file);
  }  
}

int symbTable::getSizeOnDisk(){
  int sz=4;
  std::unordered_map<std::string,symbTableItem*>::iterator itr=table.begin();
  for(;itr!=table.end();itr++){
    sz+=(4+sizeof(char)+4+4+(strlen(itr->second->name)+1)*sizeof(char)+sizeof(bool));
  }  

  return sz;
}

bool symbTable::isDefined(){
  std::unordered_map<std::string,symbTableItem*>::iterator itr=table.begin();
  for(;itr!=table.end();itr++){
    if(!(itr->second->isDef)){
      printf("%s is not defined",itr->second->name);
      //exit(0);
      itr->second->isDef=true;
      itr->second->section=-2;
      
    }
  }
  return true;  
}

symbTableItem* symbTable::getSymb(int num){
  int i=0;
  std::unordered_map<std::string,symbTableItem*>::iterator itr=table.begin();
  for(;itr!=table.end();itr++){
    if(i==num){
      return itr->second;
    }
    i++;
  }
  return nullptr;
}

