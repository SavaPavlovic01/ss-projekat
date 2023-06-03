%{
  #include "parser.tab.h"
  #include <stdio.h>
  #include <stdlib.h>
  #include <ctype.h>
%}

%option noyywrap


%%
[ \t] {}

".equ" {yylval.str=strdup(yytext);return EQU;}

[A-Za-z_\.][A-Za-z0-9_]* {
  char* temp=yytext;
  while(*temp!='\0'){
    *temp=tolower(*temp);
    temp++;
  }
  yylval.str=strdup(yytext);
  
  return STRING;
}

0[xX][0-9A-Fa-f]+ {
  sscanf(yytext,"%x",&yylval.num);
  return NUMBER;
  
}

[0-9]+ {
  yylval.num=atoi(yytext);
  return NUMBER;
}
\"[^"]+\" {
  yylval.str=strdup(yytext);
  return STRING;  
}

"#"[^\n\r]* {}

[\n\r] {return EOL;}

"[" {return LSQR;}

"-" {return MINUS;}

"]" {return RSQR;}

"," {return COMMA;}

"+" {return PLUS;}

"$" {return DOLLAR;}

"%"[rR] {return REG;}

"%" {return PRC;}

":" {return DD;}
%%