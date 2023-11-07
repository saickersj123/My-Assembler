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
 * File name: assembler.c                                        *
 * File description: Tasks for SIC/XE Assembler.                 *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "20161577.h"
#include "assembler.h"

SYMBOL_ENTRY* symTable = NULL;  // symbol table
ASM_SRC* parseList = NULL;      // ASM source parse list
OBJ_CODE* objList = NULL;       // object code listt
MOD_RECORD* modList = NULL;     // modification record list

// ASM directives list
char directives[8][6] = {
    "NOTDR",
    "START",
    "END",
    "BASE",
    "BYTE",
    "WORD",
    "RESB",
    "RESW"
};
// ASM register list
char regs[7] = {
    'A', 'X', 'L', 'B', 'S', 'T', 'F'
};

// COMMAND: assemble
bool assembleCMD(INPUT_CMD ipcmd) {
    FILE *srcFile, *lstFile, *objFile;
    char lstName[CMD_LEN] = {'\0'}, objName[CMD_LEN] = {'\0'};
    int maxSrcLen = 0, progLen = 0;

    // initialize
    symTableFree();
    parseListFree();
    objListFree();
    modListFree();

    lstFile = objFile = NULL;
    srcFile = fopen(ipcmd.arg[0], "r"); // open source file
    if(!srcFile) {
        puts("ERROR: File not found.");
        return false;
    }

    // create file name for .lst and .obj files
    strncpy(lstName, ipcmd.arg[0], strlen(ipcmd.arg[0]) - 3);
    strncpy(objName, ipcmd.arg[0], strlen(ipcmd.arg[0]) - 3);
    strcat(lstName, "lst");
    strcat(objName, "obj");

    lstFile = fopen(lstName, "w");
    objFile = fopen(objName, "w");

    if(!lstFile || !objFile) {
        puts("ERROR: Problem while creating .list and .obj files.");
        fclose(srcFile);
        if(lstFile) fclose(lstFile);
        if(objFile) fclose(objFile);
        return false;
    }

    // both Pass1 and Pass2 have to be completed
    if(!assemblerPass1(srcFile, &maxSrcLen, &progLen) || !assemblerPass2(lstFile, objFile, maxSrcLen, progLen)) {
        puts(".lst file and .obj file were not created.");
        remove(lstName);
        remove(objName);
        fclose(srcFile);
        if(lstFile) fclose(lstFile);
        if(objFile) fclose(objFile);
        return false;
    }
    else
        printf("\toutput file : [%s], [%s]\n", lstName, objName);

    if(fclose(srcFile))
        puts("WARNING: Error closing file.");
    if(fclose(lstFile))
        puts("WARNING: Error closing file.");
    if(fclose(objFile))
        puts("WARNING: Error closing file.");
    return true;
}

// Pass1 of Assembler
bool assemblerPass1(FILE* srcFile, int* maxSrcLen, int* progLen) {
    ASM_SRC *curParse = NULL, *prevParse = NULL;
    char source[ASM_LEN] = {'\0'};
    int location = -1;
    int lineNum = 5;
    bool errorFlag = false;
    int mult;

    // process ASM source file line by line
    while(fgets(source, ASM_LEN, srcFile) != NULL) {
        // remove '\n' at the end of string
        if(source[strlen(source) - 1] == '\n')
            source[strlen(source) - 1] = '\0';

        curParse = parseASM(source); // get parsed line
        // START directive found
        if(curParse->direcName == START) {
            // if START directive appears for the second time
            if(location != -1) {
                puts("ERROR: START directive appeared twice.");
                errorFlag = true;
                break;
            }
            // initialize LOCCTR
            location = hexToDec(curParse->operand[0]);
            curParse->location = location;
            parseList = curParse;
            prevParse = NULL;
        }
        // no START directive, set start address as 0
        if(location == -1) {
            location = 0;
            parseList = curParse;
            prevParse = NULL;
            curParse->location = location;
        }
        else if(prevParse)
            prevParse->next = curParse; // link node to list

        curParse->lineNum = lineNum;
        curParse->location = location;

        // if current line is NOT a comment
        if(curParse->type != COMMENT) {
            // remember longest ASM source line
            if(*maxSrcLen < strlen(source))
                *maxSrcLen = strlen(source);

            // if current line has symbol in label field
            if(curParse->hasLabel && curParse->direcName != START) {
                // if symbol is already in SYMTAB
                if(symTableSearch(curParse->label)) {
                    setError(curParse, SYMBOL);
                    errorFlag = true;
                    break;
                }
                // new symbol found
                else
                    symTableAdd(curParse->label, location);
            }
            switch(curParse->type) {
                case ERROR: // Error
                    errorFlag = true;
                    break;
                case INST: // Instruction
                    curParse->byteSize = curParse->format;
                    break;
                case PSEUDO: // Pseudo-instruction
                    // directives
                    switch(curParse->direcName) {
                        case START:
                            break;
                        case END:
                        case BASE:
                            break;
                        case BYTE:
                            // calculate byte size according to operand
                            switch(curParse->operand[0][0]) {
                                case 'X':
                                    curParse->byteSize = (strlen(curParse->operand[0]) - 3 + 1) / 2;
                                    break;
                                case 'C':
                                    curParse->byteSize = strlen(curParse->operand[0]) - 3;
                                    break;
                                default:
                                    mult = INC_BYTE;
                                    for(curParse->byteSize = 1;
                                            mult <= atoi(curParse->operand[0]);
                                            mult *= INC_BYTE)
                                        curParse->byteSize++;
                                    break;
                            }
                            break;
                        case WORD:
                            curParse->byteSize = 3;
                            break;
                        case RESB:
                            curParse->byteSize = atoi(curParse->operand[0]);
                            break;
                        case RESW:
                            curParse->byteSize = atoi(curParse->operand[0]) * 3;
                            break;
                        default:
                            break;
                    }
                    break;
                case COMMENT:
                    break;
                default:
                    break;
            }
        }

        if(errorFlag)
            break;

        location += curParse->byteSize; // calculate next location counter
        lineNum += 5; // increase line number
        prevParse = curParse; // previous node

        // condition to end parsing
        if(!strcmp(curParse->inst, "END"))
            break;
    }

    *progLen = location; // save the program length for .obj file

    if(errorFlag) {
        printASMError(curParse);
        symTableFree();
        return false;
    }
    return true;
}

// Pass2 of Assembler
bool assemblerPass2(FILE* lstFile, FILE* objFile, int maxSrcLen, int progLen) {
    ASM_SRC *curParse = NULL;
    SYMBOL_ENTRY *curSymbol = NULL;
    int pcReg, baseReg;
    bool errorFlag = false;
    bool locFlag, objFlag;
    bool nFlag, iFlag, xFlag, bFlag, pFlag, eFlag;
    int i, firstExec;

    // initialize
    curParse = parseList;
    baseReg = 0;
    pcReg = parseList->location;

    while(curParse) {
        // initialize
        locFlag = objFlag = true;
        curSymbol = NULL;

        // change PC register to next instruction
        if(curParse->next)
            pcReg = curParse->next->location;

        // if no START directive at start, choose a blank program name
        if(curParse == parseList && curParse->direcName != START) {
            printf("H%6s%06X%06X\n", " ", 0, progLen);
            firstExec = curParse->location;
        }

        switch(curParse->type) {
            case INST: // instruction
                // initialize
                nFlag = iFlag = true;
                xFlag = bFlag = pFlag = eFlag = false;
                curParse->objCode = bucketSearch(curParse->inst + (curParse->inst[0] == '+' ? 1 : 0) )->codeVal;
                switch(curParse->format) {
                    case format1:
                        break;
                    case format2:
                        // determine object code of each register
                        for(i = 0; i < 2; i++) {
                            curParse->objCode *= 16;
                            switch(curParse->operand[i][0]) {
                                case 'A':
                                    curParse->objCode += 0;
                                    break;
                                case 'X':
                                    curParse->objCode += 1;
                                    break;
                                case 'L':
                                    curParse->objCode += 2;
                                    break;
                                case 'B':
                                    curParse->objCode += 3;
                                    break;
                                case 'S':
                                    curParse->objCode += 4;
                                    break;
                                case 'T':
                                    curParse->objCode += 5;
                                    break;
                                case 'F':
                                    curParse->objCode += 6;
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case format3:
                        if(!curParse->operandCnt) {
                            curParse->objCode += 3;
                            curParse->objCode *= INC_BYTE * INC_BYTE;
                            break;
                        }
                        // indexing mode
                        if(curParse->indexing)
                            xFlag = true;
                        // immediate addressing
                        if(curParse->operand[0][0] == '#')
                            nFlag = false;
                        // indirect addressing
                        else if(curParse->operand[0][0] == '@')
                            iFlag = false;

                        // if there is a symbol in operand
                        if((curSymbol = symTableSearch(curParse->operand[0] + ((!nFlag || !iFlag) ? 1 : 0)))) {
                            // if pc relative addressing is possible
                            if(curSymbol->address - pcReg + MAX12DISP / 2 >= 0) {
                                curParse->objCode += (!nFlag ? 1 : (!iFlag ? 2  : 3));
                                curParse->objCode *= INC_HBYTE;
                                curParse->objCode += (xFlag ? 10 : 2);
                                curParse->objCode *= INC_BYTE * INC_HBYTE;
                                curParse->objCode += (curSymbol->address - pcReg) & 0xFFF;
                            }
                            // if base relative addressing is possible
                            else if(curSymbol->address - baseReg < MAX12DISP) {
                                curParse->objCode += (!nFlag ? 1 : (!iFlag ? 2  : 3));
                                curParse->objCode *= INC_HBYTE;
                                curParse->objCode += (xFlag ? 12 : 4);
                                curParse->objCode *= INC_BYTE * INC_HBYTE;
                                curParse->objCode += (curSymbol->address - baseReg) & 0xFFF;
                            }
                            // approach as SIC instruction
                            else if(curSymbol->address < MAX15ADDR){
                                curParse->objCode *= INC_HBYTE;
                                curParse->objCode += (xFlag ? 8 : 0);
                                curParse->objCode *= INC_BYTE * INC_HBYTE;
                                curParse->objCode += curSymbol->address;
                            }
                            // NOT able to make reference
                            else {
                                setError(curParse, INSTRUCTION);
                                return false;
                            }
                        }
                        // if there is a value in operand
                        else if(pcReg - atoi(curParse->operand[0] + 1) + MAX12DISP / 2 >= 0) {
                            curParse->objCode += (!nFlag ? 1 : 2);
                            curParse->objCode *= INC_HBYTE;
                            curParse->objCode += 0;
                            curParse->objCode *= INC_BYTE * INC_HBYTE;
                            curParse->objCode += atoi(curParse->operand[0] + 1);
                        }
                        break;
                    case format4:
                        eFlag = true;
                        // search for symbol
                        curSymbol = symTableSearch(curParse->operand[0] + (!isalnum(curParse->operand[0][0]) ? 1 : 0));
                        // if there is a symbol in operand
                        if(curSymbol) {
                            curParse->objCode += 3;
                            curParse->objCode *= INC_HBYTE;
                            curParse->objCode += 1;
                            curParse->objCode *= INC_BYTE * INC_BYTE * INC_HBYTE;
                            curParse->objCode += curSymbol->address & 0xFFFFF;
                        }
                        // if there is a numeric value in operand
                        else {
                            curParse->objCode += 1;
                            curParse->objCode *= INC_HBYTE;
                            curParse->objCode += 1;
                            curParse->objCode *= INC_BYTE * INC_BYTE * INC_HBYTE;
                            curParse->objCode += atoi(curParse->operand[0] + 1) & 0xFFFFF;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case PSEUDO: // pseudo-instruction
                switch(curParse->direcName) {
                    case START:
                        // initialize base register and remember first location
                        objFlag = false;
                        baseReg = curParse->location;
                        firstExec = curParse->location;
                        printOBJ(objFile, curParse, progLen, firstExec);
                        break;
                    case BASE:
                        // set base register
                        locFlag = objFlag = false;
                        baseReg = symTableSearch(curParse->operand[0])->address;
                        break;
                    case END:
                        locFlag = objFlag = false;
                        printOBJ(objFile, curParse, progLen, firstExec);
                        break;
                    case BYTE:
                        switch(curParse->operand[0][0]) {
                            case 'C': // a character constant
                                for(i = 2; curParse->operand[0][i] != '\''; i++) {
                                    curParse->objCode *= INC_BYTE;
                                    curParse->objCode += curParse->operand[0][i];
                                }
                                break;
                            case 'X': // a hexadecimal constant
                                curParse->objCode += strtol(curParse->operand[0] + 2, NULL, 16);
                                break;
                            default:
                                curParse->objCode = atoi(curParse->operand[0]);
                                break;
                        }
                        break;
                    case WORD:
                        curParse->objCode = atoi(curParse->operand[0]);
                        break;
                    case RESB:
                    case RESW:
                        objFlag = false;
                        break;
                    default:
                        break;
                }
                break;
            case COMMENT:
                locFlag = objFlag = false;
            default:
                break;
        }

        // if it has object code, print into .obj file
        if(curParse->objCode)
            printOBJ(objFile, curParse, progLen, firstExec);
        // print into .lst file
        printLST(lstFile, curParse, maxSrcLen, locFlag, objFlag);
        curParse = curParse->next;
    }

    if(errorFlag) {
        printASMError(curParse);
        symTableFree();
        return false;
    }

    return true;
}

void printLST(FILE* lstFile, ASM_SRC* parsedASM, int maxSrcLen, bool printLOC, bool printOBJ) {
    int i;
    // print line number
    fprintf(lstFile, "%4d\t", parsedASM->lineNum);

    // print location if flag is set
    (printLOC ? fprintf(lstFile, "%04X\t", parsedASM->location) : fprintf(lstFile, "\t"));

    // print the source line with blanks filled to match the max length
    for(i = 0; parsedASM->source[i]; i++)
        fprintf(lstFile, "%c", parsedASM->source[i]);
    for(;i < maxSrcLen; i++)
        fprintf(lstFile, " ");

    // print object code according to byte size
    if(printOBJ) {
        fprintf(lstFile, "\t\t");
        switch(parsedASM->byteSize) {
            case 1:
                fprintf(lstFile, "%02X", parsedASM->objCode);
                break;
            case 2:
                fprintf(lstFile, "%04X", parsedASM->objCode);
                break;
            case 3:
                fprintf(lstFile, "%06X", parsedASM->objCode);
                break;
            case 4:
                fprintf(lstFile, "%08X", parsedASM->objCode);
                break;
            default:
                break;
        }
    }
    fprintf(lstFile, "\n");
}

void printOBJ(FILE* objFile, ASM_SRC* parsedASM, int progLen, int firstExec) {
    static int lineLen = 0; // remember line lenght
    // if START directive
    if(parsedASM->direcName == START) {
        fprintf(objFile, "H%-6s%06X%06X\n", parsedASM->label, parsedASM->location, progLen);
        return;
    }
    // if END directive
    if(parsedASM->direcName == END) {
        printModList(objFile);
        fprintf(objFile, "E%06X\n", firstExec);
        return;
    }
    // if format 4 instruction with simple addressing add to modification record list
    if(parsedASM->format == format4 && symTableSearch(parsedASM->operand[0] + (!isalnum(parsedASM->operand[0][0]) ? 1 : 0)))
        addModList(parsedASM->location - firstExec + 1, 5);
    // if line lenght exceeds 0x1E
    if(lineLen + parsedASM->byteSize > 0x1E) {
        fprintf(objFile, "T%06X%02X", objList ? objList->location : parsedASM->location, lineLen);
        printOBJList(objFile);
        objListFree();
        lineLen = 0;
    }

    addOBJList(parsedASM);
    lineLen += parsedASM->byteSize;
    // if node was a variable, cut line
    if(parsedASM->direcName == BYTE || parsedASM->direcName == WORD) {
        fprintf(objFile, "T%06X%02X", objList ? objList->location : parsedASM->location, lineLen);
        printOBJList(objFile);
        objListFree();
        lineLen = 0;
    }
}

void printOBJList(FILE* objFile) {
    OBJ_CODE* cur = objList;
    // print depending on byte size
    while(cur) {
        switch(cur->byteSize) {
            case 1:
                fprintf(objFile, "%02X", cur->objCode);
                break;
            case 2:
                fprintf(objFile, "%04X", cur->objCode);
                break;
            case 3:
                fprintf(objFile, "%06X", cur->objCode);
                break;
            case 4:
                fprintf(objFile, "%08X", cur->objCode);
                break;
            default:
                break;
        }
        cur = cur->next;
    }
    fprintf(objFile, "\n");
}

void printModList(FILE* objFile) {
    MOD_RECORD* cur = modList;
    // print all modification records
    while(cur) {
        fprintf(objFile, "M%06X%02d\n", cur->location, cur->lenHB);
        cur = cur->next;
    }
}

void addOBJList(ASM_SRC* parsedASM) {
    OBJ_CODE *newOBJ, *cur = objList;
    // create new node of object code list
    newOBJ = (OBJ_CODE*) malloc(sizeof(OBJ_CODE));
    newOBJ->objCode = parsedASM->objCode;
    newOBJ->location = parsedASM->location;
    newOBJ->byteSize = parsedASM->byteSize;
    newOBJ->next = NULL;
    if(!objList) {
        objList = newOBJ;
        return;
    }
    while(cur->next)
        cur = cur->next;
    cur->next = newOBJ;
}

void addModList(int location, int length) {
    MOD_RECORD *newRec, *cur = modList;
    // create new node of modification record list
    newRec = (MOD_RECORD*) malloc(sizeof(MOD_RECORD));
    newRec->lenHB = length;
    newRec->location = location;
    newRec->next = NULL;
    if(!modList) {
        modList = newRec;
        return;
    }
    while(cur->next)
        cur = cur->next;
    cur->next = newRec;
}

// free ASM source parse list
void parseListFree() {
    ASM_SRC *cur, *next;
    cur = parseList;
    while(cur) {
        next = cur->next;
        free(cur);
        cur = next;
    }
    parseList = NULL;
}

// free object code list
void objListFree() {
    OBJ_CODE *cur, *next;
    cur = objList;
    while(cur) {
        next = cur->next;
        free(cur);
        cur = next;
    }
    objList = NULL;
}

// free modification record list
void modListFree() {
    MOD_RECORD *cur, *next;
    cur = modList;
    while(cur) {
        next = cur->next;
        free(cur);
        cur = next;
    }
    modList = NULL;
}

// print error detail of ASM source
void printASMError(ASM_SRC* parsedASM) {
    printf("ERROR: Invalid assembly source.\n");
    printf("[Line %d] Error in ", parsedASM->lineNum);
    switch(parsedASM->errorCode) {
        case SYMBOL:
            puts("symbol field:");
            break;
        case INSTRUCTION:
            puts("instruction field:");
            break;
        case OPERAND:
            puts("operand field:");
            break;
        default:
            break;
    }
    puts(parsedASM->source);
}

// COMMADN: symbol
void symbolCMD() {
    SYMBOL_ENTRY* cur = symTable;
    if(!symTable) {
        puts("Symbol table is empty.");
        return;
    }
    while(cur) {
        printf("\t%s\t%04X\n", cur->symbol, cur->address);
        cur = cur->next;
    }
}

ASM_SRC* parseASM(char* source) {
    char delim[] = " \t\n";
    char tmp[ASM_LEN] = {'\0'};
    char *tok = NULL;
    int i, j;
    ASM_SRC* parseResult = NULL;
    HASH_ENTRY* bucket = NULL;

    // initialize parse structure
    parseResult = (ASM_SRC*) malloc(sizeof(ASM_SRC));
    strcpy(parseResult->source, source);
    memset(parseResult->label, '\0', ASM_LEN);
    memset(parseResult->inst, '\0', ASM_LEN);
    memset(parseResult->operand[0], '\0', ASM_LEN);
    memset(parseResult->operand[1], '\0', ASM_LEN);
    parseResult->hasLabel = false;
    parseResult->indexing = false;
    parseResult->operandCnt = 0;
    parseResult->byteSize = 0;
    parseResult->objCode = 0;
    parseResult->type = INST;
    parseResult->direcName = NOTDR;
    parseResult->errorCode = OK;
    parseResult->next = NULL;

    // add space front and back of commas
    j = 0;
    for(i = 0; source[i]; i++) {
        if(source[i] == ',') {
            strcpy(tmp + j, " , ");
            j += 3;
        }
        else
            tmp[j++] = source[i];
    }

    tok = strtok(tmp, delim);

    if(tok[0] == '.') { // comment found
        parseResult->type = COMMENT;
        parseResult->format = NONE;
        return parseResult;
    }
    else if(!strcmp(tok, ",")) { // comma should not be present
        setError(parseResult, INSTRUCTION);
        return parseResult;
    }

    // look for directives
    for(i = 1; i < 8; i++)
        if(!strcmp(tok, directives[i])) { // pseudo instruction found
            parseResult->type = PSEUDO;
            parseResult->format = NONE;
            parseResult->direcName = i;
            strcpy(parseResult->inst, tok);
            break;
        }

    if(parseResult->type != PSEUDO) { // if not a pseudo instruction
        bucket = bucketSearch(tok + (tok[0] == '+' ? 1 : 0) );

        // if first field is a label
        if(!bucket) {
            if(symTableSearch(tok)) { // symbol already exists
                setError(parseResult, SYMBOL);
                return parseResult;
            }
            else { // new symbol found, add to SYMTAB and move on
                parseResult->hasLabel = true;
                strcpy(parseResult->label, tok);
                tok = strtok(NULL, delim);
                // missing instruction
                if(!tok) {
                    setError(parseResult, INSTRUCTION);
                    return parseResult;
                }

                bucket = bucketSearch(tok + (tok[0] == '+' ? 1 : 0) );

                if(!strcmp(tok, ",")) { // comma should not be present
                    setError(parseResult, INSTRUCTION);
                    return parseResult;
                }
            }
        }

        // look for directives
        for(i = 1; i < 8; i++)
            if(!strcmp(tok, directives[i])) { // pseudo instruction found
                parseResult->type = PSEUDO;
                strcpy(parseResult->inst, tok);
                parseResult->format = NONE;
                parseResult->direcName = i;
                break;
            }

        // if field is an instruction
        if(bucket) {
            parseResult->type = INST;
            parseResult->format = bucket->format + 1; // get format from hash table
            if(tok[0] == '+') {
                // not a format 3/4 instruction
                if(bucket->format != f34) {
                    setError(parseResult, INSTRUCTION);
                    return parseResult;
                }
                // found a format 4 instruction
                parseResult->format = format4;
            }
            strcpy(parseResult->inst, tok);
        }
        // invalid instruction
        else if(parseResult->type != PSEUDO){
            setError(parseResult, INSTRUCTION);
            return parseResult;
        }
    }

    // tokenize the operand field part
    tok = strtok(NULL, delim); // first operand
    if(bucket && ((bucket->operandCnt && !tok) || (!bucket->operandCnt && tok))) {
        setError(parseResult, OPERAND);
        return parseResult;
    }
    else if(bucket && !bucket->operandCnt)
        return parseResult;
    if(tok && !strcmp(tok, ",")) { // comma should not be present
        setError(parseResult, OPERAND);
        return parseResult;
    }
    strcpy(parseResult->operand[0], tok);
    parseResult->operandCnt = 1;
    tok = strtok(NULL, delim); // expected a comma if there's second operand
    if(tok && strcmp(tok, ",")) { // comma should be present
        setError(parseResult, OPERAND);
        return parseResult;
    }
    if(tok) {
        tok = strtok(NULL, delim); // second operand
        if(!tok || !strcmp(tok, ",")) {
            setError(parseResult, OPERAND);
            return parseResult;
        }
        strcpy(parseResult->operand[1], tok);
        parseResult->operandCnt = 2;
    }

    // check operand format for instructions
    if(parseResult->type == INST) {
        switch(parseResult->format) {
            case format2:
                // first operand must be a Register
                if(strlen(parseResult->operand[0]) != 1 || !isRegister(parseResult->operand[0][0])) {
                    setError(parseResult, OPERAND);
                    return parseResult;
                }
                // second operand for SHIFTL/R must be a number
                if(!strncmp(parseResult->inst, "SHIFT", 5)) {
                    for(i = 0; parseResult->operand[1][i]; i++)
                        if(!isdigit(parseResult->operand[1][i])) {
                            setError(parseResult, OPERAND);
                            return parseResult;
                        }
                }
                // second operand must be a Register
                else if(bucket->operandCnt != 1){
                    if(strlen(parseResult->operand[1]) != 1 || !isRegister(parseResult->operand[1][0])) {
                        setError(parseResult, OPERAND);
                        return parseResult;
                    }
                }
                break;
            case format3:
            case format4:
                // one non-register operand or second operand is not X (indexing)
                if((strlen(parseResult->operand[0]) == 1 && isRegister(parseResult->operand[0][0])) || (strlen(parseResult->operand[1]) && strcmp(parseResult->operand[1], "X")) ) {
                    setError(parseResult, OPERAND);
                    return parseResult;
                }
                if(strlen(parseResult->operand[1]))
                    parseResult->indexing = true;
                break;
            default:
                break;
        }
    }
    return parseResult;
}

// check for register name
bool isRegister(char reg) {
    int i;
    for(i = 0; i < 7; i++)
        if(regs[i] == reg)
            return true;
    return false;
}

// set information about error in node
void setError(ASM_SRC* parsedResult, ASM_ERROR error) {
    parsedResult->type = ERROR;
    parsedResult->format = NONE;
    parsedResult->errorCode = error;
}

// add new symbol to SYMTAB
void symTableAdd(char* symbol, int address) {
    SYMBOL_ENTRY* cur = symTable;
    SYMBOL_ENTRY* newEntry = (SYMBOL_ENTRY*) malloc(sizeof(SYMBOL_ENTRY));
    strcpy(newEntry->symbol, symbol);
    newEntry->address = address;
    newEntry->next = NULL;

    // if SYMTAB is empty
    if(!symTable) {
        symTable = newEntry;
        return;
    }
    // add in a non ascending order
    if(strcmp(symTable->symbol, symbol) < 0) {
        newEntry->next = symTable;
        symTable = newEntry;
        return;
    }
    // while next symbol is bigger in alphabetical order
    while(cur->next && strcmp(cur->next->symbol, symbol) > 0)
        cur = cur->next;
    newEntry->next = cur->next;
    cur->next = newEntry;
}

SYMBOL_ENTRY* symTableSearch(char* symbol) {
    SYMBOL_ENTRY* cur = symTable;
    while(cur) {
        // if a match is found
        if(!strcmp(symbol, cur->symbol))
            return cur;
        cur = cur->next;
    }
    return NULL;
}

// free SYMTAB
void symTableFree() {
    SYMBOL_ENTRY *cur, *next;
    cur = symTable;
    while(cur) {
        next = cur->next;
        free(cur);
        cur = next;
    }
    symTable = NULL;
}
