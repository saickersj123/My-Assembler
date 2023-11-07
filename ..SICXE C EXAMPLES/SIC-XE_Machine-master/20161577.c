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
 * File name: 20161577.c                                         *
 * File description: Main file for the project.                  *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "20161577.h"
#include "cmdProc.h"
#include "shell.h"
#include "memory.h"
#include "hash.h"
#include "assembler.h"
#include "linkLoader.h"
#include "execute.h"

int main() {
    char inp[CMD_LEN];  // input string
    char tmp[CMD_LEN];  // temporary string to copy input
    int i, j;
    INPUT_CMD input;    // storage for parsed input

    for(i = 0; i < REG_CNT; i++)
        registers[i] = 0; // initialize registers
    resetCMD(); // initialize memory
    hashCreate(); // create hash table of opcodes
    progAddr = 0; // default program starting address

    while(1) {
        printf("sicsim> ");
        fgets(inp, CMD_LEN, stdin); // get input string
        inp[strlen(inp) - 1] = '\0'; // replace \n with null character

        // copy input string to tmp but place one space before and after comma ','
        j = 0;
        for(i = 0; inp[i]; i++) {
            if(inp[i] == ',') {
                strcpy(tmp + j, " , "); // place space around commma ','
                j += 3;
            }
            else
                tmp[j++] = inp[i];
        }
        tmp[j] = '\0';

        input = findCMD(tmp); // find the command format from input string
        switch(input.cmd) {
            case assemble:
            case loader:
            case bp:
            case invFormat:
            case invHex:
            case invVal:
            case invFile:
                break;
            default:
                histAdd(inp); // if command is not invalid add to history
        }

        // call function for each command
        switch(input.cmd) {
            case help:
                helpCMD();
                break;
            case dir:
                dirCMD();
                break;
            case quit:
                quitCMD();
                break;
            case hist:
                histCMD();
                break;
            case dump:
                dumpCMD(input);
                break;
            case edit:
                editCMD(input);
                break;
            case fill:
                fillCMD(input);
                break;
            case reset:
                resetCMD();
                break;
            case op:
                opCMD(input);
                break;
            case oplist:
                oplistCMD();
                break;
            case type:
                typeCMD(input);
                break;
            case assemble:
                if(assembleCMD(input))
                    histAdd(inp);
                break;
            case symbol:
                symbolCMD();
                break;
            case pAddr:
                pAddrCMD(input);
                break;
            case loader:
                if(loaderCMD(input))
                    histAdd(inp);
                break;
            case run:
                runCMD();
                break;
            case bp:
                if(bpCMD(input))
                    histAdd(inp);
                break;
            case invFormat:
                invFormatCMD();
                break;
            case invHex:
                invHexCMD();
                break;
            case invVal:
                invValCMD();
                break;
            case invFile:
                invFileCMD();
                break;
        }
    }
    return 0;
}
