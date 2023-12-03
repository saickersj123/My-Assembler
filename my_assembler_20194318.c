#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include <ctype.h>
#include "my_assembler_20194318.h"


void add_to_symtab(const uchar *label, int loc, int is_equ) {
    // Check for duplicate symbol
    for (int i = 0; i < sym_index; i++) {
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

    for (int index = 0; index < MAX_LINES; index++) {
        if (sym_table[index].symbol[0] == '\0') {
            break;  // Found an empty slot
        }
    }

    // Check if the symbol table is full
    if (sym_index == MAX_LINES) {
        fprintf(stderr, "Error: Symbol table is full.\n");
        // Handle the error as needed
        return;
    }

    // Add the symbol to the symbol table
    strcpy(sym_table[sym_index].symbol, label);
    sym_table[sym_index].addr = loc;
    printf("Adding symbol to symtab: %s at address %04X\n", label, loc);
    sym_index++;
}

int search_literal(uchar *operand) {
    for (int i = 0; i < LT_num; ++i) {
        if (strcmp(LTtab[i].name, operand) == 0) {
            // Literal found, return its index
            return i;
        }
    }
    // Literal not found
    return -1;
}

int calculate_byte_length(uchar *operand) {
        if (operand[0] == '=') {
        // Literal processing
        int literal_index = search_literal(operand);
        if (literal_index != -1) {
            return LTtab[literal_index].leng;
        } else {
            // Literal not found, handle error
            fprintf(stderr, "Error: Undefined literal - %s\n", operand);
            return -1;
        }

    }else if (operand[0] == 'C' || operand[0] == 'c') {
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
    } 
}

void calculate_program_length(void) {
    program_length = locctr - starting_address;
}

int search_symtab(uchar *symbol) {
    for (int i = 0; i < sym_index; ++i) {
        if (strcmp(sym_table[i].symbol, symbol) == 0) {
            return  sym_table[i].addr; // Symbol found, return its index
        }
    }
    return -1; // Symbol not found
}

int init_my_assembler(void){
    // Initialize inst.data 
    if (init_inst_file("inst.data") == -1) {
        fprintf(stderr, "Error: Instruction Initiation Failed\n");
        return -1;
    }

    // Initialize input.txt
    if (init_input_file("input.txt") == -1) {
        fprintf(stderr, "Error: Source Code Initiation Failed\n");
        return -1;
    }
    
    make_opcode_output("optab.txt");
 

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
        sscanf(line, "%s %d %02X", inst_table[inst_index]->str, &(inst_table[inst_index]->format), &(inst_table[inst_index]->op));

        // Test print statement
        printf("Instruction %d: Mnemonic %s  Format %d  OP %02X\n", inst_index + 1, inst_table[inst_index]->str, inst_table[inst_index]->format, inst_table[inst_index]->op);

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

        // remove \n
        line[strcspn(line, "\n")] = '\0';
       // Check if the line starts with a dot (comment)
        if (line[0] != '.') {
            //Save the input into input_data
            input_data[line_index] = strdup(line);

            //Next line
            line_index++;
        }
    }

    //Save line number
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
    // Initialize
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

    // Check comments and empty Line 
   if (str[0] == '\0' || str[0] == '\n' || str[0] == '.') {
        return 0;
    }
    uchar *scopy = strdup((char *) str);

    // Tokenize with \t
    uchar *token_str = strtok(scopy, " \t");

    //Check first token is operator
    int is_operator = tok_search_opcode(token_str);

    if (is_operator == 0) {
        // If the first token is operator
        //token_table[token_line]->label = strdup("\0");
        token_table[token_line]->operator = strdup(token_str);
        token_str = strtok(NULL, " \t"); // Skip parsing operator
    } else if(is_operator == 1){
        // if is not first token is label
        token_table[token_line]->label = strdup(token_str);
        token_str = strtok(NULL, " \t\n"); // Next token
        // Parsing Operator
        if (token_str != NULL) {
        token_table[token_line]->operator = strdup(token_str);
        token_str = strtok(NULL, " \t\n"); // Next token
        } else {
        fprintf(stderr, "Error: Operator missing.\n");
        return -1;
        }
    }


       //Parsing operands
    int operand_index = 0;
    while (token_str != NULL) {
        if (operand_index < MAX_OPERAND) {
            // Check if there are multiple operands
            if (strchr(token_str, ',')) {
                char *operand_token = strtok(token_str, ",");
                while (operand_token != NULL) {                   
                    strcpy((char *)token_table[token_line]->operand[operand_index], operand_token);
                    operand_token = strtok(NULL, ",\t");
                    operand_index++;
                }
            } else {
                // if is not handle the token as operand
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
    

    /* //Rest of the token is comment
    if (token_str != NULL) {
        strcpy(token_table[token_line]->comment, token_str);
    }
    else {
        strcpy(token_table[token_line]->comment, "");
    }*/
    
    free(scopy);

    // Test output
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
            // Test print searched operator on terminal
            printf("Operator: %s / ", inst_table[i]->str);
            printf("Format: %d / ", inst_table[i]->format);
            printf("Opcode Value: 0x%02X\n", inst_table[i]->op);
            return i;  // 검색 성공
        }
    }

    // Search Faild
    fprintf(stderr, "Error: Opcode not found for %s\n", str);
    return -1;
}

int tok_search_opcode(uchar *str) {
        if (str == NULL || str[0] == '\0' || str[0] == '.') {
        // Skip comments and empty lines
        return -1;
    }
    
    for (int i = 0; i < inst_index; i++) {
        if (strcmp(str, inst_table[i]->str) == 0 || (str[0] == '+' && strcmp(str + 1, inst_table[i]->str) == 0)) {
            // Test print searched operator on terminal
            printf("Operator: %s is ON OPTAB\n", inst_table[i]->str);
            //printf("Format: %d / ", inst_table[i]->format);
            //printf("Opcode Value: 0x%02X\n", inst_table[i]->op);
            return 0;  // Successfully searched
        }
    }

    // Search Failed
    fprintf(stderr, "Error: Opcode not found for %s\n", str);
    return 1;
}

// Function to handle EXTDEF directive
void handle_extdef(uchar *symbol, int addr) {
        strcpy(extDef[extDefCount].symbol, symbol);
        extDef[extDefCount].addr = addr;
}

// Function to handle EXTREF directive
void handle_extref(uchar *symbol) {
        strcpy(extRef[extRefCount].symbol, symbol);
        extRefCount++;
}

// Function to handle EQU directive
void handle_equ_directive(uchar *label, uchar *operand) {
     if (strcmp(operand, "*") == 0) {
        // Set the address of the label to the current LOCCTR
        add_to_symtab(label, locctr, 0);
    } else {
    int equ_value = evaluate_expression(operand);
        if (equ_value != -1) {
        add_to_symtab(label, equ_value, 1); // The third argument (is_equ) is set to 1
        }
    }
}

// Function to handle LTORG directive
void handle_ltorg_directive(void) {
   int literal_length = 0;

    // Iterate through the literal table
    for (int i = 0; i < LT_num; i++) {
        // Check if the literal has been assigned an address
        if (LTtab[i].addr == -1) {
            // If not assigned, assign the next available address
            LTtab[i].addr = locctr + literal_length;
            literal_length += LTtab[i].leng;

            // Update the literal address in the intermediate file
            write_intermediate_file("", LTtab[i].addr);
        }
    }

    // Update the location counter with the total length of literals
    locctr += literal_length;
}

static int assem_pass1(void) {
    locctr = 0;
    starting_address = 0;
    program_length = 0;
    token_line = 0;
    sym_index = 0;

    // Initialize the symbol table
    for (int i = 0; i < MAX_LINES; ++i) {
        sym_table[i].sec = -1;
        sym_table[i].addr = -1;
        sym_table[i].symbol[0] = '\0';
    }


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

        /*// Add the label to the symbol table
        if (token_table[0]->label != NULL) {
            add_to_symtab(token_table[0]->label, locctr, 0);
        }*/
        
        token_line++;

    } else {
        // If no START directive, initialize LOCCTR to 0
        locctr = 0;
        write_intermediate_file(current_line, locctr);
    }

    // Process lines until OPCODE is 'END'
    while (token_line < MAX_LINES) {
        current_line = input_data[token_line];
        token_parsing(current_line);
        if (token_table[token_line] != NULL) {

            // Check for CSECT directive
            if (strcmp(token_table[token_line]->operator, "CSECT") == 0) {
                // Start a new section, reset the program counter
                locctr = strtol(token_table[token_line]->operand[0], NULL, 16);
                // Add the section name to the symbol table
                if (token_table[token_line]->label != NULL) {
                    add_to_symtab(token_table[token_line]->label, locctr, 0);
                }
                // Write line to intermediate file
                write_intermediate_file(current_line, locctr);
                token_line++;
                continue;  // Skip the rest of the loop for CSECT
            }

            // Add the label to the symbol table
            if (token_table[token_line]->label != NULL) {
                add_to_symtab(token_table[token_line]->label, locctr, 0);
            }

            // Check for EQU directive
            if (strcmp(token_table[token_line]->operator, "EQU") == 0) {
                handle_equ_directive(token_table[token_line]->label, token_table[token_line]->operand[0]);
                // Update locctr with the evaluated value from EQU directive
                locctr = sym_table[sym_index - 1].addr;  // Update locctr to the EQU value
                   // Write line to intermediate file after updating locctr
            write_intermediate_file(current_line, locctr);
            } else {
            // Write line to intermediate file for other directives and instructions
            write_intermediate_file(current_line, locctr);
             }
             

            // Check for EXTDEF and EXTREF directives
            if (strcmp(token_table[token_line]->operator, "EXTDEF") == 0) {
                // Process EXTDEF directive
                for (int i = 0; i < MAX_OPERAND && token_table[token_line]->operand[i][0] != '\0'; ++i) {
                    handle_extdef(token_table[token_line]->operand[i], locctr);
                }
            } 
             else {
                int opcode_index = search_opcode(token_table[token_line]->operator);

                if (opcode_index != -1) {

                    // Check for 4-format instruction
                    if (token_table[token_line]->operator[0] == '+') {
                        locctr += 4; 
                    } else if (inst_table[opcode_index]->format > 0) {
                        locctr += inst_table[opcode_index]->format;
                    }
                    
                } if (strcmp(token_table[token_line]->operator, "WORD") == 0) {
                    locctr += 3;
                } if (strcmp(token_table[token_line]->operator, "RESW") == 0) {
                    locctr += 3 * atoi(token_table[token_line]->operand[0]);
                } if (strcmp(token_table[token_line]->operator, "RESB") == 0) {
                    locctr += atoi(token_table[token_line]->operand[0]);
                } if (strcmp(token_table[token_line]->operator, "BYTE") == 0) {
                    locctr += calculate_byte_length(token_table[token_line]->operand[0]);
                } // Check for LTORG directive
                 if (strcmp(token_table[token_line]->operator, "LTORG") == 0) {
                handle_ltorg_directive();
                } if (strcmp(token_table[token_line]->operator, "EXTREF") == 0) {
                // Process EXTREF directive
                for (int i = 0; i < MAX_OPERAND && token_table[token_line]->operand[i][0] != '\0'; ++i) {
                    handle_extref(token_table[token_line]->operand[i]);
                    } 
                } 
            }

             // Check for literals and add them to the literal table
            for (int i = 0; i < MAX_OPERAND; ++i) {
                uchar *operand = token_table[token_line]->operand[i];
                if (operand[0] == '=' && (operand[1] == 'C' || operand[1] == 'X')) {
                    // Found a literal
                    int length = calculate_byte_length(operand);
                 
                        // Add the literal to the literal table
                        LTtab[LT_num].leng = length;
                        strcpy(LTtab[LT_num].name, operand);
                        LTtab[LT_num].addr = -1;  // Not assigned an address yet
                        LT_num++;
                }
            }
            

            // Check for the "END" directive to exit the loop
            if (strcmp(token_table[token_line]->operator, "END") == 0) {
                handle_ltorg_directive();
                break;
            }
            


        }
        //Read Next
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
    fprintf(output_file, "Mnemonic  MachineCode Format\n");

    // Iterate through the inst_table to print information
    for (int i = 0; i < inst_index; i++) {
        fprintf(output_file, "%-10s %02X %10d\n",
                inst_table[i]->str, inst_table[i]->op, inst_table[i]->format);
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

    for (int i = 0; i < sym_index; i++) {
        if (sym_table[i].addr != -1) {
            fprintf(symtab_output_file, "%s\t%04X\n", sym_table[i].symbol, sym_table[i].addr);
        }
    }

    fprintf(symtab_output_file, "\nLiteral\tAddress\tPool Num\n");
    fprintf(symtab_output_file, "--------------------------\n");
    for (int i = 0; i < LT_num; i++) {
        if (LTtab[i].addr != -1) {
            fprintf(symtab_output_file, "%s\t%04X\t%d\n", LTtab[i].name, LTtab[i].addr, i);
        }
    }
    fclose(symtab_output_file);
}

void write_intermediate_file(uchar *str, int locctr){
    FILE *intermediate_file = fopen("intermediate.txt", "a");
    if (intermediate_file == NULL) {
        fprintf(stderr, "Error: Unable to open intermediate file for writing.\n");
        return;
    }
    // write locctr and inputs
    fprintf(intermediate_file, "%04X %s\n", locctr, str);
    if (intermediate_file != NULL) {
        fclose(intermediate_file);
    }
 }

int is_digit(char c) {
    return c >= '0' && c <= '9';
}

int evaluate_expression(uchar *expr) {
    // Tokenize the expression based on operators (+, -, *, /)
    char *token = strtok(expr, "+-*/");
    int result = 0;

    while (token != NULL) {
        // If the token is '*', replace it with the current location counter
        if (strcmp(token, "*") == 0) {
            result = locctr;
        } else {
            // If the token is a symbol, get its value
            int value;
            if (isalpha(token[0])) {
                value = search_symtab(token);
                if (value == -1) {
                    // Symbol not found, handle error
                    fprintf(stderr, "Error: Undefined symbol - %s\n", token);
                    return -1;
                }
            } else {
                // If the token is a constant, convert it to an integer
                value = atoi(token);
            }

            // Determine the operator and perform the operation
            char *op = strpbrk(expr, "+-*/");
            if (op != NULL) {
                switch (*op) {
                    case '+':
                        result += value;
                        break;
                    case '-':
                        result -= value;
                        break;
                    case '*':
                        result *= value;
                        break;
                    case '/':
                        if (value != 0) {
                            result /= value;
                        } else {
                            // Division by zero, handle error
                            fprintf(stderr, "Error: Division by zero\n");
                            return -1;
                        }
                        break;
                }
            } else {
                // If there are no more operators, the current value is the final result
                result = value;
            }
        }

        // Move to the next token
        token = strtok(NULL, "+-*/");
    }

    return result;
}


void make_objectcode_output(uchar *file_name, uchar *list_name) {
          // Open the object code output file
    object_code_file = fopen(file_name, "w");
    if (object_code_file == NULL) {
        fprintf(stderr, "Error: Unable to open object code output file for writing.\n");
        return;
    }

     // Open the assembly listing file
    listing_file = fopen(list_name, "w");
    if (listing_file == NULL) {
        fprintf(stderr, "Error: Unable to open assembly listing file for writing.\n");
        fclose(object_code_file);
        return; 
    }
}

// Function to generate object code based on the instruction format
void generate_object_code(int is_extended, int opcode_index, int locctr) {
    int object_code;
    int format = inst_table[opcode_index]->format;

    // Check for format 1 instruction
    if (format == 1) {
        // Format 1: opcode only
        object_code = inst_table[opcode_index]->op;
    }
    // Check for format 2 instruction
    else if (format == 2) {
        // Format 2: opcode + registers
        int reg1 = atoi(token_table[token_line]->operand[0]);
        int reg2 = atoi(token_table[token_line]->operand[1]);
        object_code = (inst_table[opcode_index]->op << 8) + (reg1 << 4) + reg2;
    }
    // Check for format 3/4 instruction
    else {
        // Format 3/4: opcode + (nixbpe) + displacement
        int nixbpe = 0;

        // Set 'n' bit
        if (is_extended || token_table[token_line]->operand[0][0] == '#') {
            nixbpe |= 1 << 5;
        }

        // Set 'i' bit
        if (token_table[token_line]->operand[0][0] != '#') {
            nixbpe |= 1 << 4;
        }

        // Set 'x' bit
        if (token_table[token_line]->operand[1] != NULL && strcmp(token_table[token_line]->operand[1], ",X") == 0) {
            nixbpe |= 1 << 3;
        }

        int disp;
        if (is_extended) {
            // Format 4: 20-bit address in operand field
            nixbpe |= 1 << 1;  // Set 'b' bit
            nixbpe |= 1;       // Set 'p' bit

            // Calculate the 20-bit address
            disp = strtol(token_table[token_line]->operand[0] + 1, NULL, 16);
        } else {
            // Format 3: 12-bit displacement from base or PC
            int target_address = search_symtab(token_table[token_line]->operand[0]);

            // Check if base relative addressing is possible
            if (target_address != -1 && target_address >= locctr - 2048 && target_address <= locctr + 2047) {
                disp = target_address - locctr;
                nixbpe |= 1 << 1;  // Set 'b' bit
            } else {
                // Use PC relative addressing
                disp = target_address - (locctr + 3);
                nixbpe |= 1;  // Set 'p' bit
            }
        }

        // Combine opcode, nixbpe, and displacement to get the object code
        object_code = (inst_table[opcode_index]->op << 16) + (nixbpe << 12) + (disp & 0xFFF);
    }

    // Store the object code in the array
    sprintf(obj_codes[obj_code_count].code, "%06X", object_code);
    obj_codes[obj_code_count].address = locctr;
    obj_code_count++;
}

// Helper function to write a Text record to the object program
void write_text_record(FILE *output_file, int start_address, int length, uchar text_record[MAX_LINES][10]) {
    fprintf(output_file, "T%06X%02X", start_address, length / 2);
    for (int i = 0; i < length / 2; ++i) {
        fprintf(output_file, "%s", text_record[i]);
    }
    fprintf(output_file, "\n");
}

// Helper function to convert constant to object code
void convert_constant_to_object_code(uchar *constant, uchar *object_code) {
    // Extract the content between single quotes and convert accordingly

    // Initialize object_code
    object_code[0] = '\0';

    if (constant[1] == 'C') {
        // Character constant, convert each character to ASCII and append to object_code
        for (int i = 3; constant[i] != '\''; ++i) {
            sprintf(object_code, "%s%02X", object_code, constant[i]);
        }
    } else if (constant[1] == 'X') {
        // Hexadecimal constant, append to object_code
        for (int i = 3; constant[i] != '\''; ++i) {
            sprintf(object_code, "%s%c", object_code, constant[i]);
        }
    } 
}

void write_listing_line(uchar *line, int locctr, uchar *object_code) {
    fprintf(listing_file, "%-30s%04X    %s\n", line, locctr, object_code);
}

// Function to write the Header record to the object program
void write_header_record(uchar *file_name, uchar *program_name, int starting_address, int program_length) {
    fprintf(object_code_file, "H%-6s%06X%06X\n", program_name, starting_address, program_length);
}

// Function to initialize a Text record
void initialize_text_record(uchar text_record[MAX_LINES][10], int *text_record_start, int *text_record_length, int locctr) {
    *text_record_start = locctr;
    *text_record_length = 0;
    for (int i = 0; i < MAX_LINES; ++i) {
        text_record[i][0] = '\0';
    }
}

// Function to write the End record to the object program
void write_end_record(uchar *file_name, int starting_address) {
    fprintf(object_code_file, "E%06X\n", starting_address);
}

// Helper function to write a Modification record to the object program
void write_modification_record(FILE *output_file, int address, int length) {
    fprintf(output_file, "M%06X%02X\n", address, length);
}

// Helper function to write EXTDEF and EXTREF records
void write_extdef_extref_records(uchar extdef_records[MAX_LINES][10], int extdef_count,
                                uchar extref_records[MAX_LINES][10], int extref_count) {
    // Write EXTDEF records
    if (extdef_count > 0) {
        fprintf(object_code_file, "D");
        for (int i = 0; i < extdef_count; ++i) {
            fprintf(object_code_file, "%-6s%06X", extdef_records[i], 0); // Assume all addresses are 0 for simplicity
        }
        fprintf(object_code_file, "\n");
    }

    // Write EXTREF records
    if (extref_count > 0) {
        fprintf(object_code_file, "R");
        for (int i = 0; i < extref_count; ++i) {
            fprintf(object_code_file, "%-6s", extref_records[i]);
        }
        fprintf(object_code_file, "\n");
    }
}

// Function to handle pass 2 of the assembler
static int assem_pass2(void) {
    // Reset variables for pass 2
    obj_code_count = 0;

    // Initialize the object code output and listing files
    make_objectcode_output("objectcode.txt", "listing.txt");

    // Initialize variables for text record generation
    uchar text_record[MAX_LINES][10];
    int text_record_start, text_record_length;

    // Initialize variables for LTORG processing
    int ltorg_address = -1;

    // Initialize variables for control section handling
    int csect_start_address = -1;
    int csect_length = 0;
    uchar csect_name[MAX_LINES];

    // Process lines until OPCODE is 'END'
    while (token_line < MAX_LINES) {
        // Read the next input line
        uchar *current_line = input_data[token_line];
        token_parsing(current_line);

        if (token_table[token_line] != NULL) {
            int opcode_index = search_opcode(token_table[token_line]->operator);

            if (opcode_index != -1) {
                int format = inst_table[opcode_index]->format;
                if (strcmp(token_table[token_line]->operator, "END") == 0){
                    break;
                }
                // Handle directives
                if (strcmp(token_table[token_line]->operator, "WORD") == 0) {
                    // Handle WORD directive
                    generate_object_code(0, opcode_index, locctr);
                } if (strcmp(token_table[token_line]->operator, "RESB") == 0 ||
                           strcmp(token_table[token_line]->operator, "RESW") == 0) {
                    // Handle RESB and RESW directives
                    initialize_text_record(text_record, &text_record_start, &text_record_length, locctr);
                } if (strcmp(token_table[token_line]->operator, "BYTE") == 0) {
                    // Handle BYTE directive
                    convert_constant_to_object_code(token_table[token_line]->operand[0], text_record[text_record_length]);
                    text_record_length++;
                } if (strcmp(token_table[token_line]->operator, "LTORG") == 0) {
                    // Handle LTORG directive
                    initialize_text_record(text_record, &text_record_start, &text_record_length, locctr);
                    handle_ltorg_directive();
                    ltorg_address = locctr;
                } if (strcmp(token_table[token_line]->operator, "CSECT") == 0) {
                    // Handle CSECT directive
                    if (csect_start_address != -1) {
                        // Write the last text record of the previous control section
                        write_text_record(object_code_file, text_record_start, text_record_length, text_record);
                        initialize_text_record(text_record, &text_record_start, &text_record_length, locctr);
                    }

                    // Update control section variables
                    csect_start_address = locctr;
                    csect_length = 0;
                    strcpy(csect_name, token_table[token_line]->label);
                } if (strcmp(token_table[token_line]->operator, "EXTREF") == 0 ||
                           strcmp(token_table[token_line]->operator, "EXTDEF") == 0) {
                    // Handle EXTREF and EXTDEF directives
                    // These directives have already been processed in pass 1, so no additional action is needed in pass 2.
                } if (format == 3 || format == 4) {
                    // Handle instructions with format 3/4
                    generate_object_code(format == 4, opcode_index, locctr);
                    if (locctr - ltorg_address >= LTORG_LENGTH) {
                        // If the length exceeds LTORG_LENGTH, write the text record
                        write_text_record(object_code_file, text_record_start, text_record_length, text_record);
                        initialize_text_record(text_record, &text_record_start, &text_record_length, locctr);
                        ltorg_address = locctr;
                    }
                }
            }
        }

        // Read Next
        token_line++;
    }

    // Write the last text record
    if (csect_start_address != -1) {
        write_text_record(object_code_file, text_record_start, text_record_length, text_record);
    }


    // Write EXTDEF and EXTREF records
    //write_extdef_extref_records(extDef, extDefCount , extRef, extRefCount);

    // Write the End record
    write_end_record(object_code_file, csect_start_address);

    // Close the output files
    fclose(object_code_file);
    fclose(listing_file);

    return 0;
}

