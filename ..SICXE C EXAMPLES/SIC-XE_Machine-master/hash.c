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
 * File name: hash.c                                             *
 * File description: Tasks handling opcode and hash table.       *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "20161577.h"
#include "linkedList.h"
#include "hash.h"

// Format 2 instruction but 1 operand
char exceptionFmt2[][OPCODE_LEN] = {
    "CLEAR",
    "SVC",
    "TIXR"

};
// Format 3 instruction but NO operand
char exceptionFmt3[][OPCODE_LEN] = {
    "RSUB"
};

void opCMD(INPUT_CMD ipcmd) {
    HASH_ENTRY* bucket = bucketSearch(ipcmd.arg[0]);

    if(bucket) // target found!
        printf("opcode is %s\n", bucket->code);
    else { // target NOT found..
        puts("ERROR: mnemonic not found.");
        puts("Type \"opcodelist\" for list of available opcodes.");
    }
}

void oplistCMD() {
    NODE* bucket;
    int i;

    for(i = 0; i < HASH_SIZE; i++) {
        printf("%2d : ", i); // print table index
        bucket = opCodeTable[i]; // get front bucket
        printList(bucket, printOpList);
        /*
        while(bucket) {
            printf("[%s,%s]", bucket->inst, bucket->code);
            if((bucket = bucket->next)) // if there exists next bucket
                printf(" -> ");
        }
        */
        puts("");
    }
}

void hashCreate() {
    FILE* fp = fopen("opcode.txt", "r"); // open file
    char cd[3], ins[10], md[4];
    void* bucket;
    int hash;

    if(!fp) {
        puts("ERROR: Unable to load \"opcode.txt\".");
        return;
    }

    while(fscanf(fp, "%s %s %s", cd, ins, md) == 3) {
        // allocate and fill in new data
        bucket = malloc(sizeof(HASH_ENTRY));
        strcpy(((HASH_ENTRY*)bucket)->code, cd);
        strcpy(((HASH_ENTRY*)bucket)->inst, ins);
        ((HASH_ENTRY*)bucket)->codeVal = hexToDec(cd);
        ((HASH_ENTRY*)bucket)->format = md[0] - '1';
        checkOperandCnt(bucket);
        ((HASH_ENTRY*)bucket)->next = NULL;

        // calculate hash
        hash = hashFunction(((HASH_ENTRY*)bucket)->inst);
        // add to generic list
        addToList(opCodeTable + hash, bucket);
        // add to internal list for print issues
        hashAddBucket(hash, bucket);
    }

    if(fclose(fp)) {
        puts("WARNING: Error closing \"opcode.txt\".");
        return;
    }
}

void checkOperandCnt(void* bucket) {
    int i;
    switch(((HASH_ENTRY*)bucket)->format) {
        case f1: // Format 1 instruction
            ((HASH_ENTRY*)bucket)->operandCnt = 0;
            break;
        case f2: // Format 2 instruction
            ((HASH_ENTRY*)bucket)->operandCnt = 2; // Default is 2 operands
            for(i = 0; i < 3; i++) // Exception check (CLEAR, SVC, TIXR)
                if(!strcmp(((HASH_ENTRY*)bucket)->inst, exceptionFmt2[i])) {
                    ((HASH_ENTRY*)bucket)->operandCnt = 1;
                    break;
                }
            break;
        case f34: // Format 3 or 4 instruction
            ((HASH_ENTRY*)bucket)->operandCnt = 1; // Default is 1 operand
            for(i = 0; i < 1; i++) // Exception check (RSUB)
                if(!strcmp(((HASH_ENTRY*)bucket)->inst, exceptionFmt3[i])) {
                    ((HASH_ENTRY*)bucket)->operandCnt = 0;
                    break;
                }
            break;
    }
}

int hashFunction(char* inst) {
    return abs( (int) inst[0] * 2 + abs(inst[0] + inst[1] + inst[2]) ) % HASH_SIZE;
}

void hashAddBucket(int hash, void* bucket) {
    HASH_ENTRY* cur = (HASH_ENTRY*)(opCodeTable[hash]->data);

    if(!strcmp(cur->inst, ((HASH_ENTRY*)bucket)->inst)) // if it is the first bucket
        return;
    while(cur->next)
        cur = cur->next; // go to the end of list
    cur->next = (HASH_ENTRY*)bucket;
}

HASH_ENTRY* bucketSearch(char* inst) {
    void* bucket;
    char tmp[ASM_LEN] = {'\0'};

    strcpy(tmp, inst);
    bucket = opCodeTable[hashFunction(tmp)]->data; // get front bucket from hash function
    while(bucket && strcmp(((HASH_ENTRY*)bucket)->inst, inst)) // search till match or end
        bucket = ((HASH_ENTRY*)bucket)->next;
    return (HASH_ENTRY*)bucket;
}
