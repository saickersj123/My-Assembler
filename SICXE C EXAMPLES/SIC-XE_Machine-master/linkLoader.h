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
 * File name: linkLoader.h                                       *
 * File description: Header file for linking loaderd tasks.      *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void pAddrCMD(INPUT_CMD);       // progaddr COMMAND
bool loaderCMD(INPUT_CMD);      // loader COMMAND

int linkLoaderPass1(FILE**);    // Pass1 of linking loader
int linkLoaderPass2(FILE**);    // Pass2 of linking loader

void fcloseObj(FILE**);         // close obj file pointers
bool searchCS(char*);           // search for control section in ESTAB
int searchES(char*);            // search for external symbol in ESTAB
