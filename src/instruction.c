#include "../inc/instruction.h"
#include <stdlib.h>
#include <stdio.h>

/*
--header-file=FILE
-o parser.c
*/
instruction* instrHead=NULL;
instruction* instrTail=NULL;

void makeInstr(char* name,argument* arg1,int isLabel){
  instruction* instr=(instruction*)malloc(sizeof(instruction));
  instr->name=name;
  instr->arg1=arg1;
  instr->isLabel=isLabel;
  instr->next=NULL;
  if(instrTail!=NULL){
    instrTail->next=instr;
    instrTail=instr;
  }else{
    instrTail=instr;
  }

  if(!instrHead) instrHead=instr;
}

argument* makeArg(int type,int mode,char* name,int val1,int val2,argument* next){
  argument* ret=(argument*)malloc(sizeof(argument));
  ret->type=type;
  ret->mode=mode;
  ret->name=name;
  ret->val1=val1;
  ret->val2=val2;
  ret->next=next;
  return ret;
}

void printAll(instruction* head){
  instruction* cur=head;
  while(cur){
    printf("INSTRUCTION %s\n",cur->name);
    if(cur->arg1) {
      printf("Arg1{");
      printArg(cur->arg1);
      printf("}\n");
    }
    

    printf("\n");
    cur=cur->next;
  }

  printf("DONE\n");
}

void printArg(argument* arg){
  argument* cur=arg;
  int i=0;
  while(cur){
    i++;
    if(cur->name){printf("name: %s ",cur->name);}
    printf("argType:%d adrMode:%d va1:%d val2:%d",cur->type,cur->mode,cur->val1,cur->val2);
    cur=cur->next;
  }
  
}