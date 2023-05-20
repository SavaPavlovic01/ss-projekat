#ifndef _relocTable_hpp_
#define _relocTable_hpp_
#include <vector>

typedef struct relocEntry{
  int offset;
  int type;
  int symbol;
  int addend;
  char* name;
  relocEntry(int offset,int type,int symbol,int addend):offset(offset),type(type),symbol(symbol),addend(addend),name(nullptr) {}
  relocEntry(int offset,int type,char* name,int addend):offset(offset),type(type),name(name),addend(addend),symbol(-5) {}
}relocEntry;

class relocTable{
  std::vector<relocEntry*> vector;

  public:
    void addEntry(int offset,int type,int symbol,int addend);

    void addEntry(int offset,int type,char* name,int addend);

    void printTable();
};

#endif