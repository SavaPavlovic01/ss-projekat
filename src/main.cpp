#include "../inc/instruction.h"
#include "../misc/lexer.h"
#include "../misc/parser.tab.h"
#include "../inc/symbTable.hpp"



int main(int argc,char** argv){
  instrHead=nullptr;
  instrTail=nullptr;
  
  
  FILE* myFile=fopen(argv[1],"r");
  if(!myFile){
    printf("cant find file");
    return 2;
  }
  yyin=myFile;
  if(yyparse()) return 1;

  printf("\n");
  printf("\n");
  printf("\n");
  printf("\n");

  printAll(instrHead);  
}