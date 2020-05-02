%{
#include <stdio.h>
%}

%%
[ \t\n]
.           {return yytext[0];}
%%