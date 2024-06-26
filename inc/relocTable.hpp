#ifndef _relocTable_hpp_
#define _relocTable_hpp_
#include <vector>
#include <stdlib.h>
#include <stdio.h>

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

    void writeTable(FILE* file);

    int getSizeOnDisk();

    int getEntryCnt(){return vector.size();}

    relocEntry* getEntry(int num);

    void updateTable(int increment);

    static void mergeReloc(relocTable* dest,relocTable* source);
};

#endif