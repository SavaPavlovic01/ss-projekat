#include "../inc/TNS.hpp"
#include <map>

void Tns::solveTns(symbTable* table){
  bool changed=true;
  bool good=true;
  std::map<int,int> mapHelp;
  while(changed){
    changed=false;
    int calc=0;
    mapHelp.clear();
    for(int i=0;i<tnsTable.size();i++){
      calc=0;
      argument* curArg=tnsTable[i]->arg;
      good=true;
      
      while(curArg){
        curArg=curArg->next;
        if(!curArg) break;
        if(curArg->name!=nullptr){
          symbTableItem* curItem=table->getSymb(curArg->name);
          if(!curItem) {good=false;break;}
          if(!curItem->isDef) {good=false;break;}
          if(curArg->val2==0) {
            calc+=curItem->value;
            std::map<int,int>::iterator itr=mapHelp.find(curItem->section);
            if(itr==mapHelp.end()){
              mapHelp[curItem->section]=1;
              continue;
            }
            itr->second++;
            continue;
          }
          if(curArg->val2==1){
            calc-=curItem->value;
            std::map<int,int>::iterator itr=mapHelp.find(curItem->section);
            if(itr==mapHelp.end()){
              mapHelp[curItem->section]=-1;
              continue;
            }
            itr->second--;
            continue;
          }
        }
        else {
          if(curArg->val2==0) calc+=curArg->val1;
          else calc-=curArg->val1;
          continue;
        }
      }

      if(!good) continue;
      int section=-1;
      for(std::map<int,int>::iterator itr=mapHelp.begin();itr!=mapHelp.end();itr++){
        if(itr->first==-1) continue;
        if(itr->second!=0){
          if(section!=-1){printf("LOSE RELOK\n"); exit(0);}
          section=itr->first;
        }
      }
      table->insertSymb(tnsTable[i]->name,section,0,calc,0,1);
      changed=true;
      std::vector<tnsEntry*>::iterator iter=tnsTable.begin();
      iter+=i;
      tnsTable.erase(iter);
    }

  }
}