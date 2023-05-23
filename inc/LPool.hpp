#ifndef _LPool_hpp_
#define _LPool_hpp_
#include <map>
#include <string>

typedef struct literalEntry
{
  int val;
  int adr;
  char* name;
  literalEntry(int val,int adr):val(val),adr(adr),name(nullptr){}
  literalEntry(char* name,int adr):val(0),adr(adr),name(name){}
} literalEntry;


class LPool{
  std::map<int,literalEntry*> litMap;
  std::map<std::string,literalEntry*> symbMap;
  public:

    void setAdr(int key,int adr);

    void setAdr(char* key,int adr);

    literalEntry* getEntry(int key);

    literalEntry* getEntry(char* key);

    int getAdr(int key);

    int getAdr(char* key);

    bool insertLit(int key,int adr);

    bool insertLit(char* key,int adr);

    void printPool();

    void solve(int start);

    void writePool(FILE* file);

    int sizeOfPool();
};

#endif