< Project 3 - SIC/XE Linking Loader >

0. How to execute program

* After unzipping, there should be:
- 9 *.h header files
- 9 *.c source code files
- Makefile
- Document.docx
- README.txt
- opcode.txt
- proga.obj, progb.obj, progc.obj
- copy.obj

* Then, type the following to execute program:
$ make
$ ./20161577.out

* To cleanup directory after execution, type:
$ make clean


1. Description

This is the third project for the System Programmig course in the Department of Computer Science and Engineering at Sogang University, Seoul, South Korea.
The implementation of the SIC/XE Machine Linking Loader is added to the previous project.


2. Specification

- Allocated virtual memory: 1 MB


3. Allowed Commands

- h[elp]                    : show list and format of commands.
- d[ir]                     : show list of entries in current directory.
- q[uit]                    : exit system.
- hi[story]                 : show list of valid commands entered during current session.
- du[mp] [start, end]       : print content of virtual memory.
- e[dit] address, value     : change value at designated memory address.
- f[ill] start, end, value  : batch fill an interval in address with a value.
- reset                     : set whole memory to 0x00.
- opcode mnemonic           : lookup for the code of the opcode mnemonic.
- opcodelist                : show opcode list stored in hash table.
- assemble filename         : assemble .asm source file
- type filename             : print content of specified file
- symbol                    : show content of symbol table of the latest succesfully assembled source.
- progaddr                  : designate starting address
- loader                    : linking load program
- run                       : run program
- bp                        : create break point


4. About the Developer

Inho Kim, 20161577,
Undergraduate student of Dept. of Computer Science and Engineering,
Sogang University,
Seoul, South Korea
