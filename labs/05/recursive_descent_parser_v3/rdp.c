#include <stdio.h>
#include <stdlib.h>

struct ways{
    int indxs[10]; // this should be, in a real application, a struct such a list, vector, etc.
    int instructions[10];
    int len;
};

void B(char * word, struct ways * indx, int * len);
char parser(char * word, int  len, int id);
void S(char * word, struct ways  *caminos, int * len);
void match(char x, char * word, struct ways * caminos, int * len);
void unionElem(struct ways * caminos,struct ways * extra);
void copyWays(struct ways * src, struct ways * dest);
void addInstruction( struct ways * caminos, int i);
void generateDotFile(char * word, int wordLen, int bitInstructions, int id);

char instructions[5][13] = {"    1 -> 2;\n\0","    1 -> 3;\n\0","    1 -> 4;\n\0","    3 -> 5;\n\0","    3 -> 6;\n\0" };
// con el array de index, hacer array de bits de instrucciones. modificar para cada match.
char parser(char * word, int  wordLen, int id){
    struct ways caminos;
    caminos.indxs[0] =0;
    caminos.instructions[0] = 0;
    caminos.len=1;
    S(word, &caminos,  &wordLen);
    for(int i=0;i<caminos.len;i++){
        if(caminos.indxs[i] == wordLen) 
        {
            generateDotFile(word, wordLen,  caminos.instructions[i], id);
            return 1;
        }
    }
    return 0;
}
void generateDotFile(char * word, int wordLen, int bitInstructions, int id){
    FILE *file;
    char buff[30]; 
    snprintf(buff, 30, "derivation_tree%d.dot", id);
    file=fopen(buff, "w");
    fprintf(file, "digraph G{\n");   
    fprintf(file, "    1 [label=\"S\"];\n");
    fprintf(file, "    2 [label=\"a\"];\n");
    fprintf(file, "    3 [label=\"B\"];\n");
    fprintf(file, "    4 [label=\"c\"];\n");
    fprintf(file, "    5 [label=\"b\"];\n");
    if(bitInstructions==31) fprintf(file, "    6 [label=\"c\"];\n");
    int j;
    j=0;
    while(bitInstructions>0){
        if(bitInstructions&1) fprintf(file, "%s",instructions[j]);
        bitInstructions/=2;
        j++;
    }
    // title
    fprintf(file,"    labelloc=\"t\";\n");
    fprintf(file,"    label=\"");
    for(int i=0;i<wordLen;i++){fprintf(file,"%c", word[i]);}
    fprintf(file,"\";\n");
    fprintf(file, "}\n");
    fclose(file);
}

void match(char x, char * word, struct ways * caminos, int * wordLen){
    int newLen=0;
    for(int j = 0 ;j<caminos->len ; j++){
        if(caminos->indxs[j] < (*wordLen)){
            if(word[caminos->indxs[j]] == x){
                caminos->indxs[newLen] = caminos->indxs[j] + 1;
                caminos->instructions[newLen] = caminos->instructions[j];
                newLen++;
            }
        }
    }
    caminos->len = newLen;
}

void S(char * word, struct ways*  caminos, int * len){
    match('a', word, caminos, len);
    if(caminos->len == 0) return;
    addInstruction(caminos, 0);
    B( word, caminos, len);
    if(caminos->len == 0) return;
    addInstruction(caminos, 1);
    match('c', word, caminos, len);
    addInstruction(caminos, 2);
}

void B(char * word, struct ways * caminos, int * len){
    struct ways c1;
    copyWays(caminos , &c1);
    match('b', word, caminos, len);
    addInstruction(caminos, 3);
    match('c', word, caminos, len);
    addInstruction(caminos, 4);
    match('b', word, &c1, len); 
    addInstruction(&c1, 3);
    unionElem(caminos, &c1);
}

void copyWays(struct ways * src, struct ways * dest){
    dest->len = src->len;
    for(int j =0 ;j< src->len;j++){
        dest->indxs[j] = src->indxs[j];
        dest->instructions[j] = src->instructions[j];
    }
}

void addInstruction( struct ways * caminos, int i){
    for(int j =0 ;j< caminos->len;j++) {
        caminos->instructions[j] = (caminos->instructions[j] ) | (1<<i);
    }
}

void unionElem(struct ways * caminos,struct ways * extra){
    char isIn, i;
    i=0;
    while(i<extra->len){
        isIn=0;
        for(int j=0;j<caminos->len;j++){
            if(caminos->indxs[j] == extra->indxs[i]){
                isIn = 1;
                break;
            }
        }
        if(!isIn){
            caminos->indxs[caminos->len] = extra->indxs[i];
            caminos->instructions[caminos->len] = extra->instructions[i];
            caminos->len+=1; 
        }
        i++;
    }
}

int main()
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    size_t read;
    int cont =0;
    char result;
    char buff[50];
    fp = fopen("tokens.txt", "r");
    if (fp == NULL) exit(EXIT_FAILURE);
    while ((read = getline(&line, &len, fp)) != -1) {
        if(line[read-1] == '\n' || line[read-1] == '\0' ){
            result = parser(line,   read-1,  cont);
        }else{
            result = parser(line,   read,  cont);
        }
        snprintf(buff, 50, "Accepted, generated \"derivation_tree%d.dot\"", cont);
        printf("%s: %s\n", line, result?buff:"Not accepted");
        cont++;
    }

    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
    return 0;
}

/*
int main(){
        printf( "abbbbc %d\n", parser("abbbbc", 6));
    printf( "abbbb %d\n", parser("abbbb", 5));
       printf( "abcbcbc %d\n", parser("abcbcbc",7));
    printf( "ac %d\n", parser("ac",2));
    printf( "abc %d\n", parser("abc", 3));
    printf( "abcc %d\n", parser("abcc", 4));
 
    return 0;
}
*/