#include "../inc/LPool.hpp"

void LPool::setAdr(int key, int adr){
  literalEntry* item=getEntry(key);
  if(!item) return;
  item->adr=adr;
}

void LPool::setAdr(char* key,int adr){
  literalEntry* item=getEntry(key);
  if(!item) return;
  item->adr=adr;
}

literalEntry* LPool::getEntry(int key){
  std::map<int,literalEntry*>::iterator itr=litMap.find(key);
  if(itr==litMap.end()) return nullptr;
  return itr->second;  
}

literalEntry* LPool::getEntry(char* key){
  std::map<std::string,literalEntry*>::iterator itr=symbMap.find(std::string(key));  
  if(itr==symbMap.end()) return nullptr;
  return itr->second;
}

int LPool::getAdr(int key){
  literalEntry* item=getEntry(key);
  if(!item) return -1;
  return item->adr;
}

int LPool::getAdr(char* key){
  literalEntry* item=getEntry(key);
  if(!item) return -1;
  return item->adr;  
}

bool LPool::insertLit(int key,int adr){
  if(getEntry(key)) return false;
  litMap[key]=new literalEntry(key,adr);
  return true;
}

bool LPool::insertLit(char* key,int adr){
  if(getEntry(key)) return false;
  symbMap[key]=new literalEntry(key,adr);
  return true;  
}

void LPool::printPool(){
  std::map<int,literalEntry*>::iterator itr=litMap.begin();
  printf("value         adr         \n");
  for(;itr!=litMap.end();itr++){
    printf("%x          %d          \n",itr->first,itr->second->adr);
  }  
  std::map<std::string,literalEntry*>::iterator itr1=symbMap.begin();
  for(;itr1!=symbMap.end();itr1++){
    printf("%s          %d          %x\n",itr1->second->name,itr1->second->adr,itr1->second->val);
  }   
}

void LPool::solve(int start){
  std::map<int,literalEntry*>::iterator itr=litMap.begin();
  for(;itr!=litMap.end();itr++){
    itr->second->adr=start;
    start+=4;
  } 
  std::map<std::string,literalEntry*>::iterator itr1=symbMap.begin();
  for(;itr1!=symbMap.end();itr1++){
    itr1->second->adr=start;
    start+=4;
  }   
}

void LPool::writePool(FILE* file){
  std::map<int,literalEntry*>::iterator itr=litMap.begin();
  for(;itr!=litMap.end();itr++){
    fwrite(&(itr->second->val),4,1,file);
  }  
  std::map<std::string,literalEntry*>::iterator itr1=symbMap.begin();
  for(;itr1!=symbMap.end();itr1++){
    fwrite(&(itr1->second->val),4,1,file);
  }    
}

int LPool::sizeOfPool(){
  int sz=0;
  std::map<int,literalEntry*>::iterator itr=litMap.begin();
  for(;itr!=litMap.end();itr++){
    sz+=4;
  }  
  std::map<std::string,literalEntry*>::iterator itr1=symbMap.begin();
  for(;itr1!=symbMap.end();itr1++){
    sz+=4;
  }  
  return sz;   
}