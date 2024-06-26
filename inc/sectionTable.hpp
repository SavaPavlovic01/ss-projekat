#ifndef _sectionTable_hpp_
#define _sectionTable_hpp_

#include <map>
#include <string>
#include "LPool.hpp"
#include "relocTable.hpp"

// u prvom prolazu postavis sva polja
// u drugom emitujes sadrzaj sekcije (sta god to znacilo)

typedef struct cc{
  int code;
  char byte;
  int type;
  cc(int code,char byte,int type):code(code),byte(byte),type(type){}
}cc;

typedef struct sectionTableItem{
  char* name;
  int base;
  int len;
  int cnt;
  LPool* pool;
  relocTable* table;
  std::vector<cc*>* content;
  sectionTableItem(char* name,int base,int len,int cnt):name(name),base(base),len(len),cnt(cnt) {pool=new LPool();
    table=new relocTable();content=new std::vector<cc*>();}
}sectionTableItem;

class sectionTable{

  std::map<std::string,sectionTableItem*> map;
  int cnt=0;

  public:

    static char* curSection;

    bool insertSection(char* name,int base,int len);

    void insertSection(char* name,int base,int len,int ndx);

    sectionTableItem* getSection(char* name);

    int getSectionCnt() {return map.size();}

    bool setLen(char* name,int len);

    void printTable();   

    int getSectionId(char* name);

    LPool* getLPool(char* name); 

    LPool* getLPool(int num); 

    void printPool(char* name); 

    void printAllPools();

    void printAllReloc();

    void solvePools();

    relocTable* getRelocTable(char* name);

    relocTable* getRelocTableI(int num);

    void addContent(char* name,int code);

    void addContent(char* name,char byte);
    
    void printCode(sectionTableItem* item);

    void printAllCode();

    void writeSection(char* name,FILE* file);

    void writeNext(int num,FILE* file);

    void writeNextReloc(int num,FILE* file);

    int getSizeOnDiskNext(int num);

    int getSizeOnDiskSection(sectionTableItem* item);

    void writeTable(FILE* file);

    int getSize();

    void addContent(int num,char byte);

    relocTable* getRelocTable(int num);

    sectionTableItem* getSection(int num);

    static void mergeSections(sectionTableItem* itemD,sectionTableItem* itemS);

    char* getSectionNameById(int id);

};

#endif