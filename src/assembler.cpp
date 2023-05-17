#include "../misc/lexer.h"
#include "../misc/parser.tab.h"
#include "../inc/symbTable.hpp"
#include "../inc/instruction.h"
#include "../inc/assembler.hpp"
#include "../inc/sectionTable.hpp"
#include "../inc/Events.hpp"


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

  symbTable table;
  sectionTable sTable;
  instrHelp::initInstr();
  int cnt=0;

  instruction *cur=instrHead;

  for(;cur;cur=cur->next){
    std::string str(cur->name);
    if(instrHelp::getInstrType(cur->name)==-2){
      //ovde idemo ako nije instrukcija ili direktiva (znaci mora definicija simbola)
      if(cur->isLabel==1){
        if(!table.insertSymb(cur->name,sTable.getSectionId(sectionTable::curSection),2,cnt,false,true)){
          printf("Double definition of %s\n", cur->name);
          return -1;
        }
      }else {
        printf("Ne znam sta ti to bato %s\n",cur->name);
        return -2;
      }
    }else {
      //ovde idemo ako instrukcija ili direktiva
      //treba da proverimo da li ima neki simbol u argumentima
      argument* curArg=cur->arg1;
      for(;curArg;curArg=curArg->next){
        char* n=instrHelp::getSymbolsFromArg(curArg);
        if(n) table.insertSymb(n,0,2,0,false,false);
      }

      cnt+=instrHelp::getInstrSize(cur);

      // zapravo uradi ono sto instrukcija radi u prvom prolazu
      
      if(strcmp(cur->name,".section")==0){
        Events::sectionFP(&sTable,&table,cur->arg1->name,&cnt);  
      }
      if(strcmp(cur->name,".equ")==0){
        Events::equFP(&sTable,&table,cur->arg1); 
      }
      if(strcmp(cur->name,".global")==0){
        Events::globalFP(&table,cur->arg1); 
      }
      if(strcmp(cur->name,".extern")==0){
        Events::externFP(&table,cur->arg1->name);
      }
      if(strcmp(cur->name,".end")==0){
        Events::endFP(&sTable,cnt);
      }
      
    }
    
  }

  table.printTable();
  printf("\n");
  sTable.printTable();

  return 0;
}