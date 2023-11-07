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
 * File name: assembler.h                                        *
 * File description: Header file for assembler tasks.            *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool assembleCMD(INPUT_CMD); // COMMAND: assemble
ASM_SRC* parseASM(char*); // parse ASM source
bool assemblerPass1(FILE*, int*, int*); // Pass1 of SIC/XE Assembler
bool assemblerPass2(FILE*, FILE*, int, int); // Pass2 of SIC/XE Assembler
void printLST(FILE*, ASM_SRC*, int, bool, bool); // create content into .lst file
void printOBJ(FILE*, ASM_SRC*, int, int); // create content into .obj file
void printOBJList(FILE*); // print object code list
void printModList(FILE*); // print modification record list
void addOBJList(ASM_SRC*); // add node to object code list
void addModList(int, int); // add node to modification record list
void setError(ASM_SRC*, ASM_ERROR); // mark error in ASM source
void printASMError(ASM_SRC*); // print detail about error
bool isRegister(char); // check whether operand is a register name

void symbolCMD(); // COMMAND: symbol
void symTableAdd(char*, int); // add node to SYMTAB
SYMBOL_ENTRY* symTableSearch(char*); // search for node in SYMTAB
