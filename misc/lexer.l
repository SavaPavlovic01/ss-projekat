%{
  #include "parser.tab.h"
  #include <stdio.h>
  #include <stdlib.h>
%}

%option noyywrap


%%
[ \t] {}

[A-Za-z_\.][A-Za-z0-9_]* {
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

"]" {return RSQR;}

"," {return COMMA;}

"+" {return PLUS;}

"$" {return DOLLAR;}

"%"[rR] {return REG;}

"%" {return PRC;}

":" {return DD;}
%%