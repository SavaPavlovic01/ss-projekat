#include "../inc/sectionTable.hpp"

char* sectionTable::curSection=nullptr;

bool sectionTable::insertSection(char* name,int base,int len){
  
  if(getSection(name)) return false;
  std::string str(name);
  map[str]=new sectionTableItem(name,0,len,cnt++);   
  return true; 
}

sectionTableItem* sectionTable::getSection(char* name){
  std::string str(name);
  std::map<std::string,sectionTableItem*>::iterator itr=map.find(str);
  if(itr==map.end()) return nullptr;
  return itr->second;
}

bool sectionTable::setLen(char* name,int len){
  std::string str(name);
  std::map<std::string,sectionTableItem*>::iterator itr=map.find(str);
  if(itr==map.end()) return false;
  itr->second->len=len;
  return true;
}

void sectionTable::printTable(){
  std::map<std::string,sectionTableItem*>::iterator itr=map.begin();
  printf("Name    Base    Len   Cnt\n");
  for(;itr!=map.end();itr++){
    printf("%s    %d    %d    %d\n",itr->second->name,itr->second->base,
      itr->second->len,itr->second->cnt);
  }  
}

void sectionTable::printAllPools(){
  std::map<std::string,sectionTableItem*>::iterator itr=map.begin();
  printf("POOLS\n");
  for(;itr!=map.end();itr++){
    itr->second->pool->printPool();
  }    
}

int sectionTable::getSectionId(char* name){
  if(!name) return -1;
  sectionTableItem* item=this->getSection(name);
  if(item) return item->cnt;
  return -1;
}

LPool* sectionTable::getLPool(char* name){
  sectionTableItem* item=getSection(name);
  if(item) return item->pool;
  return nullptr;
}

void sectionTable::printPool(char* name){
  sectionTableItem* item=getSection(name);
  if(item) item->pool->printPool(); 
}

void sectionTable::solvePools(){
  std::map<std::string,sectionTableItem*>::iterator itr=map.begin();
  for(;itr!=map.end();itr++){
    itr->second->pool->solve(itr->second->len);
  }  
}