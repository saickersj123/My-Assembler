#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include "my_assembler_20194318.h"

int init_my_assembler(void){
    // inst.data 파일로 명령어 초기화
    if (init_inst_file("inst_mod.data") == -1) {
        fprintf(stderr, "Error: Instruction Initiation Failed\n");
        return -1;
    }

    // input.txt 파일로 소스 코드 초기화
    if (init_input_file("input.txt") == -1) {
        fprintf(stderr, "Error: Source Code Initiation Failed\n");
        return -1;
    }

    return 0;
}

int init_inst_file(uchar *inst_file){
     FILE *file = fopen(inst_file, "r");
    
    if (file == NULL) {
        perror("Error opening inst.data file");
        return -1;
    }

    char line[50];  // Assuming each line in inst.data is not longer than 30 characters

    while (fgets(line, sizeof(line), file) != NULL) {
        if (inst_index >= MAX_INST) {
            fprintf(stderr, "Error: Maximum number of instructions reached.\n");
            fclose(file);
            return -1;
        }

        inst_table[inst_index] = (inst *)malloc(sizeof(inst));

        // Assuming the format of each line in inst.data is "str ops format op"
        sscanf(line, "%s %d %d %02X", inst_table[inst_index]->str, &(inst_table[inst_index]->ops), &(inst_table[inst_index]->format), &(inst_table[inst_index]->op));

        // Test print statement
        printf("Instruction %d: %s %d %d %02X\n", inst_index + 1, inst_table[inst_index]->str, inst_table[inst_index]->ops, inst_table[inst_index]->format, inst_table[inst_index]->op);

        inst_index++;
    }

    fclose(file);
    return 0;
}

int init_input_file(uchar *input_file){
  FILE *file = fopen(input_file, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open input file '%s'\n", input_file);
        return -1; // Error opening file
    }

    char line[100]; // Assuming the maximum length of a line in input.txt is 100 characters

    while (fgets(line, sizeof(line), file) != NULL) {
        if (line_num >= MAX_LINES) {
            fprintf(stderr, "Error: Maximum line limit reached\n");
            fclose(file);
            return -1; // Maximum line limit reached
        }

        uchar *new_line = strdup(line);
        if (new_line == NULL) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            fclose(file);
            return -1; // Memory allocation failed
        }

        input_data[line_num++] = new_line;
        
        // Test print statement
        printf("Line %d: %s", line_num, new_line);

        
    }

    fclose(file);
    return 0; // Successful initialization
}

int token_parsing(uchar *str) {
       // 토큰화된 결과를 저장할 구조체 선언
    token *current_token = malloc(sizeof(token));
    if (current_token == NULL) {
        // 동적 메모리 할당 오류 처리
        fprintf(stderr, "Memory Allocation Error\n");
        exit(EXIT_FAILURE);
    }

    // 토큰 초기화
    current_token->label = NULL;
    current_token->operator = NULL;
    memset(current_token->operand, 0, sizeof(current_token->operand));
    memset(current_token->comment, 0, sizeof(current_token->comment));

    // strtok 함수를 이용하여 문자열을 토큰으로 분리
    char *token = strtok(str, " ,\t\n");

    // 토큰이 존재하는 동안 반복
    int token_count = 0;
    while (token != NULL && token != ".") {
        // 첫 번째 토큰은 라벨로 가정
        if (token_count == 0) {
            current_token->label = strdup(token);
        }
        // 두 번째 토큰은 연산자로 가정
        else if (token_count == 1) {
            current_token->operator = strdup(token);
        }
        // 세 번째부터는 피연산자로 가정 (최대 3개까지)
        else if (token_count <= 3) {
            strcpy(current_token->operand[token_count - 2], token);
        }
        // 그 외의 토큰은 주석으로 가정
        else {
            strcat(current_token->comment, token);
            strcat(current_token->comment, " ");
        }

        // 다음 토큰으로 이동
        token = strtok(NULL, " ,\t");
        token_count++;
    }

    // 구조체에 저장된 토큰 출력 (테스트용)
    printf("Label: %s\n", current_token->label);
    printf("Operator: %s\n", current_token->operator);
    printf("Operand 1: %s\n", current_token->operand[0]);
    printf("Operand 2: %s\n", current_token->operand[1]);
    printf("Operand 3: %s\n", current_token->operand[2]);
    printf("Comment: %s\n", current_token->comment);

    // 메모리 해제
    free(current_token->label);
    free(current_token->operator);
    free(current_token);

    return 0; // 성공적으로 토큰화됨을 나타내는 코드
}


int search_opcode(uchar *str) {
    for (int i = 0; i < inst_index; i++) {
        if (strcmp(inst_table[i]->str, str) == 0) {
            return i; // Return the index if found
        }
    }
    return -1; // Return -1 if not found
}

static int assem_pass1(void){
    // Initialize LOCCTR and other necessary variables
    locctr = starting_address;

    // Read the first input line
    uchar *current_line = input_data[0];

    // Check for START opcode
    if (strcmp(token_table[0]->operator, "START") == 0) {
        // Save #[OPERAND] as the starting address
        starting_address = atoi(token_table[0]->operand[0]);
        // Initialize LOCCTR to starting address
        locctr = starting_address;
        // Write the line to the intermediate file
        write_intermediate_file(current_line);
        // Read the next input line
        current_line = input_data[++line_num];
    } else {
        // If no START opcode, initialize LOCCTR to 0
        locctr = 0;
    }
   // Process lines until END opcode is encountered
    while (strcmp(token_table[line_num]->operator, "END") != 0) {
        // Check for comment line
        if (token_table[line_num]->operator[0] != '.') {
            // Check for symbol in the LABEL field
            if (token_table[line_num]->label[0] != '\0') {
                // Search SYMTAB for LABEL
                int symtab_index = search_symtab(token_table[line_num]->label);
                if (symtab_index != -1) {
                    // Set error flag (duplicate symbol)
                    printf("Error: Duplicate symbol - %s\n", token_table[line_num]->label);
                    // Handle the error as needed
                } else {
                    // Insert (LABEL, LOCCTR) into SYMTAB
                    strcpy(sym_table[inst_index].symbol, token_table[line_num]->label);
                    sym_table[inst_index].addr = locctr;
                    inst_index++; // Increment the index for the next entry
                }
            }

            // Search OPTAB for OPCODE
            int opcode_index = search_opcode(token_table[line_num]->operator);
            if (opcode_index != -1) {
                // Add 3 {instruction length} to LOCCTR
                locctr += 3;
            } else if (strcmp(token_table[line_num]->operator, "WORD") == 0) {
                // Add 3 to LOCCTR
                locctr += 3;
            } else if (strcmp(token_table[line_num]->operator, "RESW") == 0) {
                // Add 3 * #[OPERAND] to LOCCTR
                locctr += 3 * atoi(token_table[line_num]->operand[0]);
            } else if (strcmp(token_table[line_num]->operator, "RESB") == 0) {
                // Add #[OPERAND] to LOCCTR
                locctr += atoi(token_table[line_num]->operand[0]);
            } else if (strcmp(token_table[line_num]->operator, "BYTE") == 0) {
                // Find length of constant in bytes
                // Update LOCCTR accordingly
                int length = 0; // Implement the logic to determine the length
                locctr += length;
            } else {
                // Set error flag (invalid operation code)
                printf("Error: Invalid operation code - %s\n", token_table[line_num]->operator);
                // Handle the error as needed
            }

            // Write the line to the intermediate file
            write_intermediate_file(current_line);
        }

        // Read the next input line
        current_line = input_data[++line_num];
    }

    // Write the last line to the intermediate file
    write_intermediate_file(current_line);

    // Save (LOCCTR - starting address) as program length
    program_length = locctr - starting_address;

    return 0; // Success
}

void make_opcode_output(uchar *file_name) {
    FILE *output_file = fopen(file_name, "w");

    if (output_file == NULL) {
        printf("Error opening output file.\n");
        return;
    }

    // Output header
    fprintf(output_file, "Mnemonic   MachineCode   Format   Length\n");

    // Iterate through the inst_table to print information
    for (int i = 0; i < inst_index; i++) {
        fprintf(output_file, "%-10s %-14X %-8d %-6d\n",
                inst_table[i]->str, inst_table[i]->op, inst_table[i]->format, inst_table[i]->ops);
    }

    fclose(output_file);
}


void make_symtab_output(uchar *file_name) {
    FILE *output_file = fopen(file_name, "w");

    if (output_file == NULL) {
        printf("Error opening output file.\n");
        return;
    }

    // Output header
    fprintf(output_file, "Label   Address   Type/Length \n");

    // Iterate through the sym_table to print information
    for (int i = 0; i < line_num; i++) {
        fprintf(output_file, "%-7s %-9X %-12s\n",
                sym_table[i].symbol, sym_table[i].addr, "N/A");
        // Note: You need to modify the output based on the actual structure and content of sym_table.
        // This example assumes some placeholder values.
    }

    fclose(output_file);
}

static int assem_pass2(void);
void make_objectcode_output(uchar *file_name);
int main(){
   init_my_assembler();
   make_opcode_output("optab.txt");
}
