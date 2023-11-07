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
 * File name: shell.c                                            *
 * File description: Tasks processed in shell environment.       *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "20161577.h"
#include "linkedList.h"
#include "shell.h"

void helpCMD() {
    printf("h[elp]\n"
            "d[ir]\n"
            "q[uit]\n"
            "hi[story]\n"
            "du[mp] [start, end]\n"
            "e[dit] address, value\n"
            "f[ill] start, end, value\n"
            "reset\n"
            "opcode mnemonic\n"
            "opcodelist\n"
            "assemble filename\n"
            "type filename\n"
            "symbol\n"
            "progaddr [address]\n"
            "loader [object filename1] [object filename2] [...]\n"
            "run\n"
            "bp [address]\n");
}

void dirCMD() {
    DIR* dir = opendir("."); // current directory
    char* entStr;
    char path[258] = "./"; // entry path string
    ENTRY* ent; // entry
    STBUF buf; // stat

    if(!dir) {
        puts("ERROR opening directory...");
        return;
    }
    ent = readdir(dir); // read entry
    while(ent) {
        path[2] = '\0'; // clear path string
        entStr = ent->d_name; // entry name
        stat(strcat(path, entStr), &buf);
        printf("%-s", entStr); // print entry name

        if(S_ISDIR(buf.st_mode)) // check for directory
            printf("/");
        else if(buf.st_mode & S_IXUSR) // check for exec file
            printf("*");

        ent = readdir(dir); // read next entry
        if(ent)
            puts("");
    }
    closedir(dir);
    puts("");
}

void quitCMD() {
    puts("Exiting SIC...");
    freeList(&histList);    // free history linked list
    opCodeTableFree();      // free opcode hash table
    symTableFree();         // free SYMTAB
    parseListFree();        // free ASM parse list
    objListFree();          // free object code list
    modListFree();          // free modification record list
    extSymTableFree();      // free ESTAB
    freeList(&breakPntList);// free break point list
    exit(0);
}

void histCMD() {
    printList(histList, printHistory);
}

void typeCMD(INPUT_CMD ipcmd) {
    FILE* fp = fopen(ipcmd.arg[0], "r");
    char c;

    if(!fp) {
        puts("ERROR: File not found.");
        return;
    }
    while((c = fgetc(fp)) != EOF)
        putchar(c);

    if(fclose(fp))
        puts("WARNING: Error closing file.");
}

void histAdd(char* str) {
    void* data = malloc(sizeof(HIST_NODE));
    strcpy(((HIST_NODE*)data)->str, str);
    addToList(&histList, data);
}
