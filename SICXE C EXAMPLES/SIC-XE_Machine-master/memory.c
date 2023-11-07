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
 * File name: memory.c                                           *
 * File description: Tasks related to virtual memory handing.    *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "20161577.h"
#include "memory.h"

void dumpCMD(INPUT_CMD ipcmd) {
    static int start = 0;
    int end, i, j;

    if(start >= MEM_SIZE)
        start = 0; // if start address exceeded memory limit, reset to 0x00000
    end = start + 159; // set initial end value
    
    if(ipcmd.argCnt) { // if there was argument entered in command
        start = hexToDec(ipcmd.arg[0]);
        end = start + 159;
        if(ipcmd.argCnt == 2) // if there was 2 arguments
            end = hexToDec(ipcmd.arg[1]);
    }
    if(end >= MEM_SIZE)
        end = MEM_SIZE - 1; // if end address exceed memory limit, set to 0xFFFFF

    // start loop from the beginning of each 16 Bytes
    // finish loop till end of each 16 Bytes
    for(i = start / 16 * 16; i < (end / 16 + 1) * 16; i++) {
        if(!(i % 16))
            printf("%05X ", i); // print address for beginning of each line
        if(i < start || i > end)
            printf("   "); // do NOT dump memory outside print range
        else
            printf("%02X ", mem[i]); // dump memory in hexadecimal value
        if(!((i + 1) % 16)) { // line finished
            printf("; ");
            // print content converted into ASCII character
            for(j = i - 15; j <= i; j++)
                printf("%c", ((j >= start && j <= end) && mem[j] >= 32 && mem[j] <= 126) ? mem[j] : '.');
            puts("");
        }
    }
    start = end + 1; // remember last print address
}

void editCMD(INPUT_CMD ipcmd) {
    int add, val;
    add = hexToDec(ipcmd.arg[0]); // address to edit
    val = hexToDec(ipcmd.arg[1]); // replace value
    mem[add] = val;
}

void fillCMD(INPUT_CMD ipcmd) {
    int i, start, end, val;
    start = hexToDec(ipcmd.arg[0]); // fill start address
    end = hexToDec(ipcmd.arg[1]);   // fill end address
    val = hexToDec(ipcmd.arg[2]);   // fill value
    for(i = start; i <= end; i++)
        mem[i] = val;
}

void resetCMD() {
    int i;
    for(i = 0; i < MEM_SIZE; i++)
        mem[i] = 0; // set memory to 0x00000
    for(i = 0; i < REG_CNT; i++)
        registers[i] = 0; // initialize registers
    progAddr = execAddress = endAddress = 0;
}
