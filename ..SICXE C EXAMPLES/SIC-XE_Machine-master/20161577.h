/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                               *
 *                    Sogang University                          *
 *         Department of Computer Science and Engineering        *
 *                                                               *
 * Subject name: System Programming                              *
 * Project title: [3] SIC/XE Machine - Linking Loader            *
 *                                                               *
 * Author: Inho Kim                                              *
 * Student ID: 20161577                                          *
 *                                                               *
 * File name: 20161577.h                                         *
 * File description: Main header file for the project.           *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CMD_LEN 257         // maximum length of input string
#define CMD_CNT 18          // list of command formats
#define ARG_MAX 3           // maximum argument count
#define MEM_SIZE 1048576    // 2^20 = 1MB
#define HASH_SIZE 20        // hash table size
#define CS_LEN 7            // maximum control section name length
#define CS_MAX 3            // maximum control section count
#define ASM_LEN 121         // maximum length of single asm source line
#define INC_BYTE 16 * 16    // increase byte in object code
#define INC_HBYTE 16        // increase half byte in object code
#define MAX12DISP 4096      // highest number expressable with 12 bits
#define MAX15ADDR 32768     // highest number expressable with 15 bits
#define REG_CNT 9           // register count
#define Areg 0              // A register number
#define Xreg 1              // X register number
#define Lreg 2              // L register number
#define Breg 3              // B register number
#define Sreg 4              // S register number
#define Treg 5              // T register number
#define Freg 6              // F register number
#define PCreg 7             // PC register number
#define SWreg 8             // SW register number

typedef struct dirent ENTRY;        // for dir command
typedef struct stat STBUF;          // for dir command
typedef enum { false, true } bool;  // workaround for bool type in C

// error codes, for better readability
typedef enum { 
    SAFE, FORMAT, HEX, VALUE, FILENAME
} ERROR_CODE;

// command types, for better readability
typedef enum {
    shell, memory, opcode, assembler, linkLoader, invalid
} CMD_TYPE;

// command functions, for better readablitiy
typedef enum {
    help, dir, quit, hist, dump, edit, fill, reset, op, oplist, type, assemble, symbol, pAddr, loader, run, bp, invFormat, invHex, invVal, invFile
} CMD_FUNC;

// assembly source error type, for better readability
typedef enum {
    OK, SYMBOL, INSTRUCTION, OPERAND
} ASM_ERROR;

// directives list, for better readability
typedef enum {
    NOTDR, START, END, BASE, BYTE, WORD, RESB, RESW
} DIREC_NAME;

// register list, for better readability
typedef enum {
    A, X, L, B, S, T, F, PC, SW
} REG;

// command format structure
typedef struct {
    char str[CMD_LEN];
    char abb[CMD_LEN];
    CMD_TYPE category;
    CMD_FUNC func;
    bool arg;
} COMMAND;

// user input command parsed structure
typedef struct {
    CMD_FUNC cmd;
    short argCnt;
    char arg[ARG_MAX][10];
} INPUT_CMD;

// structure for generic linked list
typedef struct NODE {
    void* data;
    struct NODE* next;
} NODE;

typedef NODE* LIST;

// history node structure
typedef struct HIST_STRUCT {
    char str[CMD_LEN];
} HIST_NODE;

// hash table bucket structure
typedef struct HASH_STRUCT {
    int codeVal;
    char code[3];
    char inst[CMD_LEN];
    enum { f1, f2, f34 } format;
    int operandCnt;
    struct HASH_STRUCT* next;
} HASH_ENTRY;

// assembly source parse structure
typedef struct ASM_STRUCT {
    char source[ASM_LEN];       // source code
    char label[ASM_LEN];        // label field
    char inst[ASM_LEN];         // instruction field
    char operand[2][ASM_LEN];   // operand field
    bool hasLabel;              // flag for having label
    bool indexing;              // flag for having indexing mode
    int lineNum;                // line number in list file
    int location;               // location counter of instruction
    int operandCnt;             // operand count
    int byteSize;               // size of instruction in bytes
    unsigned objCode;           // value of object code
    enum { ERROR, INST, PSEUDO, COMMENT } type;                 // type of source line
    enum { NONE, format1, format2, format3, format4 } format;   // type of instruction
    DIREC_NAME direcName;       // directive type
    ASM_ERROR errorCode;        // error type
    struct ASM_STRUCT* next;    // pointer to next assembly source
} ASM_SRC;
    
// symbol table entry structure
typedef struct SYMBOL_STRUCT {
    char symbol[ASM_LEN];
    int address;
    struct SYMBOL_STRUCT* next;
} SYMBOL_ENTRY;

// Object Code list structure
typedef struct OBJ_STRUCT {
    unsigned objCode;
    int location;
    int byteSize;
    struct OBJ_STRUCT* next;
} OBJ_CODE;

// Modification Record list structure
typedef struct MOD_STRUCT {
    int location;
    int lenHB;
    struct MOD_STRUCT* next;
} MOD_RECORD;

// Control Section structure for ESTAB
typedef struct CS_STRUCT {
    char csName[CS_LEN];
    int stAddress;
    int length;
    LIST extSym;
} CNT_SEC;

// External Symbol structure for ESTAB
typedef struct ES_STRUCT {
    char symName[CS_LEN];
    int address;
} EXT_SYMBOL;

// Break Point list structure
typedef struct BP_STRUCT {
    int address;
} BREAK_PNT;

int progAddr;                   // program start address set by progaddr command
int execAddress, endAddress;    // execution and end address
int registers[REG_CNT]; // A, X, L, B, S, T, F, PC, SW

int hexToDec(char*);    // function to check for vaild hex value and return converted decimal value
void symTableFree();    // function to free SYMTAB
void parseListFree();   // function to free ASM parse list
void objListFree();     // function to free OBJ code list
void modListFree();     // function to free Modification Record list

HASH_ENTRY* bucketSearch(char*); // function to search bucket
