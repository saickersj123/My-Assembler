#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include <ctype.h>
#include "my_assembler_20194318.h"


void add_to_symtab(const uchar *label, int loc, int is_equ) {
    // Check for duplicate symbol
    for (int i = 0; i < MAX_LINES; i++) {
        if (strcmp(sym_table[i].symbol, label) == 0) {
            if (is_equ) {
                fprintf(stderr, "Error: Duplicate symbol found - %s\n", label);
                // Handle the error as needed
                return;
            }
            // If not an EQU, it might be a label for a future address
            // We don't want to report it as an error in this case
            return;
        }
    }

    // Find an empty slot in the symbol table
    int index;
    for (index = 0; index < MAX_LINES; index++) {
        if (sym_table[index].symbol[0] == '\0') {
            break;  // Found an empty slot
        }
    }

    // Check if the symbol table is full
    if (index == MAX_LINES) {
        fprintf(stderr, "Error: Symbol table is full.\n");
        // Handle the error as needed
        return;
    }

    // Add the symbol to the symbol table
    strcpy(sym_table[index].symbol, label);
    sym_table[index].addr = loc;
    printf("Adding symbol to symtab: %s at address %d\n", label, loc);
}


int calculate_byte_length(uchar *operand) {
    if (operand[0] == 'C' || operand[0] == 'c') {
        // Count the characters between single quotes
        int length = 0;
        for (int i = 2; operand[i] != '\''; ++i) {
            length++;
        }
        return length;
    } else if (operand[0] == 'X' || operand[0] == 'x') {
        // Count the characters between single quotes
        // Each hexadecimal digit requires 4 bits, so divide the length by 2
        int length = 0;
        for (int i = 2; operand[i] != '\''; ++i) {
            length++;
        }
        return (length + 1) / 2; // Adjust for hexadecimal format
    } else {
        // Handle other formats as needed
        // This is a placeholder implementation
        return 0;
    }
}

void calculate_program_length(void) {
    program_length = locctr - starting_address;
}

int search_symtab(uchar *symbol) {
    for (int i = 0; i < token_line; ++i) {
        if (strcmp(sym_table[i].symbol, symbol) == 0) {
            return i; // Symbol found, return its index
        }
    }
    return -1; // Symbol not found
}

int init_my_assembler(void){
    // inst.data 파일로 명령어 초기화
    if (init_inst_file("inst.data") == -1) {
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

    char line[100];  // Assuming each line in inst.data is not longer than 30 characters

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
        FILE *input_fp = fopen(input_file, "r");
    if (input_fp == NULL) {
        fprintf(stderr, "Error opening input file: %s\n", input_file);
        return 1;
    }

    char line[100];
    int line_index = 0;

    while (fgets(line, sizeof(line), input_fp) != NULL) {

        // 개행 문자 제거
        line[strcspn(line, "\n")] = '\0';
       // Check if the line starts with a dot (comment)
        if (line[0] != '.') {
            // 입력 데이터 배열에 저장
            input_data[line_index] = strdup(line);

            // 다음 라인으로 이동
            line_index++;
        }
    }

    // 라인 수 저장
    line_num = line_index;

     printf("Input Data:\n");
    for (int i = 0; i < line_num; i++) {
        printf("%d: %s\n", i + 1, input_data[i]);
    }
    printf("Total Lines: %d\n", line_num);

    fclose(input_fp);
    return 0;
}

int token_parsing(uchar *str) {
    // 초기화
    token_table[token_line] = malloc(sizeof(token));
    if (token_table[token_line] == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return -1;
    }
    token_table[token_line]->label = NULL;
    token_table[token_line]->operator = NULL;
    for (int i = 0; i < MAX_OPERAND; i++) {
    token_table[token_line]->operand[i][0] = '\0';
    }
    //token_table[token_line]->comment[0] = '\0';

    // 빈 줄이거나 주석인 경우 처리
   if (str[0] == '\0' || str[0] == '\n' || str[0] == '.') {
        return 0;
    }
    uchar *scopy = strdup((char *) str);

    // 문자열을 공백을 기준으로 토큰화
    uchar *token_str = strtok(scopy, " \t");

    // 첫 번째 토큰이 operator인지 확인
    int is_operator = search_opcode(token_str);

    if (is_operator == 0) {
        // 첫 번째 토큰이 operator인 경우
        //token_table[token_line]->label = strdup("\0");
        token_table[token_line]->operator = strdup(token_str);
        token_str = strtok(NULL, " \t"); // 오퍼레이터 파싱 건너뛰기
    } else if(is_operator == 1){
        // 첫 번째 토큰이 operator가 아닌 경우 label로 처리
        token_table[token_line]->label = strdup(token_str);
        token_str = strtok(NULL, " \t\n"); // 다음 토큰으로 이동
        // 오퍼레이터 파싱
        if (token_str != NULL) {
        token_table[token_line]->operator = strdup(token_str);
        token_str = strtok(NULL, " \t\n"); // 다음 토큰으로 이동
        } else {
        fprintf(stderr, "Error: Operator missing.\n");
        return -1;
        }
    }


       // 피연산자 파싱
    int operand_index = 0;
    while (token_str != NULL) {
        if (operand_index < MAX_OPERAND) {
            // 여러 피연산자가 ','로 구분된 경우 처리
            if (strchr(token_str, ',')) {
                char *operand_token = strtok(token_str, ",");
                while (operand_token != NULL) {                   
                    strcpy((char *)token_table[token_line]->operand[operand_index], operand_token);
                    operand_token = strtok(NULL, ",\t");
                    operand_index++;
                }
            } else {
                // ','로 구분되지 않은 경우 피연산자 처리 후 종료
                strcpy((char *)token_table[token_line]->operand[operand_index], token_str);
                operand_index++;
                token_str = strtok(NULL, "\t");
                break;
            }
        } else {
            fprintf(stderr, "Error: Too many operands.\n");
            free(scopy);
            return -1;
        }
        token_str = strtok(NULL, " \t\n"); // 다음 토큰으로 이동
    }
    

    /* 피연산자 이후의 문자열은 주석으로 처리
    if (token_str != NULL) {
        strcpy(token_table[token_line]->comment, token_str);
    }
    else {
        strcpy(token_table[token_line]->comment, "");
    }*/
    
    free(scopy);

    // 테스트 출력문
    printf("Line %d\n", token_line + 1);
    printf("Label: %s\n", token_table[token_line]->label);
    printf("Operator: %s\n", token_table[token_line]->operator);
    printf("Operands: \n");
    for (int i = 0; i < operand_index; i++) {
        printf("  %s\n", token_table[token_line]->operand[i]);
    }
    //printf("Comment: %s\n", token_table[token_line]->comment);

    return 0;
}

int search_opcode(uchar *str) {
        if (str == NULL || str[0] == '\0' || str[0] == '.') {
        // Skip comments and empty lines
        return -1;
    }
    
    for (int i = 0; i < inst_index; i++) {
        if (strcmp(str, inst_table[i]->str) == 0 || (str[0] == '+' && strcmp(str + 1, inst_table[i]->str) == 0)) {
            // 검색된 명령어 정보 출력 (예시로 콘솔에 출력)
            printf("Operator: %s / ", inst_table[i]->str);
            printf("Format: %d / ", inst_table[i]->format);
            printf("Opcode Value: 0x%02X\n", inst_table[i]->op);
            return 0;  // 검색 성공
        }
    }

    // 검색 실패
    fprintf(stderr, "Error: Opcode not found for %s\n", str);
    return 1;
}

// Function to handle EXTDEF directive
void handle_extdef(uchar *symbol, int addr) {
    if (extDefCount < MAX_OPERAND) {
        strcpy(extDef[extDefCount].symbol, symbol);
        extDef[extDefCount].addr = addr;
        extDefCount++;
    } else {
        fprintf(stderr, "Error: Too many entries in EXTDEF directive.\n");
        // Handle the error as needed
    }
}

// Function to handle EXTREF directive
void handle_extref(uchar *symbol) {
    if (extRefCount < MAX_OPERAND) {
        strcpy(extRef[extRefCount].symbol, symbol);
        extRefCount++;
    } else {
        fprintf(stderr, "Error: Too many entries in EXTREF directive.\n");
        // Handle the error as needed
    }
}

// Function to handle EQU directive
void handle_equ_directive(uchar *label, uchar *operand) {
    int equ_value = evaluate_expression(operand);
    if (equ_value != -1) {
        add_to_symtab(label, equ_value, 1); // The third argument (is_equ) is set to 1
    }
}

static int assem_pass1(void) {
    // Read the first input line
    uchar *current_line = input_data[0];
    token_parsing(current_line);

    // Check for START directive
    if (strcmp(token_table[0]->operator, "START") == 0) {
        // Save #[OPERAND] as starting address
        starting_address = strtol(token_table[0]->operand[0], NULL, 16);

        // Initialize LOCCTR to starting address
        locctr = starting_address;

        // Write line to intermediate file
        write_intermediate_file(current_line, locctr);
        token_line++;
    } else {
        // If no START directive, initialize LOCCTR to 0
        locctr = 0;
    }

    // Process lines until OPCODE is 'END'
    while (token_line < MAX_LINES) {
        current_line = input_data[token_line];
        token_parsing(current_line);

        if (token_table[token_line] != NULL) {
            // Check for EQU directive
            if (strcmp(token_table[token_line]->operator, "EQU") == 0) {
                handle_equ_directive(token_table[token_line]->label, token_table[token_line]->operand[0]);
            } 

            // Check for EXTDEF and EXTREF directives
            else if (strcmp(token_table[token_line]->operator, "EXTDEF") == 0) {
                // Process EXTDEF directive
                for (int i = 0; i < MAX_OPERAND && token_table[token_line]->operand[i][0] != '\0'; ++i) {
                    handle_extdef(token_table[token_line]->operand[i], locctr);
                }
            } else if (strcmp(token_table[token_line]->operator, "EXTREF") == 0) {
                // Process EXTREF directive
                for (int i = 0; i < MAX_OPERAND && token_table[token_line]->operand[i][0] != '\0'; ++i) {
                    handle_extref(token_table[token_line]->operand[i]);
                }
            } else {
                int opcode_index = search_opcode(token_table[token_line]->operator);

                if (opcode_index != -1) {
                    // Check for 4-format instruction
                    if (token_table[token_line]->operator[0] == '+') {
                        locctr += 4;
                    } else {
                        locctr += 3;
                    }
                } else if (strcmp(token_table[token_line]->operator, "WORD") == 0) {
                    locctr += 3;
                } else if (strcmp(token_table[token_line]->operator, "RESW") == 0) {
                    locctr += 3 * atoi(token_table[token_line]->operand[0]);
                } else if (strcmp(token_table[token_line]->operator, "RESB") == 0) {
                    locctr += atoi(token_table[token_line]->operand[0]);
                } else if (strcmp(token_table[token_line]->operator, "BYTE") == 0) {
                    locctr += calculate_byte_length(token_table[token_line]->operand[0]);
                } else {
                    fprintf(stderr, "Error: Invalid operation code - %s\n", token_table[token_line]->operator);
                    return -1;
                }
            }

            // Write line to intermediate file
            write_intermediate_file(current_line, locctr);

            // Check for the "END" directive to exit the loop
            if (strcmp(token_table[token_line]->operator, "END") == 0) {
                break;
            }
        }

        token_line++;
    }

    int program_length = locctr - starting_address;
    make_symtab_output("symtab.txt");

    return 0;
}



void make_opcode_output(uchar *file_name){
    FILE *output_file = fopen(file_name, "w");

    if (output_file == NULL) {
        printf("Error opening output file.\n");
        return;
    }

    // Output header
    fprintf(output_file, "Mnemonic   MachineCode   Format   Length\n");

    // Iterate through the inst_table to print information
    for (int i = 0; i < inst_index; i++) {
        fprintf(output_file, "%-10s %02X %-8d %-6d\n",
                inst_table[i]->str, inst_table[i]->op, inst_table[i]->format, inst_table[i]->ops);
    }

    fclose(output_file);
}


void make_symtab_output(uchar *file_name) {
    FILE *symtab_output_file = fopen(file_name, "w");

    if (symtab_output_file == NULL) {
        fprintf(stderr, "Error opening %s for writing.\n", file_name);
        exit(EXIT_FAILURE);
    }

    fprintf(symtab_output_file, "Symbol\tAddress\n");
    fprintf(symtab_output_file, "----------------\n");

    for (int i = 0; i < MAX_LINES; i++) {
        if (sym_table[i].addr != -1) {
            fprintf(symtab_output_file, "%s\t%04X\n", sym_table[i].symbol, sym_table[i].addr);
        }
    }

    fclose(symtab_output_file);
}


static void insert_into_symtab(uchar *label, int locctr){
    int index = search_symtab(label);

    if (index == -1) {
        // Symbol not found, insert into the symbol table
        strcpy(sym_table[token_line].symbol, label);
        sym_table[token_line].addr = locctr;
        token_line++;
    } else {
        // Symbol found, handle the duplicate symbol error
        printf("Error: Duplicate symbol '%s' found.\n", label);
    }
}

void write_intermediate_file(uchar *str, int locctr){
    FILE *intermediate_file = fopen("intermediate.txt", "a");
    if (intermediate_file == NULL) {
        fprintf(stderr, "Error: Unable to open intermediate file for writing.\n");
        return;
    }
    // 현재 라인을 중간 파일에 주어진 형식에 맞게 쓰기
    fprintf(intermediate_file, "%04X %s\n", locctr, str);
    if (intermediate_file != NULL) {
        fclose(intermediate_file);
    }
 }

int is_digit(char c) {
    return c >= '0' && c <= '9';
}

// Function to evaluate expressions in EQU directive
int evaluate_expression(uchar *expr) {
    int result = 0;
    int current_value = 0;
    int sign = 1; // 1 for positive, -1 for negative

    for (int i = 0; expr[i] != '\0'; ++i) {
        if (is_digit(expr[i])) {
            current_value = current_value * 10 + (expr[i] - '0');
        } else if (expr[i] == '-') {
            sign = -1;
        } else if (expr[i] == '+') {
            sign = 1;
        } else if (expr[i] == '*') {
            // Handle '*' as a reference to the current location counter (locctr)
            result += sign * locctr;
            // Reset current_value for the next numeric part of the expression
            current_value = 0;
            // Reset sign to default (positive)
            sign = 1;
        } else if (expr[i] == ' ' || expr[i] == '\t') {
            // Skip whitespace
        } else {
            // Handle symbols (assuming the symbol is a single character for simplicity)
            char symbol[2] = {expr[i], '\0'};
            int sym_value = search_symtab(symbol);

            if (sym_value != -1) {
                // Symbol found, add its value to the result
                result += sign * sym_value;
            } else {
                fprintf(stderr, "Error: Undefined symbol - %s\n", symbol);
                return -1; // or handle the error as needed
            }

            // Reset current_value for the next numeric part of the expression
            current_value = 0;
            // Reset sign to default (positive)
            sign = 1;
        }
    }

    // Add the last numeric part of the expression
    result += sign * current_value;

    return result;
}