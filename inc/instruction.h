#ifndef _instruction_h_
#define _instruction_h_
typedef enum adrMode {no,dir,memDir,memInd,pcRel,regDir,regInd,regIndOff} adrMode;
typedef enum argType {reg,lit,symb} argType;

typedef struct argument{
  int type;
  int mode;
  char* name;
  int val1;
  int val2;
  
  struct argument* next;
} argument;

typedef struct instruction{
  char* name;
  argument* arg1;
  struct instruction* next;
  int isLabel;

} instruction;

extern instruction* instrTail;
extern instruction* instrHead;

void makeInstr(char* name,argument* arg1,int isLabel);

argument* makeArg(int type,int mode,char* name,int val1,int val2,argument* next);

void printAll(instruction* head);

void printArg(argument* arg);

#endif