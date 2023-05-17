#ifndef _sectionTable_hpp_
#define _sectionTable_hpp_

#include <map>
#include <string>

// u prvom prolazu postavis sva polja
// u drugom emitujes sadrzaj sekcije (sta god to znacilo)

typedef struct sectionTableItem{
  char* name;
  int base;
  int len;
  int cnt;

  sectionTableItem(char* name,int base,int len,int cnt):name(name),base(base),len(len),cnt(cnt) {}
}sectionTableItem;

class sectionTable{

  std::map<std::string,sectionTableItem*> map;
  int cnt=0;

  public:

    static char* curSection;

    bool insertSection(char* name,int base,int len);

    sectionTableItem* getSection(char* name);

    bool setLen(char* name,int len);

    void printTable();   

    int getSectionId(char* name);   

};

#endif