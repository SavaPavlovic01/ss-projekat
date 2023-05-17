#include "../inc/instrHelp.hpp"
#include <iterator>

std::map<std::string,instrInfo*> instrHelp::instrMap={};
/*
* 0 is instruction
* 1 is directive
* 2 is symbol
* -2 is when name is not in map
*/
int instrHelp::getInstrType(char* name){
  std::string str(name);
  std::map<std::string, instrInfo*>::iterator itr=instrMap.find(str);
  if(itr==instrMap.end()) return -2;
  else return itr->second->type;
}

int instrHelp::getInstrArgNum(char* name){
  //case sensitive!!!!!!!!!!!!
  std::string str(name);
  std::map<std::string, instrInfo*>::iterator itr=instrMap.find(str);
  if(itr==instrMap.end()) return -2;
  else return itr->second->argCnt;
}

char* instrHelp::getSymbolsFromArg(argument* arg){
  if(arg->type==2) return arg->name;
  else return nullptr;
}

instrInfo* instrHelp::getAllInstrInfro(char* name){
  std::string str(name);
  std::map<std::string, instrInfo*>::iterator itr=instrMap.find(str);
  if(itr==instrMap.end()) return nullptr;
  else return itr->second;  
}

int instrHelp::calcSize(instruction* instr){
  std::string str(instr->name);
  if(str.compare(".word")==0){
    int cnt=0;
    for(argument* arg=instr->arg1;arg;arg=arg->next) cnt++;
    return 4*cnt;
  }
  if(str.compare(".skip")==0){
    return instr->arg1->val1;
  }
  if(str.compare(".ascii")==0){
    return (std::string(instr->arg1->name).size()-2)*8;
  }

  return -1;
}

int instrHelp::getInstrSize(instruction* instr){
  std::string str(instr->name);
  std::map<std::string, instrInfo*>::iterator itr=instrMap.find(str);
  if(itr==instrMap.end()) return -2;
  if(itr->second->size==-1){
    return calcSize(instr);
  }
  else return itr->second->size;  
}



void instrHelp::initInstr(){
  // neke instrukcije mozda nemaju 1-1 slikanje u masinsku pa ce neke mozda morati vise od 4
  instrMap["halt"]=new instrInfo(0,0,4);
  instrMap["int"]=new instrInfo(0,0,4);
  instrMap["iret"]=new instrInfo(0,0,4);
  instrMap["call"]=new instrInfo(1,0,4);
  instrMap["ret"]=new instrInfo(0,0,4);
  instrMap["jmp"]=new instrInfo(1,0,4);
  instrMap["beq"]=new instrInfo(3,0,4);
  instrMap["bne"]=new instrInfo(3,0,4);
  instrMap["bgt"]=new instrInfo(3,0,4);
  instrMap["push"]=new instrInfo(1,0,4);
  instrMap["pop"]=new instrInfo(1,0,4);
  instrMap["xchg"]=new instrInfo(2,0,4);
  instrMap["add"]=new instrInfo(2,0,4);
  instrMap["sub"]=new instrInfo(2,0,4);
  instrMap["mul"]=new instrInfo(2,0,4);
  instrMap["div"]=new instrInfo(2,0,4);
  instrMap["not"]=new instrInfo(1,0,4);
  instrMap["and"]=new instrInfo(2,0,4);
  instrMap["or"]=new instrInfo(2,0,4);
  instrMap["xor"]=new instrInfo(2,0,4);
  instrMap["shl"]=new instrInfo(2,0,4);
  instrMap["shr"]=new instrInfo(0,0,4);
  instrMap["ld"]=new instrInfo(2,0,4);
  instrMap["st"]=new instrInfo(2,0,4);
  instrMap["csrrd"]=new instrInfo(2,0,4);
  instrMap["csrwr"]=new instrInfo(2,0,4);

  instrMap[".global"]=new instrInfo(-1,1,0);
  instrMap[".extern"]=new instrInfo(-1,1,0);
  instrMap[".section"]=new instrInfo(0,1,0);
  instrMap[".word"]=new instrInfo(-1,1,-1); // -1 znaci da je promenljiva i mora da se izracuna
  instrMap[".skip"]=new instrInfo(1,1,-1);
  instrMap[".ascii"]=new instrInfo(1,1,-1);
  instrMap[".end"]=new instrInfo(0,1,0);
  instrMap[".equ"]=new instrInfo(2,1,0);
}