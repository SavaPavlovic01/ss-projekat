#include "../inc/LPool.hpp"

void LPool::setAdr(int key, int adr){
  literalEntry* item=getEntry(key);
  if(!item) return;
  item->adr=adr;
}

literalEntry* LPool::getEntry(int key){
  std::map<int,literalEntry*>::iterator itr=litMap.find(key);
  if(itr==litMap.end()) return nullptr;
  return itr->second;  
}

int LPool::getAdr(int key){
  literalEntry* item=getEntry(key);
  if(!item) return -1;
  return item->adr;
}

bool LPool::insertLit(int key,int adr){
  if(getEntry(key)) return false;
  litMap[key]=new literalEntry(key,adr);
  return true;
}

void LPool::printPool(){
  std::map<int,literalEntry*>::iterator itr=litMap.begin();
  printf("value         adr         \n");
  for(;itr!=litMap.end();itr++){
    printf("%x          %d          \n",itr->first,itr->second->adr);
  }  
}

void LPool::solve(int start){
  std::map<int,literalEntry*>::iterator itr=litMap.begin();
  for(;itr!=litMap.end();itr++){
    itr->second->adr=start;
    start+=4;
  }  
}