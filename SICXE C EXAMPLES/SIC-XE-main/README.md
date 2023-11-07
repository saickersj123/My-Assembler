# SIC/XE ASSEMBLER
##  What is SIC/XE Machhine
* SIC Machine's extended Edition
* you can read article at [SIC/XE](https://en.wikipedia.org/wiki/Simplified_Instructional_Computer) on wikipedia
## Function
* Control Section
* Addressing Mode (Flag bit)
  * Direct Addressing Mode
  * Indirect Addressing Mode
  * Simple Addressing Mode
  * Immediate Addressing Mode
  * Relative Addressing Mode
    *   1. Program Counter (PC Register)
    *   2. Base (Base Register)
* Extended Instruction (4 bit Instruction)

## How it works :
Using two passes ...
* Pass one
  * Assign addresses to all statements in source code
  * Save values (addresses) assigned to labels for use in pass two
  * Process directives
* Pass two
  * Translate instructions
  * Convert labels to addresses
  * Generate values defined by BYTE and WORD
  * Process the directives not done in pass one
  * Write object code to output file


## Compiling and running the program

### For Windows

GCC compiler
```
gcc SicXEpass1.c -o SicXEpass1.exe
gcc SicXEpass2.c -o SicXEpass2.exe
```
### For Linux/Mac

GCC compiler
```
gcc SicXEpass1.c -o SicXEpass1.o
gcc SicXEpass2.c -o SicXEpass2.o
```

## Run Precompiled Binaries

### For Windows

CMD
```
SicXEpass1.exe
SicXEpass2.exe
```
Powershell
```
&'.\SicXEpass1.exe'
&'.\SicXEpass2.exe'
```

### For Linux/Mac

```
./SicXEpass1.o
./SicXEpass2.o
```


