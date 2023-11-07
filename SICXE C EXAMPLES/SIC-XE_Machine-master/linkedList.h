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
 * File name: linkedList.h                                       *
 * File description: Header file for linked list related tasks.  *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

LIST histList;
LIST opCodeTable[HASH_SIZE];
LIST extSymTable;
LIST breakPntList;

void addToList(LIST*, void*);

void freeList(LIST*);
void opCodeTableFree();
void extSymTableFree();

void printList(LIST, void (void*));
void printHistory(void*);
void printOpList(void*);
void printCntSecTable(void*);
void printExtSym(void*);
void printBreakPntList(void*);
