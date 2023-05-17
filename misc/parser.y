%{
  
  #include "../inc/instruction.h"
  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>

  extern int yylex();
  extern int yyparse();
  extern FILE *yyin;
 
  void yyerror(const char *s);
  
%}

%union{
  int num;
  char* str;
  struct argument* arg;
}

%token<num> NUMBER
%token<str> STRING
%type<arg> ARGUMENT

%token EOL
%token LSQR
%token RSQR
%token COMMA
%token PLUS
%token PRC
%token DOLLAR
%token REG
%token DD

%%
program:
  | line program;

line:
  EOL {

  }
  | STRING EOL {
    makeInstr($1,NULL,0);
    printf("Found instr %s with 0 args\n", $1);
  }
  | STRING ARGUMENT {
    makeInstr($1,$2,0);
  }
  | STRING {
    makeInstr($1,NULL,0);
    printf("Found instr %s with 0 args\n", $1);
  }
  | STRING DD line {
    makeInstr($1,NULL,1);  
  };

ARGUMENT:
  DOLLAR NUMBER EOL {
    $$=makeArg(1,1,NULL,$2,-1,NULL);
    printf("Found a arg with lit and dir value:%d\n",$2);
  }
  | DOLLAR STRING EOL {
    $$=makeArg(2,1,$2,-1,-1,NULL);
    printf("Found a arg with symb and dir name:%s\n",$2);
  }
  | NUMBER EOL {
    $$=makeArg(1,2,NULL,$1,-1,NULL);
    printf("Found a arg with lit and memDir value:%d\n",$1);
  }
  | STRING EOL {
    $$=makeArg(2,2,$1,-1,-1,NULL);
    printf("Found a arg with symb and Memdir name:%s\n",$1);
  }
  | REG NUMBER EOL {
    $$=makeArg(0,5,NULL,$2,-1,NULL);
    printf("found reg %d\n",$2);
  }
  | LSQR REG NUMBER RSQR EOL {
    $$=makeArg(0,6,NULL,$3,-1,NULL);
    printf("regMemInd reg%d\n",$3);
  }
  | LSQR REG NUMBER PLUS NUMBER RSQR EOL{
    $$=makeArg(0,7,NULL,$3,$5,NULL);
    printf("regIndOff reg:%d off:%d\n",$3,$5);
  }
  | LSQR REG NUMBER PLUS STRING RSQR EOL {
    $$=makeArg(0,7,$5,$3,-1,NULL);
    printf("regIndOff reg:%d symb:%s\n",$3,$5);
  }
  | PRC STRING EOL {
    $$=makeArg(0,5,$2,-1,-1,NULL);
    printf("found sys reg %s\n",$2);
  }
  | LSQR PRC STRING RSQR EOL {
    $$=makeArg(0,6,$3,-1,-1,NULL);
    printf("regMemInd sys reg%s\n",$3);
  }
  | LSQR PRC STRING PLUS NUMBER RSQR EOL {
    $$=makeArg(0,7,$3,-1,$5,NULL);
    printf("regIndOff sys reg:%s off:%d\n",$3,$5);
  }
  | LSQR PRC STRING PLUS STRING RSQR EOL {
    $$=makeArg(0,7,$5,-1,-1,NULL);
    printf("regIndOff reg:%s symb:%s\n",$3,$5);
  }
  | DOLLAR NUMBER COMMA ARGUMENT {
    $$=makeArg(1,1,NULL,$2,-1,$4);
    printf("Found a arg with lit and dir value:%d\n",$2);
  }
  | DOLLAR STRING COMMA ARGUMENT {
    $$=makeArg(2,1,$2,-1,-1,$4);
    printf("Found a arg with symb and dir name:%s\n",$2);
  }
  | NUMBER COMMA ARGUMENT {
    $$=makeArg(1,2,NULL,$1,-1,$3);
    printf("Found a arg with lit and memDir value:%d\n",$1);
  }
  | STRING COMMA ARGUMENT {
    $$=makeArg(2,2,$1,-1,-1,$3);
    printf("Found a arg with symb and Memdir name:%s\n",$1);
  }
  | REG NUMBER COMMA ARGUMENT {
    $$=makeArg(0,5,NULL,$2,-1,$4);
    printf("found reg %d\n",$2);
  }
  | LSQR REG NUMBER RSQR COMMA ARGUMENT {
    $$=makeArg(0,6,NULL,$3,-1,$6);
    printf("regMemInd reg%d\n",$3);
  }
  | LSQR REG NUMBER PLUS NUMBER RSQR COMMA ARGUMENT{
    $$=makeArg(0,7,NULL,$3,$5,$8);
    printf("regIndOff reg:%d off:%d\n",$3,$5);
  }
  | LSQR REG NUMBER PLUS STRING RSQR COMMA ARGUMENT {
    $$=makeArg(0,7,$5,$3,-1,$8);
    printf("regIndOff reg:%d symb:%s\n",$3,$5);
  }
  | PRC STRING COMMA ARGUMENT {
    $$=makeArg(0,5,$2,-1,-1,$4);
    printf("found sys reg %s\n",$2);
  }
  | LSQR PRC STRING RSQR COMMA ARGUMENT {
    $$=makeArg(0,6,$3,-1,-1,$6);
    printf("regMemInd sys reg%s\n",$3);
  }
  | LSQR PRC STRING PLUS NUMBER RSQR COMMA ARGUMENT {
    $$=makeArg(0,7,$3,-1,$5,$8);
    printf("regIndOff sys reg:%s off:%d\n",$3,$5);
  }
  | LSQR PRC STRING PLUS STRING RSQR COMMA ARGUMENT {
    $$=makeArg(0,7,$5,-1,-1,$8);
    printf("regIndOff reg:%s symb:%s\n",$3,$5);
  };

%%


void yyerror(const char *s) {
  printf("parser error %s",s);
  // might as well halt now:
  exit(-1);
}