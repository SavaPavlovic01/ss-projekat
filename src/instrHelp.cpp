#include "../inc/instrHelp.hpp"
#include <iterator>
#include <string.h>


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
    return (std::string(instr->arg1->name).size()-2);
  }
  if(str.compare("st")==0){
    if(instr->arg1->next->type==0 && instr->arg1->next->mode==6) return 4;
    else return 16;
  }
  if(str.compare("ld")==0){
    if((instr->arg1->type==0 && instr->arg1->mode==7)) return 16;
    if((instr->arg1->type==1 && instr->arg1->mode==2)) return 8;
    else return 4;
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

generated* instrHelp::getCode(context* context)
{
  instruction* instr=context->instr;
  std::string str(instr->name);
  std::map<std::string, instrInfo*>::iterator itr=instrMap.find(str);
  if(itr==instrMap.end()) return nullptr;
  if(itr->second->generateCode==nullptr) return nullptr;
  return itr->second->generateCode(context);   
}

bool instrHelp::isValid(instruction* instr){
  instrInfo* info=instrHelp::getAllInstrInfro(instr->name);
  argument* arg=instr->arg1;
  int cnt=0;
  if(strcmp(instr->name,"st")==0){
    if(instr->arg1->next->type==2 && instr->arg1->next->mode==1) return false;
    if(instr->arg1->next->type==1 && instr->arg1->next->mode==1) return false;
    return true;
  }
  if(info->argCnt!=-1){
    for(;arg;arg=arg->next){
      cnt++;
      if(cnt>info->argCnt) return false;
      if(cnt==1) {
        if(info->arg1Type==0) continue;
        if(info->arg1Type==1 && arg->mode==5) continue; 
        if(info->arg1Type==2 && arg->type==3) continue;
        if(info->arg1Type==3 && arg->type==2) continue;
        if(info->arg1Type==4 && arg->type==1) continue;
        if(info->arg1Type==5 && (arg->type==1 || arg->type==2)) continue;
        if(info->arg1Type==6 && arg->mode==2) continue;
        return false;
      }
      if(cnt==2) {
        if(info->arg2Type==0) continue;
        if(info->arg2Type==1 && arg->mode==5) continue; 
        if(info->arg2Type==2 && arg->type==3) continue;
        if(info->arg2Type==3 && arg->type==2) continue;
        if(info->arg2Type==4 && arg->type==1) continue;
        if(info->arg2Type==5 && (arg->type==1 || arg->type==2)) continue;
        if(info->arg2Type==6 && arg->mode==2) continue;
        return false;
      }
      if(cnt==3) {
        if(info->arg3Type==0) continue;
        if(info->arg3Type==1 && arg->mode==5) continue; 
        if(info->arg3Type==2 && arg->type==3) continue;
        if(info->arg3Type==3 && arg->type==2) continue;
        if(info->arg3Type==4 && arg->type==1) continue;
        if(info->arg3Type==5 && (arg->type==1 || arg->type==2)) continue;
        if(info->arg3Type==6 && arg->mode==2) continue;
        return false;
      }
    }
    return true;
  }
  for(;arg;arg=arg->next){
    if(info->arg1Type==3 && arg->type==2) continue; 
    if(info->arg1Type==5 && (arg->type==1 || arg->type==2)) continue; 
    return false;
  }
  return true;
}

void instrHelp::initInstr(){
  // neke instrukcije mozda nemaju 1-1 slikanje u masinsku pa ce neke mozda morati vise od 4
  instrInfo* info=new instrInfo(0,0,4,-1,-1,-1,&codeGen::halt);
  instrMap["halt"]=info;
  
  instrMap["int"]=new instrInfo(0,0,4,-1,-1,-1,&codeGen::interupt);
  instrMap["iret"]=new instrInfo(0,0,8,-1,-1,-1,&codeGen::iret);
  instrMap["call"]=new instrInfo(1,0,4,6,-1,-1,&codeGen::call);
  instrMap["ret"]=new instrInfo(0,0,4,-1,-1,-1,&codeGen::ret);
  instrMap["jmp"]=new instrInfo(1,0,4,6,-1,-1,&codeGen::jmp);
  instrMap["beq"]=new instrInfo(3,0,4,1,1,6,&codeGen::beq);
  instrMap["bne"]=new instrInfo(3,0,4,1,1,6,&codeGen::bne);
  instrMap["bgt"]=new instrInfo(3,0,4,1,1,6,&codeGen::bgt);
  instrMap["push"]=new instrInfo(1,0,4,1,-1,-1,&codeGen::push);
  instrMap["pop"]=new instrInfo(1,0,4,1,-1,-1,&codeGen::pop);
  instrMap["xchg"]=new instrInfo(2,0,4,1,1,-1,&codeGen::xchg);
  instrMap["add"]=new instrInfo(2,0,4,1,1,-1,&codeGen::add);
  instrMap["sub"]=new instrInfo(2,0,4,1,1,-1,&codeGen::sub);
  instrMap["mul"]=new instrInfo(2,0,4,1,1,-1,&codeGen::mul);
  instrMap["div"]=new instrInfo(2,0,4,1,1,-1,&codeGen::div);
  instrMap["not"]=new instrInfo(1,0,4,1,-1,-1,&codeGen::bitNot);
  instrMap["and"]=new instrInfo(2,0,4,1,1,-1,&codeGen::bitAnd);
  instrMap["or"]=new instrInfo(2,0,4,1,1,-1,&codeGen::bitOr);
  instrMap["xor"]=new instrInfo(2,0,4,1,1,-1,&codeGen::bitXor);
  instrMap["shl"]=new instrInfo(2,0,4,1,1,-1,&codeGen::shl);
  instrMap["shr"]=new instrInfo(0,0,4,1,1,-1,&codeGen::shr);
  instrMap["ld"]=new instrInfo(2,0,-1,0,1,-1,&codeGen::ld);
  instrMap["st"]=new instrInfo(2,0,-1,1,0,-1,&codeGen::st);
  instrMap["csrrd"]=new instrInfo(2,0,4,2,1,-1,&codeGen::csrrd);
  instrMap["csrwr"]=new instrInfo(2,0,4,1,2,-1,&codeGen::csrwr);

  instrMap[".global"]=new instrInfo(-1,1,0,3,-1,-1,nullptr);
  instrMap[".extern"]=new instrInfo(-1,1,0,3,-1,-1,nullptr);
  instrMap[".section"]=new instrInfo(1,1,0,3,-1,-1,nullptr);
  instrMap[".word"]=new instrInfo(-1,1,-1,5,-1,-1,nullptr); // -1 znaci da je promenljiva i mora da se izracuna
  instrMap[".skip"]=new instrInfo(1,1,-1,4,-1,-1,nullptr);
  instrMap[".ascii"]=new instrInfo(1,1,-1,3,-1,-1,nullptr);
  instrMap[".end"]=new instrInfo(0,1,0,-1,-1,-1,nullptr);
  instrMap[".equ"]=new instrInfo(2,1,0,3,4,-1,nullptr);
}