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
 * File name: execute.h                                          *
 * File description: Header file for execution related tasks.    *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// add mnemonics as enum with corresponding opcode
typedef enum {
    ADD = 0x18,
    ADDF = 0x58,
    ADDR = 0x90,
    AND = 0x40,
    CLEAR = 0xB4,
    COMP = 0x28,
    COMPF = 0x88,
    COMPR = 0xA0,
    DIV = 0x24,
    DIVF = 0x64,
    DIVR = 0x9C,
    FIX = 0xC4,
    FLOAT = 0xC0,
    HIO = 0xF4,
    J = 0x3C,
    JEQ = 0x30,
    JGT = 0x34,
    JLT = 0x38,
    JSUB = 0x48,
    LDA = 0x00,
    LDB = 0x68,
    LDCH = 0x50,
    LDF = 0x70,
    LDL = 0x08,
    LDS = 0x6C,
    LDT = 0x74,
    LDX = 0x04,
    LPS = 0xD0,
    MUL = 0x20,
    MULF = 0x60,
    MULR = 0x98,
    NORM = 0xC8,
    OR = 0x44,
    RD = 0xD8,
    RMO = 0xAC,
    RSUB = 0x4C,
    SHIFTL = 0xA4,
    SIO = 0xF0,
    SSK = 0xEC,
    STA = 0x0C,
    STB = 0x78,
    STCH = 0x54,
    STF = 0x80,
    STI = 0xD4,
    STL = 0x14,
    STS = 0x7C,
    STSW = 0xE8,
    STT = 0x84,
    STX = 0x10,
    SUB = 0x1C,
    SUBF = 0x5C,
    SUBR = 0x94,
    SVC = 0xB0,
    TD = 0xE0,
    TIO = 0xF8,
    TIX = 0x2C,
    TIXR = 0xB8,
    WD = 0xDC
} OPmnemonic;

// instruction format
typedef enum {
    constant, fmt1, fmt2, fmt3, fmt4
} FMT;

// addressing mode
typedef enum {
    SIC, immediate, indirect, simple
} ADR_MODE;

// CC status ( <=, ==, >= )
enum {
    lt, eq, gt
} CCstatus;

// struct to store parsed obj code
typedef struct {
    OPmnemonic mnemonic;
    FMT format;
    ADR_MODE addrMode;
    union {
        int target;
        int immediate;
        REG reg[2];
    } operand;
    bool indexing;
} OBJ;

bool bpCMD(INPUT_CMD);          // search for break point
bool searchBP(int);             // search for break point

void runCMD();                  // run command
void dumpReg();                 // dump registers
int getTargetAddress(int, FMT); // return target address
int SICAddress(int);            // SIC instruction
int immediateAddress(int, FMT); // immediate address
int indirectAddress(int, FMT);  // indirect address
int simpleAddress(int, FMT);    // simple address

int getMem(int, int);           // get value from memory
void putMem(int, int, int);     // store value in memory
