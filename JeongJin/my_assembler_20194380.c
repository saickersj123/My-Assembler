/*
    시스템 프로그래밍 과제물
    20194380 문정진
*/
#define _CRT_SECURE_NO_WARNINGS	
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "my_assembler_20194380.h"
#include "Operation_status.h"

int main(void) {
    init_my_assembler();
    int t =0;
    for (t; t < MAX_LINES; t++) {
        uchar *currentline = input_data[t];
            token_parsing(currentline);
    }

    //어셈블러 작업을 위한 준비 시작
    //assem_pass1();
    //pass1 시작
}

int init_my_assembler(void) {
    init_inst_file("inst.data");
    //inst.data파일을 inst_table구조체에 저장하기 위한 함수

    init_input_file("input.txt");
    //input.txt파일을 input_data배열에 저장하기 위한 함수
    return 0;
}

int init_inst_file(uchar* inst_file) {
    FILE* inst_data_file = fopen(inst_file, "r");
    //inst_file로 전달받은 inst.data파일을 읽기 모드로 불러온 후 포인터형 inst_data_file변수에 대입 
    if (inst_data_file == NULL) {
        //inst.data파일 불러오기 오류
        printf("파일을 열 수 없습니다.\n");
        return 1;
    }
    inst_index = 0;
    for (inst_index = 0; inst_index < MAX_INST; inst_index++) {
        inst_table[inst_index] = (inst*)malloc(sizeof(inst));
    }
    inst_index = 0;
    uchar temp_storage[3];
    while (inst_index < MAX_INST && fscanf(inst_data_file, "%s %s %d %02x",
        inst_table[inst_index]->str,
        &temp_storage[0],
        &inst_table[inst_index]->format,
        &inst_table[inst_index]->op) != EOF) {
        // 읽은 데이터를 구조체에 저장하는 작업 수행.
        // 다음 구조체 인덱스로 이동
        //	0:-		1:M		2:R		3:N		4:RR	5:RN
        if (strcmp(temp_storage, "-") == 0) {
            inst_table[inst_index]->ops = 0;
        }
        else if (strcmp(temp_storage, "M") == 0) {
            inst_table[inst_index]->ops = 1;
        }
        else if (strcmp(temp_storage, "R") == 0) {
            inst_table[inst_index]->ops = 2;
        }
        else if (strcmp(temp_storage, "N") == 0) {
            inst_table[inst_index]->ops = 3;
        }
        else if (strcmp(temp_storage, "RR") == 0) {
            inst_table[inst_index]->ops = 4;
        }
        else if (strcmp(temp_storage, "RN") == 0) {
            inst_table[inst_index]->ops = 5;
        }
        inst_index++;
    }

    fclose(inst_data_file);
    // 파일 닫기
    return 0;
}
int init_input_file(uchar* input_file) {
    int i;
    uchar* temp_char[MAX_LINES];
    //문자열을 임시로 저장할 곳.
    FILE* input_txt_file = fopen(input_file, "r");
    //input_file로 전달받은 input.txt파일을 읽기 모드로 불러온 후 포인터형 input_txt_file변수에 대입 
    if (input_txt_file == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return 1;
    }
    for (i = 0; i < MAX_LINES; i++) {
        temp_char[i] = (uchar*)malloc(MAX_LINES);
        input_data[i] = (uchar*)malloc(MAX_LINES);
    }
    i = 0;
    while (i < MAX_LINES && fgets(temp_char, MAX_LINES, input_txt_file) != NULL) {
        strcpy(input_data[i], temp_char);
        i++;
    }
    fclose(input_txt_file);

    return 0;
}
int token_parsing(uchar* before_str) {
    token_table[token_line] = (token*)malloc(sizeof(token));
    
    char* token_str = strtok(before_str, " \t\n");

    if (token_str != NULL && token_str[0] != '.') {
        token_table[token_line]->label = _strdup(token_str);
    }
    else if (token_str == NULL) {
        token_table[token_line]->label = "";
    }
    else if (token_str[0] == '.') {
        return;
    }

    token_str = strtok(NULL, " \t\n");
    if (token_str != NULL) {
        token_table[token_line]->operator = _strdup(token_str);
    }
    else {
        strcpy(token_table[token_line]->operator, "");
    }

  
    for (int i = 0; i < 1; i++) {
        token_str = strtok(NULL, " \t\n");

        if (token_str != NULL) {
            strcpy(token_table[token_line]->operand[i], token_str);
        }
        else {
            strcpy(token_table[token_line]->operand[i], "");
        }
    }

    /* operand 0 1 2를 구분하기 전에 사용된 코드임.*/
    /*
    for (int i = 0; i < 1; i++) {
        token_str = strtok(NULL, " \t\n");

        if (token_str != NULL) {
            strcpy(token_table[token_line]->operand[i], token_str);
        }
        else {
            strcpy(token_table[token_line]->operand[i], "");
        }
    }
    */

    token_str = strtok(NULL, "\n");
    if (token_str != NULL) {
        strcpy(token_table[token_line]->comment, token_str);
    }
    else {
        strcpy(token_table[token_line]->comment, "");
    }
    token_line++;

    //밑은 토큰화 테스트 코드임.
    printf("[LINE : %2d]\n", token_line + 1);
    printf("label : %s\n", token_table[token_line]->label);
    printf("operator : %s\n", token_table[token_line]->operator);
    for (int i = 0; i < MAX_OPERAND; i++) {
        printf("Operand %d: %s\n", i + 1, token_table[token_line]->operand[i]);
    }
    printf("Comment: %s\n", token_table[token_line]->comment);


    printf("\n\n");
    return 0;
}

static int assem_pass1(void) {
    make_opcode_output("optab.txt");
    //상대주소? 방식 넣을 것
    make_symtab_output("symtab.txt");
}

void make_opcode_output(uchar* optable_txt) {
    FILE* optab_file = fopen("optab.txt", "w");
    if (optab_file == NULL) {
        printf("optab.txt 파일을 만들 수 없습니다.\n");
        return 1;
    }
    return 0;
}


void make_symtab_output(uchar* symboltable_txt) {
    FILE* symtab_file = fopen("symtab.txt", "w");
    if (symtab_file == NULL) {
        printf("symtab.txt파일을 만들 수 없습니다.\n");
        return 1;
    }

    return 0;
}

int search_opcode(uchar* str);

static int assem_pass2(void);
void make_objectcode_output(uchar* file_name);