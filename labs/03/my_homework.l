%{
#include <stdio.h>
#define NO_ERRORS 0
#define COMMENTS 1
#define ERR 2
char state =NO_ERRORS;
%}

int     -?[0-9]+
float   -?[0-9]+\.[0-9]{1,5} 
var      [a-e]|[gh]|[j-o]|[q-z]
number   {float}|{int} 

%%

{float}                 { if(state==NO_ERRORS){
                            printf("fnum ");
                            }
                        }
{int}                   { if(state==NO_ERRORS) {
                            printf("inum ");
                            }
                        }

{var}                   { if(state==NO_ERRORS) {
                            printf("id "); 
                            }
                        }

"="                     { if(state==NO_ERRORS){ 
                            printf("assign "); 
                            }
                        }
"+"                     { if(state==NO_ERRORS){ 
                            printf("plus "); 
                            }
                        }
"-"                     { if(state==NO_ERRORS){ 
                                printf("minus "); 
                            }
                        }
"*"                     { if(state==NO_ERRORS){ 
                                printf("multiply "); 
                            } 
                        }
"/"                     { if(state==NO_ERRORS){ 
                                printf("divide "); 
                            }
                        }


[ \t]+                  {  //blank spaces
                          if(state==ERR) state=NO_ERRORS; }


f                       {if(state==NO_ERRORS){
                            printf("floatdcl ");
                            }
                        }

i                       {if(state==NO_ERRORS){
                            printf("intdcl "); 
                            }
                        }

p                       {if(state==NO_ERRORS){
                            printf("print "); 
                            }
                        }
"//"                    {if(state==NO_ERRORS){
                            printf("comment ");
                            state=1;
                            }
                        }

{int}\.           |
{int}\.[a-zA-Z]   |
{number}[a-zA-Z]  |
[a-zA-Z][^ \t\n]         { if(state==NO_ERRORS) {
                            state=ERR;
                            printf("err ");
                            }
                        }
"\n"                    { printf("\n");
                          state=NO_ERRORS;
                        }


.                      {if(state==NO_ERRORS) {
                            printf("err ");
                            state=ERR;
                        }}

%%

int main(int argc, char** argv){
    if(argc > 1){
        FILE *file;
        file = fopen(argv[1],"r");
        if(!file){
            fprintf(stderr, "fail open %s\n", argv[1]);
            exit(1);
        }
        yyin = file;
        
    }
    yylex();
    return 0;
}