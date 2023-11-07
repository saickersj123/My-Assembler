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
 * File name: hash.h                                             *
 * File description: Header file for hash table related tasks.   *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define OPCODE_LEN 7

// HASH_ENTRY* hashTable[HASH_SIZE]; // hash table pointer array

void opCMD(INPUT_CMD);  // COMMAND: opcode
void oplistCMD();       // COMMAND: opcodelist

void hashCreate();                      // function to create hash table
void checkOperandCnt(void*);
void hashAddBucket(int, void*);   // function to add bucket to hash table
int hashFunction(char*);                // function that returns hash function
