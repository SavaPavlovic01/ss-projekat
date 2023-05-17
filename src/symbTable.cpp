#include "../inc/symbTable.hpp"
#include <iterator>

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
  //vrv ovde treba update ako je tu al nije definisan
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
  item->globalDef=val;
  item->type=1; //globalno vezivanje??? idk
  return 0;
}