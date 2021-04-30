#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define BSIZE 128   /* buffer size */
#define NONE -1
#define EOS '\0'
#define NUM 256
#define DIV 257
#define MOD 258
#define ID 259
#define DONE 260
#define BEGIN 261
#define END 262
#define PRG 263
#define INP 264
#define OUT 265
#define TIM 266
#define MIN 267
#define PLS 268
#define EXP 269
#define VAR 270
#define ITG 272
#define REL 273
#define CHR 274
#define BOL 275
#define STRMAX 999
#define SYMMAX 100
#define variablesMax 20

FILE *input;
FILE *output;

int tokenval = NONE;   /* value of token attribute */
int lineno = 1;
char lexbuf[BSIZE];
int lookahead;
char lexemes[STRMAX];
int lastchar = -1;
int lastentry = 0;
int variables[variablesMax];
int currentVariableCount = 0;


struct entry {
    char *lexptr;
    int token;
};

struct entry symtable[SYMMAX];

int lexan();
void parse();
void header();
void declarations();
void variableDeclarations();
void variableDeclarationsPrime();
void variableDeclaration();
void identifierList();
void identifierListPrime();
void type();
void expr();
void term();
void factor();
void match(int);
void emit(int, int);
int lookup(char[]);
void init();
void error(char*);
int insert(char[], int);

int lexan()     /* lexical analyzer */
{
    int t;
    while (1) {
        t = getc(input);
        //printf("%c", t);
        if (t == ' ' || t == '\t')
            ;
        else if (t == '\n')
            lineno = lineno + 1;
        else if (isdigit(t)) {
            ungetc(t, input);
            fscanf(input, "%d", &tokenval);
            return NUM;
        }
        else if (isalpha(t)) {
            int p, b = 0;
            while (isalnum(t)) {
                lexbuf[b] = t;
                t = getc(input);
                b++;
                if (b >= BSIZE)
                    error("Compiler Error");
            }
            lexbuf[b] = EOS;
            if (t != EOF)
                ungetc(t, input);
            p = lookup(lexbuf);
            if (p == 0)
                p = insert(lexbuf, ID);
            tokenval = p;
            return symtable[p].token;
        }
        else if (t == EOF)
            return DONE;
        else {
            tokenval = NONE;
            return t;
        }
    }
}


void parse()
{
    lookahead = lexan();
    header();
    declarations();
    fprintf(output, "begin\n");
    match(BEGIN);
    while (lookahead != END) {  
        expr(); match(';'); fprintf(output, ";\n");
    }
    match(END); match('.');
    fprintf(output, "end.");
    match(DONE);
}

void header()
{
    match(PRG); match(EXP); match('('); match(INP); match(','); match(OUT); match(')'); match(';');
    fprintf(output, "#include <iostream>\nusing namespace std;\n");
}

void declarations()
{
    switch (lookahead)
    {
    case VAR:
        match(VAR);
        variableDeclarations();
        fprintf(output, "\n");
    default:
        return;
    }
}

void variableDeclarations()
{
    variableDeclaration();
    variableDeclarationsPrime();
}

void variableDeclaration()
{
    currentVariableCount = 0;
    identifierList();
    match(':');
    type();
    match(';');
    for (int i = 0; i < currentVariableCount; i++)
    {
        emit(ID, variables[i]);
        if (i == currentVariableCount-1)
            continue;
        fprintf(output, ", ");
    }
    fprintf(output, "; ");
}

void identifierList()
{
    variables[currentVariableCount++] = tokenval;
    match(ID);
    identifierListPrime();
}

void identifierListPrime()
{
    switch (lookahead)
    {
    case ',':
        match(',');
        //emit(ID, tokenval);
        variables[currentVariableCount++] = tokenval;
        match(ID);
        identifierListPrime();
        break;
    default:
        return;
    }
}

void variableDeclarationsPrime()
{
    switch (lookahead)
    {
    case ID:
        variableDeclaration();
        variableDeclarationsPrime();
        break;
    default:
        return;
    }   
}

void type()
{
    switch (lookahead)
    {
    case ITG:
        fprintf(output, "int ");
        match(ITG);
        break;
    case REL:
        fprintf(output, "float ");
        match(REL);
        break;
    case CHR:
        fprintf(output, "char ");
        match(CHR);
        break;
    case BOL:
        fprintf(output, "bool ");
        match(BOL);
        break;
    default:
        error("Syntax error from type");
    }
}

void expr()
{
    int t;
    term();
    while (1)
        switch (lookahead)
        {
        case '+': case '-': case PLS: case MIN:
            t = lookahead;
            match(lookahead); term(); emit(t, NONE);
            continue;
        
        default:
            return;
        }
}


void term()
{
    int t;
    factor();
    while (1)
        switch(lookahead) {
            case '*': case '/': case DIV: case MOD: case TIM:
                t = lookahead;
                match(lookahead); factor(); emit(t, NONE);
                continue;
            default:
                return;
        }
}

void factor()
{
    switch (lookahead) {
        case '(':
            match('('); expr(); match(')'); break;
        case NUM:
            emit(NUM, tokenval); match(NUM); break;
        case ID:
            emit(ID, tokenval); match(ID); break;
        default:
            error("Syntax Error from factor");
    }
}


void match(int t)
{
    //std::cout << lookahead << " " << t << std::endl;
    printf("%d\n", t);
    if (lookahead == t)
        lookahead = lexan();
    else error("Syntax Error from match");
}


void emit(int t, int tval)
{
    switch(t) {
        case '+': case '-': case '*': case '/':
            fprintf(output, "%c ", t); break;
        case PLS:
            fprintf(output, "plus "); break;
        case MIN:
            fprintf(output, "minus "); break;
        case TIM:
            fprintf(output, "times "); break;
        case DIV:
            fprintf(output, "DIV "); break;
        case MOD:
            fprintf(output, "MOD "); break;
        case NUM:
            fprintf(output, "%d ", tval); break;
        case ID:
            fprintf(output, "%s", symtable[tval].lexptr); break;
        default:
            fprintf(output, "Token %d, tokenval %d ", t, tval);
    }
}


int lookup(char s[])
{
    int p;
    for (p = lastentry; p > 0; p--)
        if (strcmp(symtable[p].lexptr, s) == 0)
            return p;
    return 0;
}


int insert(char s[], int tok)
{
    int len;
    len = strlen(s);
    if (lastentry + 1 >= SYMMAX)
        error("Symbol Table Full");
    if (lastchar + len + 1 >= STRMAX)
        error("Lexemes Array Full");
    lastentry = lastentry + 1;
    symtable[lastentry].token = tok;
    symtable[lastentry].lexptr = &lexemes[lastchar + 1];
    lastchar = lastchar + len + 1;
    strcpy(symtable[lastentry].lexptr, s);
    return lastentry;
}


struct entry keywords[] = {
    "div", DIV,
    "mod", MOD,
    "begin", BEGIN,
    "end", END,
    "PROGRAM", PRG,
    "Example", EXP,
    "Input", INP,
    "Output", OUT,
    "times", TIM,
    "plus", PLS,
    "minus", MIN,
    "VAR", VAR,
    "integer", ITG,
    "real", REL,
    "char", CHR,
    "boolean", BOL,
    0, 0
};

void init()
{
    struct entry *p;
    for (p = keywords; p->token; p++)
        insert(p->lexptr, p->token);
}

void error(char* m)
{
    
    printf("Error: line %d: %s \n", lineno, m);
    exit(1);
}

void openInputFile(char* fileName)
{
    if (input = fopen(fileName, "r")) 
        ;
    else
    {
        error("Input file does not exist, please check your spelling.");
    }
}

void openOutputFile(char* fileName)
{
    output = fopen(fileName, "w");
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
      error("Too few arguments");
    }
    else if (argc > 3)
    {
      error("Too many arguments");
    }
    else
    {
      openInputFile(argv[1]);
      openOutputFile(argv[2]);
      init();
      parse();
      fclose(input);
      fclose(output);
      exit(0);
      return 0;
    }
}