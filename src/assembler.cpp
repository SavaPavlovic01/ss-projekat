#include "../misc/lexer.h"
#include "../misc/parser.tab.h"
#include "../inc/symbTable.hpp"
#include "../inc/instruction.h"
#include "../inc/assembler.hpp"
#include "../inc/sectionTable.hpp"
#include "../inc/Events.hpp"
#include "../inc/codeGen.hpp"
#include "../inc/instrHelp.hpp"

// za prvi prolaz je ostalo:
// tabela literala (uradio valjda dobro; samo u drugom prolazu postavi adrese)
// tabela relokacionih zapisa (idk da l prvi ili drugi (vrv drugi))
// .equ :( (tns i ta sranja) takodje treba da doradis parser da moze da cita izraze za equ
// provera greske (uradjeno ali treba da se testira)

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
  context* con=new context(&sTable,&table,nullptr,&cnt);

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

      if(!instrHelp::isValid(cur)) {
        printf("%s je sjeban",cur->name);
        return -3;
      }
      //treba da proverimo da li ima neki simbol u argumentima
      argument* curArg=cur->arg1;
      for(;curArg;curArg=curArg->next){
        char* n=instrHelp::getSymbolsFromArg(curArg);
        if(n) table.insertSymb(n,0,2,0,false,false);
        if(curArg->type==1){
          if(strcmp(cur->name,".equ")!=0){
            LPool* pool=sTable.getLPool(sectionTable::curSection);
            pool->insertLit(curArg->val1,0);
          }
        }
      }

      cnt+=instrHelp::getInstrSize(cur);

      // zapravo uradi ono sto instrukcija radi u prvom prolazu
      // placeholder kod sredi ovo pls
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
  
  sTable.solvePools();
  table.printTable();
  printf("\n");
  sTable.printTable();
  sTable.printAllPools();

  return 0;
}