/*
    시스템 프로그래밍 과제물
    20194380 문정진
*/
#define _CRT_SECURE_NO_WARNINGS	
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "my_assembler_20194380.h"
#include "Operation_status.h"   //내가 어디까지 작업을 했는지 나타내는 파일로 어셈블러의 역할과는 무관

int main(void) {
    init_my_assembler();
    //어셈블러 작업을 위한 준비 시작
    assem_pass1();
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

    //밑에 코드는 inst.data파일을 잘 저장했는지 확인을 하는 코드임.
    /*
    int digits = countDigits(inst_index);
    int i;
    for (i = 0; i < inst_index; i++) {
        printf("[Line : %*d] : str: %9s,\t ops : %d,\t format : %d,\t op : %02x\n",
            digits, i + 1,
            inst_table[i]->str,
            inst_table[i]->ops,
            inst_table[i]->format,
            inst_table[i]->op);
    }
    */
    fclose(inst_data_file);
    // 파일 닫기
    return 0;
}
int init_input_file(uchar* input_file) {
    int i = 0;
    uchar temp_char[MAX_LINES];
    int isFirstCharSpace;
    FILE* input_txt_file = fopen(input_file, "r");
    //input_file로 전달받은 input.txt파일을 읽기 모드로 불러온 후 포인터형 input_txt_file변수에 대입 
    if (input_txt_file == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return 1;
    }
    for (i = 0; i < MAX_LINES; i++) {
        input_data[i] = (uchar*)malloc(MAX_LINES);
        temp_char[i] = (uchar*)malloc(MAX_LINES);
    }
    i = 0;
    while (i < MAX_LINES && fgets(temp_char, MAX_LINES, input_txt_file) != NULL) {
        temp_char[strcspn(temp_char, "\n")] = '\0';
        strcpy(input_data[i], temp_char);
        bool isTabBeforeString = false;
        char* ptr = temp_char;
        char newString[MAX_LINES + 1]; // +1은 '-'를 추가하기 위함
        while (*ptr != '\0' && (*ptr == ' ' || *ptr == '\t')) {
            if (*ptr == '\t') {
                isTabBeforeString = true;
            }
            strcpy(newString, "-");
            strcat(newString, temp_char);
            ptr++;
        }
        token_line = i;
        if (isTabBeforeString == true) {
            //공백이 먼저 나온 경우 실행
            token_parsing(newString);
        }
        else {
            //문자가 먼저 나온 경우 실행
            uchar* line = temp_char;
            token_parsing(line);
        }
        i++;
    }
    fclose(input_txt_file);
    //밑에 있는 주석 처리된 코드는 input.txt파일을 잘 저장을 했는지 확인을 하기 위해 작성된 코드임.
    /*
    int digits = countDigits(i);
    int test_index;
    printf("파일에서 읽은 데이터:\n");
    for (test_index = 0; test_index < i; test_index++) {
        printf("Line %*d: %s\n", digits, test_index + 1, input_data[test_index]);
    }
    */
    return 0;
}
int token_parsing(uchar* before_str) {
    token_table[token_line] = (token*)malloc(sizeof(token));

    token_table[token_line]->label = NULL;
    token_table[token_line]->operator = NULL;
    for (int j = 0; j < MAX_OPERAND; ++j) {
        token_table[token_line]->operand[j][0] = '\0';  // 빈 문자열로 초기화
    }
    token_table[token_line]->comment[0] = '\0';  // 빈 문자열로 초기화


    char* token_str = strtok(before_str, " \t\n");

    if (token_str != NULL && token_str[0] != '.' && token_str[0] != '-') {
        token_table[token_line]->label = _strdup(token_str);
    }
    else if (token_str == NULL && token_str[0] != '.') {
        token_table[token_line]->label = "\0";
    }
    else if (token_str[0] == '.') {
        return;
    }

    token_str = strtok(NULL, " \t\n");
    if (token_str != NULL) {    
        token_table[token_line]->operator = _strdup(token_str);
    }
    else{
        strcpy(token_table[token_line]->operator, " ");
    }

    //밑은 ,이 있는지 확인하여 operand에 값을 넣는 코드
    int comma_contain_test;
    token_str = strtok(NULL, " \t\n");
    char** token_buf = (char**)malloc(sizeof(char*) * sizeof(token_str));
    if (token_buf == NULL) {
        printf(stderr, "메모리 할당 오류\n");
            return 1;
    }
    token_buf = _strdup(token_str);
    if (token_buf != NULL && strchr(token_buf, ',') != NULL) {
        comma_contain_test = 1;
    }
    else {
        comma_contain_test = 0;
    }
        
    if (comma_contain_test == 1) {
        //만약 문자열에 ,이 있다면 
        // strtok을 이용하여 ,로 토큰화
        char* comma_token = strtok(token_buf, ",");
        int i = 0;
        while (comma_token != NULL) {
            strcpy(token_table[token_line]->operand[i], comma_token);
            comma_token = strtok(NULL, ",");
            i++;
        }
        if (i < MAX_OPERAND) {
            for (i; i < MAX_OPERAND; i++) {
                token_table[token_line]->operand[i][0] = '\0';  // 빈 문자열로 초기화
            }
        }
    }
    else if (comma_contain_test == 0) {
        if (token_str != NULL) {
            strcpy(token_table[token_line]->operand[0], token_str);
            token_table[token_line]->operand[1][0] = '\0';
            token_table[token_line]->operand[2][0] = '\0';
        }
    }

    token_str = strtok(NULL, "\n");
    if (token_str != NULL) {
        strcpy(token_table[token_line]->comment, token_str);
    }
    else {
        strcpy(token_table[token_line]->comment, "");
    }


    //밑은 토큰화 테스트 코드임.
    
    printf("[LINE : %2d]\n", token_line);
    if (token_table[token_line]->label != NULL) {
        printf("label : %s\n", token_table[token_line]->label);
    }
    else {
        printf("\n");
    }
    printf("operator : %s\n", token_table[token_line]->operator);
    for (int print_i = 0; print_i < MAX_OPERAND; print_i++) {
        if (token_table[token_line]->operand[print_i][0] != '\0') {
            printf("Operand %d: %s\n", print_i + 1, token_table[token_line]->operand[print_i]);
        }
    }
    printf("Comment: %s\n", token_table[token_line]->comment);
    

    printf("\n\n");
    return 0;
}

static int assem_pass1(void) {
    make_opcode_output("optab.txt");
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


int countDigits(int number) {
    /*
        inst.data, input.txt파일을 불러와 테스트하는 과정에서 line의 가독성을 높이기 위해 사용된 테스트용 코드임.
        따라서 어셈블러 기능과는 전혀 무관함.
    */
    int count = 0;
    // 자릿수 계산을 위한 변수
    if (number == 0) {
        //만약 전달받은 변수가 0이면 그대로 자리가 0자릿수라고 리턴시켜줌
        return 1;
    }
    while (number != 0) {
        //자릿수 계산용 while문
        number = number / 10;
        //계속 자릿수를 나눠봄
        count++;
    }
    return count;
}
