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
 * File name: cmdProc.h                                          *
 * File description: Header file for input processing tasks.     *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

INPUT_CMD findCMD(char*);                       // function to find command type of input
ERROR_CODE testValidInput(INPUT_CMD, COMMAND);  // function to check validity of input command

void invFormatCMD();    // called when invalid format input
void invHexCMD();       // called when invalid hexadecimal value
void invValCMD();       // called when invalid value
void invFileCMD();      // called when invalid filename
