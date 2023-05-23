#include "../misc/lexer.h"
#include "../misc/parser.tab.h"
#include "../inc/symbTable.hpp"
#include "../inc/instruction.h"
#include "../inc/assembler.hpp"
#include "../inc/sectionTable.hpp"
#include "../inc/Events.hpp"
#include "../inc/codeGen.hpp"
#include "../inc/instrHelp.hpp"
#include "../inc/relocTable.hpp"
#include <fstream>
#include <iostream>

// za prvi prolaz je ostalo:
// tabela literala (uradio valjda dobro; samo u drugom prolazu postavi adrese)
// tabela relokacionih zapisa (idk da l prvi ili drugi (vrv drugi))
// .equ :( (tns i ta sranja) takodje treba da doradis parser da moze da cita izraze za equ
// provera greske (uradjeno ali treba da se testira)
// !!!NOVO!!!
// fali provera da l je sve definisano (dodao)
// case sensitive (ovo najbolje u lexeru da resis) (dodao)
// equ :(
// endian (msm da se samo resilo, nisam siguran)
// dodaj da store neposredno bude greska (dodao)
//promeni call na push pa ld
// uslovni skok moze: proveris suprotan uslov, ako nije ispunjen propadnes na bezuslovni skok ako je ispunjen preskocis bezuslovni
// onda moras i drugu vrstu relok zapisa

int main(int argc,char** argv){

  char* in=nullptr;
  char* out=nullptr;
  for(int i=1;i<argc;i++){
    if(strcmp(argv[i],"-o")==0){
      if(i+1<argc){
        out=argv[i+1];
        i++;
      }
    }else in=argv[i];
    if(in!=nullptr && out!=nullptr) break;
  }

  char* temp;
  if(in==nullptr){printf("No input file"); exit(-5);}
  if(out==nullptr){
    temp=strdup(in);
    char* temp1=temp;
    while(*temp1!='.') temp1++;
    temp1++;*temp1='o';temp1++;*temp1='\0';
    out=temp;
  }

  instrHead=nullptr;
  instrTail=nullptr;
  FILE* myFile=fopen(in,"r");
  if(!myFile){
    printf("cant find file %s",in);
    return 2;
  }
  yyin=myFile;
  if(yyparse()) return 1;

  symbTable table;
  sectionTable sTable;
  
  instrHelp help;
  help.initInstr();
  int cnt=0;
  context* con=new context(&sTable,&table,nullptr,&cnt);
  int lineCnt=0;
  instruction *cur=instrHead;

  for(;cur;cur=cur->next){
    lineCnt++;
    std::string str(cur->name);
    if(help.getInstrType(cur->name)==-2){
      //ovde idemo ako nije instrukcija ili direktiva (znaci mora definicija simbola)
      if(cur->isLabel==1){
        if(!table.insertSymb(cur->name,sTable.getSectionId(sectionTable::curSection),2,cnt,false,true)){
          printf("Double definition of %s\n", cur->name);
          exit(-1);
          return -1;
        }
      }else {
        printf("Unrecognized instruction at line %d: %s\n",lineCnt,cur->name);
        exit(-2);
        return -2;
      }
    }else {
      //ovde idemo ako instrukcija ili direktiva

      if(!help.isValid(cur)) {
        printf("%s at line %d: arguments not valid",cur->name,lineCnt);
        exit(-3);
        return -3;
      }
      //treba da proverimo da li ima neki simbol u argumentima
      argument* curArg=cur->arg1;
      for(;curArg;curArg=curArg->next){
        char* n=help.getSymbolsFromArg(curArg);
        if(n) table.insertSymb(n,0,2,0,false,false);
        if(curArg->type==1){
          if(strcmp(cur->name,".equ")!=0){
            LPool* pool=sTable.getLPool(sectionTable::curSection);
            pool->insertLit(curArg->val1,0);
          }
        }
        if(curArg->type==2){
          if(sectionTable::curSection){
            LPool* pool=sTable.getLPool(sectionTable::curSection);
            pool->insertLit(curArg->name,0);
          }
        }
      }

      cnt+=help.getInstrSize(cur);

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
  table.isDefined();
  
  sTable.solvePools();
  table.printTable();
  printf("\n");
  sTable.printTable();
  sTable.printAllPools();
  sectionTable::curSection=nullptr;
  cur=instrHead;
  cnt=0;
  for(;cur;cur=cur->next){
    con->instr=cur;
    generated* gen=help.getCode(con);
    for(;gen;gen=gen->next){
      printf("%s:%x\n",cur->name,gen->code);
      sTable.addContent(sectionTable::curSection,gen->code);
    }
    cnt+=help.getInstrSize(cur);
    if(strcmp(cur->name,".section")==0){
        Events::sectionFP(&sTable,&table,cur->arg1->name,&cnt);  
    }

    if(strcmp(cur->name,".ascii")==0){
      Events::asciiSP(&sTable,cur);
    }

    if(strcmp(cur->name,".skip")==0){
      Events::skipSP(&sTable,cur);
    }

    if(strcmp(cur->name,".word")==0){
      Events::wordSP(&sTable,&table,cur,&cnt);
    }
   
  }

  sTable.printAllReloc();

  //sTable.printAllCode();

  FILE* file=fopen(out,"wb");

  //sTable.writeNext(0,out);
  //sTable.writeNext(1,out);
  //sTable.writeNextReloc(0,out);
  //table.writeTable(out,&sTable);
  printf("%d",sTable.getSizeOnDiskNext(0));
  int secCount=sTable.getSectionCnt()+1;
  fwrite(&secCount,4,1,file);
  int adr=8*secCount;
  int sz=table.getSizeOnDisk();
  fwrite(&adr,4,1,file);// offset do tabele simbola
  fwrite(&sz,4,1,file);// velicina tabele simbola

  for(int i=0;i<secCount-1;i++){
    adr=(secCount-1-i)*8;
    adr+=table.getSizeOnDisk();
    sz=sTable.getSizeOnDiskNext(i);
    fwrite(&adr,4,1,file);// offset do pocetka sekcije
    fwrite(&sz,4,1,file);// velicina sekcije
  }

  table.writeTable(file,&sTable);
  for(int i=0;i<secCount-1;i++){
    adr=sTable.getSizeOnDiskNext(i);
    fwrite(&adr,4,1,file);// offset do tabele relokacionih zapisa
    sTable.writeNext(i,file);
    sTable.writeNextReloc(i,file);
  }

  FILE* file1=fopen("test1.o","wb");

  sTable.writeNext(0,file1);

  return 0;
}