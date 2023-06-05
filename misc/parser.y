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
%type<arg> EXPRESSION

%token EOL
%token LSQR
%token RSQR
%token COMMA
%token PLUS
%token PRC
%token DOLLAR
%token REG
%token DD
%token MINUS
%token<str> EQU

%%
program:
  | line program;

line:
  EOL {

  }
  | EQU STRING COMMA EXPRESSION {
    argument* arg=makeArg(2,1,$2,0,0,$4);
    makeInstr($1,arg,0);
  } 
  | STRING EOL {
    makeInstr($1,NULL,0);
    
  }
  | STRING ARGUMENT {
    makeInstr($1,$2,0);
  }
  | STRING {
    makeInstr($1,NULL,0);
    
  }
  | STRING DD operacija {
    makeInstr($1,NULL,1);  
  };

EXPRESSION:
  STRING EXPRESSION {
    $$=makeArg(2,1,$1,0,0,$2);
  }
  | NUMBER EXPRESSION {
    $$=makeArg(1,1,NULL,$1,0,$2);
  }
  | PLUS NUMBER EXPRESSION {
    $$=makeArg(1,1,NULL,$2,0,$3);
  }
  | MINUS NUMBER EXPRESSION {
    $$=makeArg(1,1,NULL,$2,1,$3);
  }
  | PLUS STRING EXPRESSION {
    $$=makeArg(2,1,$2,0,0,$3);
  }
  | MINUS STRING EXPRESSION {
    $$=makeArg(2,1,$2,0,1,$3);
  }
  | EOL {$$=NULL;};

operacija:
  EOL {

  }
  | STRING EOL {
    makeInstr($1,NULL,0);
  
  }
  | STRING ARGUMENT {
    makeInstr($1,$2,0);
  }
  | STRING {
    makeInstr($1,NULL,0);
    
  };  

ARGUMENT:
  DOLLAR NUMBER EOL {
    $$=makeArg(1,1,NULL,$2,-1,NULL);
    
  }
  | DOLLAR STRING EOL {
    $$=makeArg(2,1,$2,-1,-1,NULL);
    
  }
  | NUMBER EOL {
    $$=makeArg(1,2,NULL,$1,-1,NULL);
    
  }
  | STRING EOL {
    $$=makeArg(2,2,$1,-1,-1,NULL);
    
  }
  | REG NUMBER EOL {
    $$=makeArg(0,5,NULL,$2,-1,NULL);
    
  }
  | LSQR REG NUMBER RSQR EOL {
    $$=makeArg(0,6,NULL,$3,-1,NULL);
    
  }
  | LSQR REG NUMBER PLUS NUMBER RSQR EOL{
    $$=makeArg(0,7,NULL,$3,$5,NULL);
    
  }
  | LSQR REG NUMBER PLUS STRING RSQR EOL {
    $$=makeArg(0,7,$5,$3,-1,NULL);
    
  }
  | PRC STRING EOL {
    if(strcmp($2,"sp")==0){
      $$=makeArg(0,5,NULL,14,-1,NULL);  
      
    }
    if(strcmp($2,"pc")==0){
      $$=makeArg(0,5,NULL,15,-1,NULL);  
      
    }else{
      if(strcmp($2,"status")==0){
        $$=makeArg(3,5,NULL,0,-1,NULL);
      }
      if(strcmp($2,"handler")==0){
        $$=makeArg(3,5,NULL,1,-1,NULL);
      }
      if(strcmp($2,"cause")==0){
        $$=makeArg(3,5,NULL,2,-1,NULL);
      }
      
      
    }
    
  }
  | LSQR PRC STRING RSQR EOL {
    if(strcmp($3,"sp")==0){
      $$=makeArg(0,6,NULL,14,-1,NULL);  
      
    }
    if(strcmp($3,"pc")==0){
      $$=makeArg(0,6,NULL,15,-1,NULL);  
      
    }else{
      if(strcmp($3,"status")==0){
        
        $$=makeArg(3,6,NULL,0,-1,NULL);
      }
      if(strcmp($3,"handler")==0){
        $$=makeArg(3,6,NULL,1,-1,NULL);
      }
      if(strcmp($3,"cause")==0){
        $$=makeArg(3,6,NULL,2,-1,NULL);
      }
      
      
    }
  }
  | LSQR PRC STRING PLUS NUMBER RSQR EOL {
    if(strcmp($3,"sp")==0){
      $$=makeArg(0,7,NULL,14,$5,NULL);  
      
    }
    if(strcmp($3,"pc")==0){
      $$=makeArg(0,7,NULL,15,$5,NULL);  
      
    }else{
      if(strcmp($3,"status")==0){
        
        $$=makeArg(3,7,NULL,0,$5,NULL);  
      }
      if(strcmp($3,"handler")==0){
        $$=makeArg(3,7,NULL,1,$5,NULL);
      }
      if(strcmp($3,"cause")==0){
        $$=makeArg(3,7,NULL,2,$5,NULL);
      }
      
      
    }
  
  }
  | LSQR PRC STRING PLUS STRING RSQR EOL {
    if(strcmp($3,"sp")==0){
      $$=makeArg(0,7,$5,14,-1,NULL);  
      
    }
    if(strcmp($3,"pc")==0){
      $$=makeArg(0,7,$5,15,-1,NULL);  
      
    }else{
      if(strcmp($3,"status")==0){
        
        $$=makeArg(3,7,$5,0,-1,NULL);
      }
      if(strcmp($3,"handler")==0){
       $$=makeArg(3,7,$5,1,-1,NULL);
      }
      if(strcmp($3,"cause")==0){
        $$=makeArg(3,7,$5,2,-1,NULL);
      }
      
      
    }
  
  }
  | DOLLAR NUMBER COMMA ARGUMENT {
    $$=makeArg(1,1,NULL,$2,-1,$4);
    
  }
  | DOLLAR STRING COMMA ARGUMENT {
    $$=makeArg(2,1,$2,-1,-1,$4);
    
  }
  | NUMBER COMMA ARGUMENT {
    $$=makeArg(1,2,NULL,$1,-1,$3);
    
  }
  | STRING COMMA ARGUMENT {
    $$=makeArg(2,2,$1,-1,-1,$3);
    
  }
  | REG NUMBER COMMA ARGUMENT {
    $$=makeArg(0,5,NULL,$2,-1,$4);
    
  }
  | LSQR REG NUMBER RSQR COMMA ARGUMENT {
    $$=makeArg(0,6,NULL,$3,-1,$6);
    
  }
  | LSQR REG NUMBER PLUS NUMBER RSQR COMMA ARGUMENT{
    $$=makeArg(0,7,NULL,$3,$5,$8);
    
  }
  | LSQR REG NUMBER PLUS STRING RSQR COMMA ARGUMENT {
    $$=makeArg(0,7,$5,$3,-1,$8);
    
  }
  | PRC STRING COMMA ARGUMENT {
    if(strcmp($2,"sp")==0){
      $$=makeArg(0,5,NULL,14,-1,$4);  
      
    }
    if(strcmp($2,"pc")==0){
      $$=makeArg(0,5,NULL,15,-1,$4);  
      
    }else{
      if(strcmp($2,"status")==0){
        $$=makeArg(3,5,NULL,0,-1,$4);
      }
      if(strcmp($2,"handler")==0){
        $$=makeArg(3,5,NULL,1,-1,$4);
      }
      if(strcmp($2,"cause")==0){
        $$=makeArg(3,5,NULL,2,-1,$4);
      }
      
      
    }
  }
  | LSQR PRC STRING RSQR COMMA ARGUMENT {
    if(strcmp($3,"sp")==0){
      $$=makeArg(0,6,NULL,14,-1,$6);  
      
    }
    if(strcmp($3,"pc")==0){
      $$=makeArg(0,6,NULL,15,-1,$6);  
      
    }else{
      if(strcmp($3,"status")==0){
        
        $$=makeArg(3,6,NULL,0,-1,$6);
      }
      if(strcmp($3,"handler")==0){
        $$=makeArg(3,6,NULL,1,-1,$6);
      }
      if(strcmp($3,"cause")==0){
        $$=makeArg(3,6,NULL,2,-1,$6);
      }
      
      
    }
  }
  | LSQR PRC STRING PLUS NUMBER RSQR COMMA ARGUMENT {
    if(strcmp($3,"sp")==0){
      $$=makeArg(0,7,NULL,14,$5,$8);  
      
    }
    if(strcmp($3,"pc")==0){
      $$=makeArg(0,7,NULL,15,$5,$8);  
      
    }else{
      if(strcmp($3,"status")==0){
        
        $$=makeArg(3,7,NULL,0,$5,$8);  
      }
      if(strcmp($3,"handler")==0){
        $$=makeArg(3,7,NULL,1,$5,$8);
      }
      if(strcmp($3,"cause")==0){
        $$=makeArg(3,7,NULL,2,$5,$8);
      }
      
      
    }
  }
  | LSQR PRC STRING PLUS STRING RSQR COMMA ARGUMENT {
   if(strcmp($3,"sp")==0){
      $$=makeArg(0,7,$5,14,-1,$8);  
      
    }
    if(strcmp($3,"pc")==0){
      $$=makeArg(0,7,$5,15,-1,$8);  
      
    }else{
      if(strcmp($3,"status")==0){
        
        $$=makeArg(3,7,$5,0,-1,$8);
      }
      if(strcmp($3,"handler")==0){
       $$=makeArg(3,7,$5,1,-1,$8);
      }
      if(strcmp($3,"cause")==0){
        $$=makeArg(3,7,$5,2,-1,$8);
      }
      
      
    }
  };

%%


void yyerror(const char *s) {
  printf("parser error %s",s);
  // might as well halt now:
  exit(-1);
}