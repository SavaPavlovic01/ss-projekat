#ifndef _LPool_hpp_
#define _LPool_hpp_
#include <map>

typedef struct literalEntry
{
  int val;
  int adr;

  literalEntry(int val,int adr):val(val),adr(adr){}
} literalEntry;


class LPool{
  std::map<int,literalEntry*> litMap;

  public:

    void setAdr(int key,int adr);

    literalEntry* getEntry(int key);

    int getAdr(int key);

    bool insertLit(int key,int adr);

    void printPool();

    void solve(int start);
};

#endif