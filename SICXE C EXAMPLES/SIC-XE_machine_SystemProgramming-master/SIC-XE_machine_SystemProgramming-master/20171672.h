#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <math.h>

#define MAX_INPUT_SIZE 500
#define MEGA_BYTE 1024*1024
#define MEGA_BYTE_ROW 256*256
#define MAX_ARR_SIZE 100
#define HASHSIZE 20


typedef struct NODE{

  char command[100];
  struct NODE* link;
}NODE;


typedef struct OPTAB{
    int id,count;
    char code[10];
    char name[10];
    char format[10];
    struct OPTAB* next;
}OPTAB;

typedef struct TEMP_OP{

    char code[10];
    char name[10];
    char format[10];

}TEMP_OP;

typedef struct SYMTAB{
    char label[32];
    int address;
}SYMTAB;

typedef struct HASH_SYMTAB{
    int id;
    char label[32];
    int address;
    struct HASH_SYMTAB* next;
}HASH_SYMTAB;

typedef struct ESTAB{
    char csect[10];
    char symbol[10];
    unsigned int address;
    unsigned int length;
    char refer[10];
}ESTAB;

typedef struct runtable{
    int code;
    int format;
}runtable;

static runtable RUNTAB[]= {
      {0x00,3},
      {0x68,3},
      {0x74,3},
      {0x50, 3},
      {0x0C, 3},
      {0x10, 3},
      {0x14, 3},
      {0x54, 3},
      {0x3C, 3},
      {0x48, 3},
      {0x38, 3},
      {0x30, 3},
      {0x4C, 3},
      {0x28, 3},
      {0xA0, 2},
      {0xB4, 2},
      {0xB8, 2},
      {0xE0, 3},
      {0xD8, 3},
      {0xDC, 3}
};


typedef struct Intermediate{
    unsigned short int line;
    unsigned short int location;
    unsigned  int OBJ;
    char label[32];
    char mnmn[32];
    char operand[32];
}Intermediate;

typedef struct Relocation{
    int address;
    int length;
}Relocation;

typedef struct Register{
    char regname[32];
    int number;
}Register;

static Register REGTAB[]={
      { "A", 0 },
      { "X", 1 },
      { "L", 2 },
      { "B", 3 },
      { "S", 4 },
      { "T", 5 },
      { "F", 6 },
      { "PC", 8 },
      { "SW", 9 }
};

struct OPTAB* hashtable[HASHSIZE];
struct HASH_SYMTAB* s_hashtable[HASHSIZE];
struct SYMTAB symboltb[HASHSIZE][HASHSIZE];
struct TEMP_OP optab;

struct ESTAB estab[HASHSIZE];

int BUFF_length;
int REGidx;
int INDX;
int SYMidx;
int LOCCTR[100];
int LOCCTR_counter = 0;
int SYMTAB_counter[10] = { 0, };
int start_address[10] = { 0, };
int program_length[10] = { 0, };
int list_index[10] = { 0, };
int relocation_counter[10] = { 0, };
int prefix;
int IsAssembled = 0;

char BUFF[200];
char LABEL[32];
char MNMN[32];
char OPND[32];
char end_operand[32];
char fileobj[32];
char filelst[32];

unsigned int PROGADDR = 0;
unsigned int CSADDR = 0;
unsigned int CSLTH = 0;
unsigned int EXECADDR = 0;
char ORG_LOAD[200] = { 0, };
const char *ARR_LOAD[5];
unsigned int BRKPN[MAX_INPUT_SIZE] = { 0, };
int bp_count = 0;
int current_bp = 0;

static int REG[11] = { 0, };

Intermediate* list[10][100];
Relocation Rlist[10][20];
