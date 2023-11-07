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
 * File name: memory.h                                           *
 * File description: Header file for memory related tasks.       *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

short mem[MEM_SIZE];        // virtual memory (1MB)

void dumpCMD(INPUT_CMD);    // COMMAND: dump
void editCMD(INPUT_CMD);    // COMMAND: edit
void fillCMD(INPUT_CMD);    // COMMAND: fill
void resetCMD();            // COMMAND: reset
