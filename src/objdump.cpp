#include <stdlib.h>
#include <vector>
#include "../inc/objdump.hpp"

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

void printSection(FILE* file,int size){
  int cnt=0;
  char temp;
  char* tempChar=(char*)malloc(sizeof(char)*256);
  /*while(cnt<size){
    fread(&temp,sizeof(char),1,file);
    //printf("%x\n",temp);
    cnt+=1;
  }
  printf("%d",cnt);*/
  fread(tempChar,sizeof(char),size,file);
  //printf("%s",tempChar);
}

void printReloc(FILE* file){
  int cnt;
  int temp;
  int symbol;
  char* tempChar=(char*)malloc(sizeof(char)*256);
  fread(&cnt,4,1,file);
  
  for(int i=0;i<cnt;i++){
    printf("Ulaz %d: ",i);
    fread(&temp,4,1,file);
    printf("%d ",temp);
    fread(&temp,4,1,file);
    printf("%d ",temp);
    fread(&symbol,4,1,file);
    printf("%d ",symbol);
    fread(&temp,4,1,file);
    printf("%d ",temp);
    if(symbol==-5) {
      readString(file,tempChar);
      printf("%s",tempChar);
    }
    printf("\n");
  }  
  free(tempChar);
}

int main(int argc,char** argv){
  FILE* file=fopen(argv[1],"rb");
  int cnt;
  int cnt1;
  fread(&cnt,4,1,file);
  printf("%x\n",cnt);
  cnt1=cnt;
  std::vector<int> sizes;
  int temp;
  for(int i=0;i<cnt;i++){
    fread(&temp,4,1,file); //offset  
    fread(&temp,4,1,file); // size sekcije
    sizes.push_back(temp);
    printf("%d\n",temp);
  }

  //citamo tabelu simbola
  fread(&cnt,4,1,file);// broj ulaza
  bool tempBool;
  char* tempString=(char*)malloc(sizeof(char)*256);
  for(int i=0;i<cnt;i++){
    fread(&temp,4,1,file);// value
    printf("Ulaz %d: ",i);
    printf("%d ",temp);  
    fread(&tempBool,sizeof(bool),1,file); //isSection
    printf("%d ",tempBool);
    fread(&temp,4,1,file); // type
    printf("%d ",temp);  
    fread(&temp,4,1,file); // section
    printf("%d ",temp); 
    readString(file,tempString);
    printf("%s ",tempString);
    fread(&tempBool,sizeof(bool),1,file);
    printf("%d\n",tempBool);
  }

  int cntSec;
  fread(&cntSec,4,1,file);
  int k;
  printf("TABELA SEKCIJA\n");
  for(int i=0;i<cntSec;i++){
    printf("Ulaz %d: ",i);
    fread(&k,4,1,file);
    printf("%d ",k);
    fread(&k,4,1,file);
    printf("%d ",k);
    fread(&k,4,1,file);
    printf("%d ",k);
    readString(file,tempString);
    printf("%s\n",tempString);
  }

  // citamo sekcije i relok zapise
  int secCount=cnt1-2;
  int byteCount=0;
  //printf("Relok tabele\n");
  for(int i=0;i<secCount;i++){
    printf("Relok tabela sekcije %d\n",i);
    printSection(file,sizes[i+2]);
    printReloc(file);
  }
  

}