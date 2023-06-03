#ifndef _TNS_HPP_
#define _TNS_HPP_
#include "instruction.h"
#include "symbTable.hpp"
#include <vector>

typedef struct tnsEntry{
  char* name;
  argument* arg;

  tnsEntry(char* name,argument* arg):name(name),arg(arg){}
} tnsEntry;

class Tns{
  std::vector<tnsEntry*> tnsTable;

  public:
    void addEntry(char* name,argument* arg){tnsTable.push_back(new tnsEntry(name,arg));}

    void solveTns(symbTable* table);
};

#endif