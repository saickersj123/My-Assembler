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
 * File name: linkedList.c                                       *
 * File description: Manages functions related to linked lists.  *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "20161577.h"
#include "linkedList.h"

void addToList(LIST* listHead, void* data) {
    NODE* cur = *listHead;
    NODE* newNode = malloc(sizeof(NODE));
    newNode->data = data;
    newNode->next = NULL;
    if(!cur) {
        *listHead = newNode;
        return;
    }
    while(cur->next)
        cur = cur->next;
    cur->next = newNode;
}

void freeList(LIST* listHead) {
    NODE* cur = *listHead;
    NODE* next = NULL;
    while(cur) {
        next = cur->next;
        free(cur->data);
        free(cur);
        cur = next;
    }
    *listHead = NULL;
}

void opCodeTableFree() {
    int i;
    for(i = 0; i < HASH_SIZE; i++)
        freeList(opCodeTable + i);
}

void extSymTableFree() {
    NODE* cur = extSymTable;
    while(cur) {
        freeList(&(((CNT_SEC*)(cur->data))->extSym));
        cur = cur->next;
    }
    freeList(&extSymTable);
}

void printList(LIST listHead, void (fptr)(void*)) {
    NODE* cur = listHead;
    while(cur) {
        (fptr)(cur->data);
        cur = cur->next;
    }
}

void printHistory(void* data) {
    static int index = 1;
    if(data == histList->data)
        index = 1;
    printf("%-3d  ", index++);
    puts(((HIST_NODE*)data)->str);
}

void printOpList(void* data) {
    printf("[%s,%s]", ((HASH_ENTRY*)data)->inst, ((HASH_ENTRY*)data)->code);
    if(((HASH_ENTRY*)data)->next)
        printf(" -> ");
}

void printCntSecTable(void* data) {
    if(data == extSymTable->data) {
        printf("Control\t\tSymbol\t\tAddress\t\tLength\n");
        printf("section\t\tname\n");
        printf("---------------------------------------------------------\n");
    }
    printf("%s\t\t\t\t%04X\t\t%04X\n", ((CNT_SEC*)data)->csName, ((CNT_SEC*)data)->stAddress, ((CNT_SEC*)data)->length);
    printList(((CNT_SEC*)data)->extSym, printExtSym);
}

void printExtSym(void* data) {
    printf("\t\t%s\t\t%04X\n", ((EXT_SYMBOL*)data)->symName, ((EXT_SYMBOL*)data)->address);
}

void printBreakPntList(void* data) {
    printf("\t%04X\n", ((BREAK_PNT*)data)->address);
}
