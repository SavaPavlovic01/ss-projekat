#include "../inc/relocTable.hpp"
#include <stdio.h>

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