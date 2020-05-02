%{
    #include<stdio.h>
    #include<string.h>
    #include <stdlib.h>

    extern int yy_scan_string();
    extern int yylex_destroy();
    extern FILE *yyin;
    extern int yylex();

    int yyerror( char *s){ return 1; }
%}


%start s
%% 
    s:  'a' b    
        ;
    b:  'b' 'c' c
        ;
    c: | 'c' {/*Yaac doesn't accept grammars with more than 1 lookahead symbol
                Therefore, the grammar S -> aBc, B -> b | aBc
                is not valid in yaac */
             }
        ;
%%



int main(int argc, char **argv) {
    FILE    *fd;

    if (argc != 2) {
        printf("Incorrect number of attributes. Aborting\n");
        return -1;
    }
    if (!(fd = fopen(argv[1], "r"))){
        printf("Error: \n");
        return -1;
    }
    char * line = NULL;
    size_t lenLine;
    int isRefused;
    lenLine= 0;
    while (getline(&line, &lenLine, fd) != -1) {
        printf("%s", line);
        yy_scan_string(line);
        isRefused = yyparse();
        printf("%sAccepted\n",  (isRefused ? "No ": ""));
        yylex_destroy();
    }
    fclose(fd);
    if (line)  free(line);
	return 0;
}