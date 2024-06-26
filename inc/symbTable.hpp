#ifndef _symbTable_hpp_
#define _symbTable_hpp_
//#include "instrHelp.hpp"
#include <unordered_map>
#include "sectionTable.hpp"

// .global u prvom prolazu samo postavlja globalDef na true i postavlja vezivanje(type kod tebe) na globalno
// u drugom samo proveri da li je definisano sve
// .extern postavlja vezivanje na globalno i u redu je ako nije definisano nako drugog prolaza


typedef struct symbTableItem {
  char* name;
  int section;
  int type;
  int value;
  bool globalDef;
  bool isDef;
  symbTableItem(char* n,int s,int t,int v,bool g,bool isDef): name(n),section(s),type(t),value(v),globalDef(g),isDef(isDef){}
} symbTableItem;

class symbTable {
  private:
    std::unordered_map<std::string,symbTableItem*> table;

  public:
    bool insertSymb(symbTableItem*);

    int getSymbCnt(){return table.size();}

    bool insertSymb(char* name,int section,int type,int value,bool globalDef,bool isDef);

    /*
    * Returns symbol by name 
    * If the symbol doesnt exist, returns null
    */
    symbTableItem* getSymb(char* name);

    symbTableItem* getSymb(int num);

    int setGlobalDef(char* name,bool val);

    int getSection(char* name);

    int getValue(char* name);

    void printTable();

    void writeTable(FILE* file,sectionTable* secTable);

    int getSizeOnDisk();

    bool isDefined();

};

#endif