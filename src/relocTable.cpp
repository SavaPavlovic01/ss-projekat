#include "../inc/relocTable.hpp"
#include <stdio.h>
#include <string.h>

void relocTable::addEntry(int offset,int type,int symbol,int addend){
  vector.push_back(new relocEntry(offset,type,symbol,addend));
}

void relocTable::addEntry(int offset,int type,char* name,int addend){
  vector.push_back(new relocEntry(offset,type,name,addend));
}

void relocTable::printTable(){
  printf("offset        type        symbol        addend        name\n");
  for(int i=0;i<vector.size();i++){
    printf("%d        %d        %d        %d        ",vector[i]->offset,vector[i]->type,vector[i]->symbol,vector[i]->addend);
    if(vector[i]->name) printf("%s",vector[i]->name);
    printf("\n");
  }
}

void relocTable::writeTable(FILE* file){
  int siz=vector.size();
  fwrite(&siz,4,1,file);
  for(int i=0;i<vector.size();i++){
    fwrite(&(vector[i]->offset),4,1,file);
    fwrite(&(vector[i]->type),4,1,file);
    fwrite(&(vector[i]->symbol),4,1,file);
    fwrite(&(vector[i]->addend),4,1,file);
    if(vector[i]->symbol==-5) fwrite(vector[i]->name,sizeof(char),strlen(vector[i]->name)+1,file);
  }
}

int relocTable::getSizeOnDisk(){
  int siz=vector.size();
  int sz=4;
  for(int i=0;i<vector.size();i++){
    sz+=(4+4+4+4);
    if(vector[i]->symbol==-5) sz+=((strlen(vector[i]->name)+1)*sizeof(char));
  }  
  return sz;
}

relocEntry* relocTable::getEntry(int num){
  return vector[num];
}