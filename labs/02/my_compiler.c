//Exercise 1-24. Write a program to check a C program for rudimentary syntax errors like
//unmatched parentheses, brackets and braces.  Don't forget about quotes, both single and double,
//escape sequences, and comments. (This program is hard if you do it in full generality.)

#include <stdio.h>
#include <stdlib.h>

#define STATE_NO_ERRORS 0
#define STATE_ONE_SLASH 1
#define STATE_COMMENT 2
#define STATE_MULTI_LINE_COMMENT 22
#define STATE_SCAPED_MULTI_LINE_COMMENT 23
#define STATE_CHAR 3
#define STATE_CHAR_SCAPED 31
#define STATE_CHAR_FILLED 32
#define ERR_CHAR_OVERFILLED 33
#define ERR_CHAR 34
#define STATE_STRING_SCAPE_CHAR 41
#define STATE_STRING 4
#define ERR_IN_STRING 44
#define ERR_STK_FULL 5
#define ERR_SCAPED_CHAR 6
#define ERR_UNBALANCED_CURRENT 7
#define ERR_UNBALANCED_LAST_CURRENT 8

#define ERR_OPEN_COMMENT 92
#define ERR_OPEN_BRACE  95
#define ERR_OPEN_CHAR 93
#define ERR_OPEN_STRING 94

//states:
//0 no errors
//1 one slash
//2 comment
//22 multi line comment.
//3 simple quote
//31 simple quote, \ scappe char
//32 simple quote filled
//33 more than 1 char in simple quote
//34 simplequoteerror
//4 double qoute
//41 double quoute, \ scape char
//44 double quote error
//5 stack full.
//6 scaped char misplaced. err


// ERR 92,93,94,95, after reading if some multiComment, string, char or string is still open.

int size = 100;

struct symbol{
    char chr;
    int line;
    int position;
};

int throwError(int error, struct symbol stack[] , int *stkLen, int line, int position ,char c);
int machineState(struct symbol stack[] , int *stkPos, int *state, int line, int position ,char c);
int readFile(char* file);
int push(struct symbol stack[] , int *stkPos, int line, int position ,char c);
struct symbol top(struct symbol stack[] , int *stkPos );
struct symbol pop(struct symbol stack[] , int *stkPos );
int readFile(char* file);
int main(int argc, char** argv);
int printStack(struct symbol stack[] , int *stkPos);
struct symbol createStruct(char c, int line, int position);



int main(int argc, char** argv){
    //char* file = "mod6.c";
    char* file = argv[1];
    printf("Checking balance of parenthesis, braces and curl braces in the file: %s\n", file);
    readFile(file);
}


int push(struct symbol stack[] , int *stkPos, int line, int position ,char c){
    if(*stkPos==size){
        struct symbol *newStack = (struct symbol*)malloc(2*size*sizeof(struct symbol)); 
        for(int i=0;i<size;i++){
            newStack[i] = stack[i];
        }
        free(stack);
        stack = newStack;
        size<<=1;
    }

    struct symbol s=  createStruct( c,  line,  position);

    stack[*stkPos] = s;
    *stkPos = *stkPos + 1;
    return 0;
}

int printStack(struct symbol stack[] , int *stkPos){
    printf("Stack: ");
    for(int i=0;i < *stkPos;i++){
        printf("%c, ", stack[i].chr);
    }printf("\n");
}

struct symbol top(struct symbol stack[] , int *stkPos ){
    struct symbol toTop;
    toTop = stack[*stkPos-1];
    return toTop;
}

struct symbol pop(struct symbol stack[] , int *stkPos ){
    struct symbol toPop;
    toPop=top(  stack ,  stkPos);
    *stkPos =*stkPos-1;
    return toPop;
}

int readFile(char* file){
    FILE *filePointer;
    char ch;
    int showedError,error, initialState, initialPosition;
    int* state,* stackPos,line, counterChar;

    struct symbol* stack = (struct symbol*)malloc(size*sizeof(struct symbol));
    line = 1;
    showedError =0;
    counterChar =0;
    initialState = 0;
    initialPosition=0;

    stackPos = &initialPosition;
    state = &initialState;

    filePointer = fopen(file, "r");
    if (filePointer == NULL) { printf("File is not available \n");}
    else
    {
        while ((ch = fgetc(filePointer)) != EOF)
        {      
            counterChar++;
            //printf("%c %d %d s%d \n", ch, line, counterChar, *state);
            error = machineState(stack, stackPos,state,line, counterChar, ch);  
            if(error) {
                throwError(error, stack , stackPos,   line,  counterChar , ch);
                showedError = 1;
                break;
            }
            if(ch=='\n') {
                counterChar =0;
                line++;
            }
        }
        if(!showedError){
            if(*stackPos == 0){ 
                if(*state){
                     //TODO QUE PASA SI NO SE CIERRAN COMMENTS, PARENTESIS, ETC.
                    switch (*state)
                    {
                    case STATE_STRING:
                    case STATE_STRING_SCAPE_CHAR:
                          throwError(ERR_OPEN_STRING,stack,stackPos,line,counterChar,'\0' ); 
                         break;
                    case STATE_CHAR:
                    case STATE_CHAR_SCAPED:
                    case STATE_CHAR_FILLED:
                         throwError(ERR_OPEN_CHAR,stack,stackPos,line,counterChar,'\0' ); 
                         break;
                    case STATE_SCAPED_MULTI_LINE_COMMENT:
                    case STATE_MULTI_LINE_COMMENT:
                         throwError(ERR_OPEN_COMMENT,stack,stackPos,line,counterChar,'\0' ); 
                         break;
                    case STATE_ONE_SLASH:
                         throwError(STATE_ONE_SLASH,stack,stackPos,line,counterChar,'/' ); 
                         break;
                    default:
                         break;
                    }
                }else{
                    printf("The document %s has not errors.\n", file);
                }
            }else{ 
                // stack is not empty: unbalanced (){}[]
                throwError(ERR_OPEN_BRACE,stack,stackPos,line,counterChar,'\0' );     
            }
        }
    }

    fclose(filePointer);

    return 0;

}

int throwError(int error, struct symbol stack[] , int *stkLen, int line, int position ,char c){
                switch (error) 
                {
                case STATE_ONE_SLASH: 
                    printf("Slash / missplaced at  line %d and position %d\n", line, position);
                    break;
                case ERR_CHAR_OVERFILLED:
                    printf("\'\' can only have one character. Error at line %d and position %d\n", line, position);
                    break;
                case ERR_CHAR:
                    printf("Error in character at line %d and position %d\n", line, position);
                    break;
                case ERR_IN_STRING:
                    if(top(stack, stkLen).chr =='\"'){
                        struct  symbol s = pop(stack, stkLen);
                        printf("Error in string at line %d and position %d.\nThe string started at line %d and position %d is not properly closed.\n",line,
                        position,s.line,s.position);
                    }else{
                    printf("Error in string at line %d and position %d\n", line, position);
                    }
                    break;
                case ERR_STK_FULL:
                    printf("Stack is full. This error is deprecated. At line %d and position %d\n", line, position);
                    break;
                case ERR_SCAPED_CHAR:
                    printf("Error in character, character %c can\'t be scaped. Error at line %d and position %d\n",c, line, position);
                    break;
                case ERR_UNBALANCED_CURRENT:
                    printf("Unbalanced %c at line %d, charater %d\n",c,line,position );
                    break;
                case ERR_UNBALANCED_LAST_CURRENT:
                    if(*stkLen >0){
                        struct symbol s = pop(stack, stkLen);
                        printf("Symbol %c from line %d and position %d, doesn't match with symbol %c from line %d and position %d\n", 
                                    s.chr, s.line, s.position,
                                    c,     line,   position );
                    }else{printf("Unbalanced %c at line %d, charater %d\n",c,line,position );}
                    break;
                case ERR_OPEN_COMMENT:
                    printf("Multi line comment is open at the end of the document. At line %d and position %d\n", line, position);
                    break;
                case ERR_OPEN_CHAR:
                    printf("Character is open at the end of the document. At line %d and position %d\n", line, position);
                    break;
                case ERR_OPEN_STRING:
                    printf("String is open at the end of the document. At line %d and position %d\n", line, position);
                    break;
                case ERR_OPEN_BRACE:
                    if(1){ // This evades the error : error: a label can only be part of a statement and a declaration is not a statement ??? WHY?
                    struct symbol s= pop(stack, stkLen);
                    printf("%c  from line %d and position %d, is open at the end of the document. At line %d and position %d\n", s.chr,s.line,s.position, line, position);
                    }
                    break;
                
                default:
                    break;
                }
}

int machineState(struct symbol stack[] , int *stkPos, int *state, int line, int position ,char c){
    //Setted by hand all states and transitions.
    if(c=='{'  || c=='[' ||c=='(' ){
        //printf("%c %d %d s%d \n", c, line, position, *state);
        if(*state==STATE_NO_ERRORS){
            struct symbol s= createStruct(c, line, position);
            int err = push( stack , stkPos,  line,  position , c);
            if(err){ return ERR_STK_FULL;} 
        }else{
            if(*state==STATE_ONE_SLASH) return STATE_ONE_SLASH;
            if(*state==STATE_CHAR_FILLED) return ERR_CHAR_OVERFILLED;
            if(*state==STATE_CHAR ) *state=STATE_CHAR_FILLED;
            if(*state==STATE_CHAR_SCAPED) return ERR_SCAPED_CHAR;
            if(*state==STATE_SCAPED_MULTI_LINE_COMMENT) *state = STATE_MULTI_LINE_COMMENT;
        }
    }
    else if(c==']'|| c=='}' || c==')'){
        if(!*state){
            if(*stkPos<=0) return ERR_UNBALANCED_CURRENT;
            struct symbol head = top(  stack, stkPos); 
            if( head.chr == '[' && c==']'||
                head.chr == '{' && c=='}'||
                head.chr == '(' && c==')' ){
                pop(stack ,  stkPos);//balanced, no error.
            }else{  return ERR_UNBALANCED_LAST_CURRENT; }
        }else{
            switch (*state)
            {
            case STATE_ONE_SLASH:
                return STATE_ONE_SLASH;
            case STATE_CHAR_SCAPED:
                return ERR_SCAPED_CHAR;
            case STATE_CHAR_FILLED:
                return ERR_CHAR_OVERFILLED;
            case STATE_CHAR:
                *state=STATE_CHAR_FILLED;
                break;
            case STATE_SCAPED_MULTI_LINE_COMMENT:
                *state = STATE_MULTI_LINE_COMMENT;
            break;    
            default:
                break;
            }
        }
        
    }else if(c=='\\'){
        switch (*state)
        {
        case STATE_CHAR_SCAPED:
            *state=STATE_CHAR_FILLED;
            break;
        case STATE_CHAR_FILLED:
            return ERR_CHAR_OVERFILLED;
        case ERR_SCAPED_CHAR:
            *state=STATE_NO_ERRORS;
            break;
        case STATE_CHAR:
            *state=STATE_CHAR_SCAPED;
            break;
        case STATE_NO_ERRORS:
            *state=ERR_SCAPED_CHAR;
            break;
        case STATE_STRING_SCAPE_CHAR:
            *state=STATE_STRING;
            break;
        case STATE_STRING:
            *state=STATE_STRING_SCAPE_CHAR;
            break;
        case STATE_SCAPED_MULTI_LINE_COMMENT:
            *state = STATE_MULTI_LINE_COMMENT;
            break;        
        
        default:
            break;
        }
    }else if(c=='/'){
        switch (*state)
        {
        case STATE_NO_ERRORS:
            *state = STATE_ONE_SLASH;
            break;
        case STATE_SCAPED_MULTI_LINE_COMMENT:
            *state = STATE_NO_ERRORS;
            break;
        case STATE_ONE_SLASH:
            *state = STATE_COMMENT;
            break;
        case STATE_CHAR:
            *state= STATE_CHAR_FILLED;
            break;
        case STATE_CHAR_SCAPED:
            return ERR_SCAPED_CHAR;
        case STATE_CHAR_FILLED:
            return ERR_CHAR_OVERFILLED; 
        default:
            break;
        }
    }else if(c=='\''){
        switch (*state)
        {
        case STATE_NO_ERRORS:
            *state=STATE_CHAR;
            break;
        case STATE_SCAPED_MULTI_LINE_COMMENT:
            *state = STATE_MULTI_LINE_COMMENT;
            break;    
        case STATE_ONE_SLASH:
                return STATE_ONE_SLASH;
            break;
        case STATE_CHAR:
            return ERR_CHAR;
        case STATE_CHAR_SCAPED:
            *state=STATE_CHAR_FILLED;
            break;
        case STATE_CHAR_FILLED:
            *state=STATE_NO_ERRORS;
            break;
        case STATE_STRING_SCAPE_CHAR:
            *state = STATE_STRING; 
            break;      
        default:
            break;
        }  
    }else if(c == '\"' ){
        switch (*state)
        {
        case STATE_NO_ERRORS:
            push(stack, stkPos,line,position,c);
            *state=4;
            break;
        case STATE_CHAR_FILLED:
            return ERR_CHAR_OVERFILLED;
        case STATE_CHAR_SCAPED:
            *state=STATE_CHAR_FILLED;
            break;
        case STATE_STRING:
            if(top(stack, stkPos).chr == '\"'){
                pop(stack,stkPos);
            }
            *state=STATE_NO_ERRORS;
            break;
        case STATE_CHAR:
            return ERR_CHAR;
        case STATE_ONE_SLASH:
            return STATE_ONE_SLASH;
        case STATE_STRING_SCAPE_CHAR:
            *state = STATE_STRING;
            break;
        case STATE_SCAPED_MULTI_LINE_COMMENT:
            *state = STATE_MULTI_LINE_COMMENT;
            break;   
        default:
            break;
        }
    }else if(c=='\n'){ 
        if(*state==STATE_ONE_SLASH)  return STATE_ONE_SLASH;
        if(*state==STATE_CHAR ||*state==STATE_CHAR_SCAPED ||*state==STATE_CHAR_FILLED ) return ERR_CHAR; 
        if(*state== STATE_STRING) return ERR_IN_STRING;
        if(*state==STATE_STRING_SCAPE_CHAR ) *state = STATE_STRING;
        if(*state==STATE_COMMENT ) *state=STATE_NO_ERRORS;
        if(*state == STATE_SCAPED_MULTI_LINE_COMMENT) *state = STATE_MULTI_LINE_COMMENT;
    }else if(c=='='){
        if(*state == STATE_ONE_SLASH) *state=STATE_NO_ERRORS;
        if(*state == STATE_SCAPED_MULTI_LINE_COMMENT) *state = STATE_MULTI_LINE_COMMENT;
    }else if(c=='*'){
        if(*state == STATE_ONE_SLASH)  *state = STATE_MULTI_LINE_COMMENT;
        if(*state == STATE_MULTI_LINE_COMMENT) *state = STATE_SCAPED_MULTI_LINE_COMMENT;
    }else{
        switch (*state)
        {
        case STATE_ONE_SLASH: 
            *state = STATE_NO_ERRORS;
            break;
        case STATE_CHAR:
        case STATE_CHAR_SCAPED:
            *state=STATE_CHAR_FILLED;
            break;
        case STATE_STRING_SCAPE_CHAR:
            *state =STATE_STRING;
            break;
        case STATE_CHAR_FILLED:
            return ERR_CHAR_OVERFILLED;
        case STATE_SCAPED_MULTI_LINE_COMMENT:
            *state = STATE_MULTI_LINE_COMMENT;
            break;
        default:
            break;
        }
    }
    return 0;
}

struct symbol createStruct(char c, int line, int position){
    struct symbol s;
    s.chr = c;
    s.line = line;
    s.position = position;
    return s;
}