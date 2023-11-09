#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include "my_assembler_20194318.h"

int init_my_assembler(void){

    if (init_inst_file("inst.data") != 0) {
        return -1;
    }

    if (init_input_file("input.txt") != 0) {
        return -1;
    }

    return 0; 
}

int init_inst_file(uchar *inst_file){
    FILE *file = fopen(inst_file, "r");
    inst_index = 0;
    if (file == NULL) {
        perror("Error opening inst.data file");
        return -1;
    }
    int i;
    for ( i = 0; i < MAX_INST; i++) {
        inst_table[i] = (inst *)malloc(sizeof(inst));
    }

    printf("Instructions from inst.data:\n");

    while (fscanf(file, "%s %d %d %X", inst_table[inst_index]->str, 
                                      &inst_table[inst_index]->ops, 
                                      &inst_table[inst_index]->format, 
                                      &inst_table[inst_index]->op) != EOF) {
        printf("%s %d %d %X\n", inst_table[inst_index]->str, 
                                 inst_table[inst_index]->ops, 
                                 inst_table[inst_index]->format, 
                                 inst_table[inst_index]->op);
        inst_index++;

        if (inst_index >= MAX_INST) {
            printf("Error: Maximum number of instructions reached.\n");
            break;
        }
    }

    fclose(file);
    return 0;
}

int init_input_file(uchar *input_file){
    FILE *file = fopen(input_file, "r");
    int line_num = 0;

    if (file == NULL) {
        perror("Error opening input.txt file");
        return -1;
    }
	int i;
    for ( i = 0; i < MAX_LINES; i++) {
        input_data[i] = (uchar *)malloc(MAX_LINES); 
    }

    while (fgets(input_data[line_num], MAX_LINES, file) != NULL) {
        printf("Line %d: %s", line_num + 1, input_data[line_num]);
        line_num++;

        if (line_num >= MAX_LINES) {
            printf("Error: Maximum number of lines reached.\n");
            break;
        }
    }

    printf("\n");
    fclose(file);
    return 0;
}

int token_parsing(uchar *str) {
    token_line = 0;  
    token_table[token_line] = (token *)malloc(sizeof(token));

    char *token_str = strtok(str, " \t\n");  

    while (token_str != NULL) {
        printf("Token: %s\n", token_str);

        if (token_str != NULL && token_str[0] != '.') {
            token_table[token_line]->label = strdup(token_str);
            printf("Label: %s\n", token_table[token_line]->label);
        } else {
            token_table[token_line]->label = NULL;
        }

        token_str = strtok(NULL, " \t\n");

        if (token_str != NULL) {
            token_table[token_line]->operator = strdup(token_str);
            printf("Operator: %s\n", token_table[token_line]->operator);
        } else {
            token_table[token_line]->operator = NULL;
        }
		int i;
        for (i = 0; i < MAX_OPERAND; i++) {
            token_str = strtok(NULL, " \t\n");

            if (token_str != NULL) {
                strcpy(token_table[token_line]->operand[i], token_str);
                printf("Operand %d: %s\n", i + 1, token_table[token_line]->operand[i]);
            } else {
                strcpy(token_table[token_line]->operand[i], "");
            }
        }

        token_str = strtok(NULL, "\n");

        if (token_str != NULL) {
            strcpy(token_table[token_line]->comment, token_str);
            printf("Comment: %s\n", token_table[token_line]->comment);
        } else {
            strcpy(token_table[token_line]->comment, "");
        }

        token_line++;
        token_table[token_line] = (token *)malloc(sizeof(token));  

        if (token_table[token_line] == NULL) {
            perror("Error allocating memory for token");
            exit(EXIT_FAILURE);
        }

        token_str = strtok(NULL, " \t\n");
    }

    printf("Reached the end of token_parsing\n");
    return 0;
}

int search_opcode(uchar *str) {
      uchar *opcode = token_table[token_line]->operator;  

    // inst_table에서 명령어 검색
    int i;
    for (i = 0; i < inst_index; i++) {
        if (strcmp(inst_table[i]->str, opcode) == 0) {
            printf("Opcode found: %s\n", opcode);
            return inst_table[i]->op;
        }
    }

    printf("Error: Opcode not found for %s\n", opcode);
    return -1;
}

static int assem_pass1(void) {
    make_opcode_output("optab.txt");
    make_symtab_output("symtab.txt");
}

void make_opcode_output(uchar *file_name) {
    FILE *optab_file = fopen(file_name, "w");
    if (optab_file == NULL) {
        perror("Error creating optab.txt file");
        return;
    }

    // input_data를 순회하면서 명령어를 찾아 optab 정보 생성
    int i;
    for (i = 0; i < line_num; i++) {
        uchar *line = input_data[i];

        // 명령어 토큰 추출
        printf("Before token_parsing: %s\n", line);
        token_parsing(line);
        uchar *mnemonic = token_table[token_line]->operator;

        printf("Token: %s\n", mnemonic);

        // 검색된 opcode 정보를 optab 파일에 쓰기
        int opcode = search_opcode(mnemonic);
        if (opcode != -1) {
            fprintf(optab_file, "%s %02X\n", mnemonic, opcode);
        }
    }

    fclose(optab_file);
    printf("%s file created successfully.\n", file_name);
}

void make_symtab_output(uchar *file_name) {
    FILE *symtab_file = fopen(file_name, "w");
    if (symtab_file == NULL) {
        perror("Error creating symtab.txt file");
        return;
    }

    // locctr 초기화
    locctr = 0;

    // input_data를 순회하면서 symtab 정보 생성
    int i;
    for (i = 0; i < line_num; i++) {
        uchar *line = input_data[i];

        // 명령어 토큰 추출
        printf("Before token_parsing: %s\n", line);
        token_parsing(line);
        uchar *mnemonic = token_table[token_line]->operator;

        printf("Token: %s\n", mnemonic);

        // label이 있는 경우 symtab에 추가하고 locctr 값 계산
        if (token_table[token_line]->label != NULL) {
            uchar *label = token_table[token_line]->label;
            fprintf(symtab_file, "%s %04X\n", label, locctr);
        }

        // 명령어의 형식에 따라 locctr 값 계산
        int opcode = search_opcode(mnemonic);
        if (opcode != -1) {
            inst *instruction = inst_table[opcode];
            locctr += instruction->format;
        }
    }

    fclose(symtab_file);
    printf("%s file created successfully.\n", file_name);
}

static int assem_pass2(void);
void make_objectcode_output(uchar *file_name);
int main(){
   init_my_assembler();
   assem_pass1();
}
