#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAX_REGISTERS 128
/*Consideraciones:

GLC: E ->  int | ( E ) | E + E | E - E | E * E
Obervese que solo existen operaciones unarias o binarias en esta gramatica.
EL AST es un arbol binario, por ende.
Este arbol binario puede ser incluso representado con un heap, o en el caso
de estas operaciones, con un array en notacion postfija.
Por ende, nuestro arbol abstracto esta representado por un array.

*/

int createAST(int n, char * inputText, char *output,int * outIndx){
    (*outIndx)=0;
    int digits=0;
    int *stk = malloc(n*sizeof(char));
    int sIndx=0;
    int done=0;
    for(int i=0;i<n;i++){
        if(done) break;
        char c= inputText[i];
        switch (c) {
        case '\t':
        case ' ':
            continue;
            break;
        case '\n':
            done=1;
            break;
        case '+':
        case '-':
            digits=0;
            while (1){
                if( sIndx==0 || stk[sIndx-1] == '('){
                    stk[sIndx++] = c;
                    break; 
                }else{
                    output[(*outIndx)++]=stk[--sIndx];
                }
            }     
            break;
        case '*':
        case '(':
            digits=0;
            stk[sIndx++] = c;
            break;
        case ')':
            digits=0;
            while (1){
                
                if( sIndx==0) return -1;
                char op = stk[--sIndx]; 
                if(op=='(') break;
                output[(*outIndx)++]=op;
                
            }   
            break;
        default:
            if(c==' ' || c=='\t') continue;
            if(c<'0' || c>'9') return -1;
            if(digits) return -1;
            output[(*outIndx)++]=c;
            digits++;
            break;
        }
    }
    while(sIndx>0) {
		char op = stk[--(sIndx)];
		if(op=='(') return -1;
		output[(*outIndx)++] = op;
	}
	return 0;
}

int createInstructions(int n, char *input){
    short regUsed[MAX_REGISTERS];
    for(int s=0;s<MAX_REGISTERS;s++){ regUsed[s] = 0; }
    int *stk = malloc(n*sizeof(char));
    int sIndx=0;
    int currentReg=0;
    for (int i=0;i<n; i++){
        char c=input[i];
        if(c>='0' && c<='9'){
            stk[sIndx++] = c-'0';
        }else{
            char *operation;
            switch (c)
            {
            case '+':
                operation="add";
                break;
            case '-':
                operation="sub";
                break;
            default:
                operation="mul";
                break;
            }
            int val2=  stk[--sIndx];
            int val1 = stk[--sIndx];
            char * v2,*v1;
            char* acc="accm";
            if(val2>=10){
                 v2= acc;
                 regUsed[val2-10] =0;
            }else{
                v2 = (char *)malloc(sizeof(char) *2 );
                v2[0]= ((char)(val2+'0'));
                v2[1]= 0;
            }
            if(val1>=10){
                 v1= acc;
                 regUsed[val1-10] =0;
            }else{
                v1 = (char *)malloc(sizeof(char) *2 );
                v1[0]= ((char)(val1+'0'));
                v1[1]= 0;
            }

            for(int s=0;s<MAX_REGISTERS;s++){ 
                if(regUsed[s] == 0){
                    stk[sIndx++] = 10+s;
                    regUsed[s] = 1;
                    currentReg=s;
                    break;
                }
            }
            if(val2>=10){
                
                if(val1>=10){
                    printf("%s &%s%d, &%s%d, %s%d\n", operation, v1,val1-10, v2,val2-10, acc,currentReg);
                }else{
                    printf("%s %s, &%s%d, %s%d\n", operation, v1, v2,val2-10, acc,currentReg);
                }
            }else{
                if(val1>=10){
                    printf("%s &%s%d, %s, %s%d\n", operation,  v1,val1-10,v2, acc,currentReg);
                }else{
                    printf("%s %s, %s, %s%d\n", operation, v1, v2, acc, currentReg);
                }
            }
        }
    }
}

int main(int argc, char ** argv){
    if(argc!=2){
        printf("Incorrect number of arguments. Please add the file path to analyze. Aborting\n");
        return -1;
    }
    FILE *fd;
    fd = fopen(argv[1], "r");
    if(fd==NULL){
        printf("File doesn't exist. Aborting\n");
        return -1;
    }
    char line[1024];
    while(fgets(line, 1024, fd)!=NULL){
        int n= strlen(line);
        int outIndx = 0;
        for(int q=0;q<n;q++){
                if(line[q]=='\n') break;
                printf("%c", line[q]);
            }printf("\n");
        char * output =(char *) malloc((n+1)*sizeof(char));
        if(createAST( n, line, output, &outIndx)==0){
            for(int i=0;i<outIndx;i++){
                printf("%c", output[i]);
            }printf("\n\n");
            createInstructions(outIndx,output);
        }else{
            printf("There is a sintax error in the sentence.\n");
        }
        printf("\n");
    }
}