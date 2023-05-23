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

relocTable* sectionTable::getRelocTable(char* name){
  sectionTableItem* item=getSection(name);
  if(item) return item->table;   
  return nullptr;
}

void sectionTable::printAllReloc(){
  std::map<std::string,sectionTableItem*>::iterator itr=map.begin();
  printf("RELOC TABLES\n");
  for(;itr!=map.end();itr++){
    itr->second->table->printTable();
  }      
}

void sectionTable::addContent(char* name,int code){
  sectionTableItem* item=getSection(name);
  if(!item) return;
  item->content->push_back(new cc(code,0,0));
}

void sectionTable::addContent(char* name,char byte){
  sectionTableItem* item=getSection(name);
  if(!item) return;
  item->content->push_back(new cc(-1,byte,1));  
}

void sectionTable::printCode(sectionTableItem* item){
  std::vector<cc*> help=*(item->content);
  for(int i=0;i<item->content->size();i++){
    if(help[i]->type==0) printf("%x\n",help[i]->code);
    else printf("%x\n",help[i]->byte);
  }
}

void sectionTable::printAllCode(){
  std::map<std::string,sectionTableItem*>::iterator itr=map.begin();
  printf("CODE \n");
  for(;itr!=map.end();itr++){
    printf("%s\n",itr->second->name);
    printCode(itr->second);
  }        
}

void sectionTable::writeSection(char* name,FILE* file){
  sectionTableItem* item=getSection(name);
  if(!item) return;
  std::vector<cc*> help=*(item->content);
  for(int i=0;i<help.size();i++){
    if(help[i]->type==0){
      fwrite(&(help[i]->code),4,1,file);
    }else fwrite(&(help[i]->byte),1,1,file);
  }

  getLPool(name)->writePool(file);
}

void sectionTable::writeNext(int num,FILE* file){
  std::map<std::string,sectionTableItem*>::iterator itr=map.begin();
  int i=0;
  for(;itr!=map.end();itr++){
    if(i==num) {
      writeSection(itr->second->name,file);
      return;
    }
    i++;
  }   
}

void sectionTable::writeNextReloc(int num,FILE* file){
  std::map<std::string,sectionTableItem*>::iterator itr=map.begin();
  int i=0;
  for(;itr!=map.end();itr++){
    if(i==num) {
      itr->second->table->writeTable(file);
      return;
    }
    i++;
  }     
}

int sectionTable::getSizeOnDiskNext(int num){
  std::map<std::string,sectionTableItem*>::iterator itr=map.begin();
  int i=0;
  int sz=0;
  for(;itr!=map.end();itr++){
    if(i==num) {
      
      return getSizeOnDiskSection(itr->second);
    }
    i++;
  }     
}

int sectionTable::getSizeOnDiskSection(sectionTableItem* item){
  int sz=0;
  if(!item) return -1;
  std::vector<cc*> help=*(item->content);
  for(int i=0;i<help.size();i++){
    if(help[i]->type==0){
      sz+=4;
    }else sz+=1;
  }

  sz+=item->pool->sizeOfPool(); 
  return sz;
}