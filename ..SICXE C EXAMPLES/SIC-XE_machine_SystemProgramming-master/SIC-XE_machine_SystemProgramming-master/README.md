# SIC-XE_machine_SystemProgramming
"SIC/XE Machine" System Programming, Sogang University, 2019 Spring Season


## Overview
이 프로젝트는 SIC/XE machine을 구현하는 것을 목적으로 하는 프로젝트입니다. 사용자의 입력을 받아 해당하는 명령을 수행하고, 메모리와 opcode 에 관련한 것들에 관여합니다.

## Guidelines

Typically you run make by simply typing the following command at the shell prompt.

    make
              
              
In this project, after you type 'make' at the shell prompt, it builds TARGET specified in the Makefile. After this process, if you type:
                    
    ./20171672
    
the program will run.

To delete the executable file and all the object files from the directory, type:

    make clean
    
## Shell Related Commands
### help
사용자에게 어떠한 명령들이 있는지 보여 준다.

### dir
사용자의 현재 디렉토리에 있는 파일들과 상태를 보여 준다.

### quit
프로그램을 종료한다.

### history
현재까지 실행했던 명령들을 나열하여 보여 준다.

### type filename
    type 2_5.obj
사용자에게 파일의 내용을 보여 준다.



## Memory Related Commands

### dump start, end
    dump
  
    dump 32
  
    dump 5, 49
해당 주소 메모리 공간의 내용을 출력한다.

### edit address, value
    edit 6, 7D
입력받은 주소에 입력받은 값을 메모리에 넣는다.

### fill start, end, value
    fill 5, 34, 3A
입력받은 시작 주소와 끝 주소에 입력받은 값으로 채운다.

### reset
메모리를 모두 0으로 초기화한다.

## Opcode Related Commands
### opcode mnemonic
    opcode MULF
    opcode STSW
    
입력받은 mnemonic 에 상응하는 opcode를 보여 준다.

### opcodelist
해시 테이블을 이용하여 opcodelist를 보여 준다.


## Assembler Related Commands
### assemble filename
    assemble 2_5.asm
주어진 소스 파일을 읽어 오브젝트 파일과 리스트 파일을 생성한다.

### symbol
assemble 명령 이후 생성된 심볼 테이블을 내림차순으로 출력하여 보여 준다.


## Loader Realted Commands
### progaddr address
    progaddr 4000
프로그램 시작 주소를 지정한다.

### load filenames
    loader proga.obj progb.obj progc.obj
오브젝트 파일을 읽어 링킹과 로딩을 한다.

## Execution Related Commands
### run
프로그램을 실행한다.

### bp
breakpoint를 지정한다.


