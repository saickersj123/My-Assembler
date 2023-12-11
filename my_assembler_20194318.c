#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include <ctype.h>
#include "my_assembler_20194318.h"


void add_to_symtab(const uchar *label, int loc, int is_equ, int sec) {

    // Check for duplicate symbol
    for (int i = 0; i < sym_index; i++) {
        if (strcmp(sym_table[i].symbol, label) == 0 && sym_table[i].sec == sec) {
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
    sym_table[sym_index].sec = sec;
    printf("Adding symbol to symtab: %s of section %d at address %04X\n", label, sec, loc);
    sym_index++;
    sec++;
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
            if (operand[1] == 'C' || operand[1] == 'c') {
                // Count the characters between single quotes
                int length = 0;
                for (int i = 3; operand[i] != '\''; ++i) {
                    length++;
                }
                    return length;
                } else if (operand[1] == 'X' || operand[1] == 'x') { // Count the characters between single quotes
                // Each hexadecimal digit requires 4 bits, so divide the length by 2
                    int length = 0;
                    for (int i = 3; operand[i] != '\''; ++i) {
                    length++;
                }
                    return (length + 1) / 2; // Adjust for hexadecimal format
                } 

        // Literal processing
        int literal_index = search_literal(operand);
        if (literal_index != -1) {
            return LTtab[literal_index].leng;
        } else {
            // Literal not found, handle error
            fprintf(stderr, "Error: Undefined literal - %s\n", operand);
            return -1;
        }
    }
    if (operand[0] == 'C' || operand[0] == 'c') {
        // Count the characters between single quotes
        int length = 0;
        for (int i = 2; operand[i] != '\''; ++i) {
            length++;
        }
        return length;
    } if (operand[0] == 'X' || operand[0] == 'x') {
        // Count the characters between single quotes
        // Each hexadecimal digit requires 4 bits, so divide the length by 2
        int length = 0;
        for (int i = 2; operand[i] != '\''; ++i) {
            length++;
        }
        return (length + 1) / 2; // Adjust for hexadecimal format
    } 
}

int search_symtab(uchar *symbol) {
    for (int i = 0; i < sym_index; ++i) {
        if (strcmp(sym_table[i].symbol, symbol) == 0) {
            return  sym_table[i].addr; // Symbol found, return its index
        }
    }
    return -1; // Symbol not found
}

int search_extRtab_sec(int section) {
    for (int i = 0; i < MAX_EXTREF; ++i) {
        if (section == extRef[i].sec) {
            return i;
        }
    }
    return -1;
}

int search_extDtab(uchar *symbol) {
    for (int i = 0; i < MAX_EXTDEF; ++i) {
        if (strcmp(extDef[i].symbol, symbol) == 0) {
            return  extDef[i].addr;
        }
    }
    return -1;
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
    /*
    token_table[token_line]->label = NULL;
    token_table[token_line]->operator = NULL;
    for (int i = 0; i < MAX_OPERAND; i++) {
    token_table[token_line]->operand[i][0] = '\0';
    }*/
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
        token_table[token_line]->label = NULL;
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
void handle_extdef(uchar *symbol) {
    // Check for duplicates before adding
    for (int i = 0; i < extDefCount; i++) {
        if (strcmp(extDef[i].symbol, symbol) == 0) {
            fprintf(stderr, "Error: Duplicate EXTDEF found - %s\n", symbol);
            // Handle the error as needed
            return;
        }
    }
    // Add the symbol to the EXTDEF table
    strcpy(extDef[extDefCount].symbol, symbol);
    extDef[extDefCount].addr = search_symtab(symbol);
    extDefCount++;
}

// Function to handle EXTREF directive
void handle_extref(uchar *symbol, int section) {
    // Check for duplicates before adding
    for (int i = 0; i < extRefCount; i++) {
        if (strcmp(extRef[i].symbol, symbol) == 0 && extRef[i].sec == section) {
            fprintf(stderr, "Error: Duplicate EXTREF found - %s\n", symbol);
            // Handle the error as needed
            return;
        }
    }
    // Add the symbol to the EXTREF table
    extRef[extRefCount].sec = section;
    strcpy(extRef[extRefCount].symbol, symbol);
    extRefCount++;
}

// Function to handle EQU directive
void handle_equ_directive(uchar *label, uchar *operand) {
    int equ_value;

    // Check if the operand is "*"
    if (strcmp(operand, "*") == 0) {
        // Operand is "*", refer to the location counter (locctr)
        equ_value = locctr;
    } else {
        // Operand is an expression, evaluate it
        equ_value = evaluate_expression(operand);
        locctr = equ_value; // Update the location counter with the evaluated expression
    }
        add_to_symtab(label, locctr, 1, sec); // The third argument (is_equ) is set to 1
}


// Function to handle LTORG directive
void handle_ltorg_directive(void) {
    int literal_length = 0;
    int current_pool = -1;  // Track the current LTORG pool

    // Iterate through the literal table
    for (int i = 0; i < LT_num; i++) {
        // Check if the literal has been assigned an address
        if (LTtab[i].addr == -1) {
            // If not assigned, assign the next available address in the current LTORG pool
            if (current_pool == -1) {
                // Start a new LTORG pool
                current_pool = locctr;
            }

            LTtab[i].addr = locctr + literal_length;
            literal_length += LTtab[i].leng;

            // Update the literal address in the intermediate file
            write_intermediate_file(LTtab[i].name, LTtab[i].addr);
        } else {
            // Literal has already been assigned an address, so reset the LTORG pool
            current_pool = -1;
        }
    }

    // Update the location counter with the total length of literals
    locctr = locctr + literal_length;
}


static int assem_pass1(void) {
    locctr = 0;
    starting_address = 0;
    token_line = 0;
    sym_index = 0;
    sec = 0;
    extRefCount = 0;

    // Initialize the symbol table
    for (int i = 0; i < MAX_LINES; ++i) {
        sym_table[i].sec = -1;
        sym_table[i].addr = -1;
        sym_table[i].symbol[0] = '\0';
        LTtab[i].name[0] = '\0';
        LTtab[i].addr = -1;
        LTtab[i].leng = 0;
    }

    // Initialize the csect_table array
    for (int i = 0; i < MAX_CSECT; ++i) {
        csect_table[i].sec = -1;
        //csect_table[i].locctr = -1;
        csect_table[i].program_length = 0;
    }

    for (int i = 0; i < MAX_EXTDEF; i++) {
        extDef[i].addr = -1;
        extDef[i].sec = -1;
        extDef[i].symbol[0] = '\0';
        extRef[i].addr = -1;
        extRef[i].sec = -1;
        extRef[i].symbol[0] = '\0';
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
        token_table[token_line]->addr = locctr; 
        write_intermediate_file(current_line, locctr);

        token_line++;

    } else {
        // If no START directive, initialize LOCCTR to 0
        locctr = 0;
        token_table[token_line]->addr = locctr; 
        write_intermediate_file(current_line, locctr);
    }

    // Process lines until OPCODE is 'END'
    while (token_line < line_num) {
        current_line = input_data[token_line];
        token_parsing(current_line);

        if (token_table[token_line] != NULL) {

            // Check for CSECT directive
            if (strcmp(token_table[token_line]->operator, "CSECT") == 0) {
                if(locctr < token_table[token_line - 2]->addr) {
                    csect_table[sec].program_length = token_table[token_line - 2]->addr;
                } else {
                    csect_table[sec].program_length = locctr;
                }
                csect_table[sec].sec = sec;
                // Start a new section, reset the program counter
                locctr = strtol(token_table[token_line]->operand[0], NULL, 16);
                sec++;

                // Add the section name to the symbol table
                if (token_table[token_line]->label != NULL) {
                    add_to_symtab(token_table[token_line]->label, locctr, 0, sec);
                }
                // Write line to intermediate file
                write_intermediate_file(current_line, locctr);
                token_table[token_line]->addr = locctr; 
                token_line++;
                continue;  // Skip the rest of the loop for CSECT
            }
            // Check for EQU directive
            if (strcmp(token_table[token_line]->operator, "EQU") == 0) {
                handle_equ_directive(token_table[token_line]->label, token_table[token_line]->operand[0]);
            }
            if (strcmp(token_table[token_line]->operator, "EXTREF") == 0) {
                    // Process EXTREF directive
                    for (int i = 0; i < MAX_OPERAND &&
                    token_table[token_line]->operand[i][0] != '\0'; i++) {
                    handle_extref(token_table[token_line]->operand[i],sec);
                    } 
            }
            // Add the label to the symbol table
            if (token_table[token_line]->label != NULL) {
                add_to_symtab(token_table[token_line]->label, locctr, 0, sec);
            }     

            // Write line to intermediate file for other directives and instructions
            write_intermediate_file(current_line, locctr);
            token_table[token_line]->addr = locctr;
             
                int opcode_index = search_opcode(token_table[token_line]->operator);

                if (opcode_index != -1) {

                    // Check for 4-format instruction
                    if (token_table[token_line]->operator[0] == '+') {
                        locctr += 4;
                        
                    } else {
                        locctr += inst_table[opcode_index]->format;
                    }
                    
                if (strcmp(token_table[token_line]->operator, "WORD") == 0) {
                    locctr += 3;
                    
                } if (strcmp(token_table[token_line]->operator, "RESW") == 0) {
                    locctr += 3 * atoi(token_table[token_line]->operand[0]);
                   
                } if (strcmp(token_table[token_line]->operator, "RESB") == 0) {
                    locctr += atoi(token_table[token_line]->operand[0]);
                  
                } if (strcmp(token_table[token_line]->operator, "BYTE") == 0) {
                    locctr += calculate_byte_length(token_table[token_line]->operand[0]);
                  
                } 
                 if (strcmp(token_table[token_line]->operator, "LTORG") == 0) {
                    handle_ltorg_directive();
                }

                // Check for the "END" directive to exit the loop
                if (strcmp(token_table[token_line]->operator, "END") == 0) {
                    csect_table[sec].program_length = locctr;
                    handle_ltorg_directive();
                    break;
                } 
            }

            
            // Check for literals and add them to the literal table
            for (int i = 0; i < MAX_OPERAND; ++i) {
                uchar *operand = token_table[token_line]->operand[i];
                if (operand[0] == '=' && (operand[1] == 'C' || operand[1] == 'X')) {
                // Found a literal
                    int length = calculate_byte_length(operand);
                    int duplicate_found = 0;  // Flag to track duplicate literals
                    for (int j = 0; j < LT_num; j++) {
                        if (strcmp(LTtab[j].name, operand) == 0) {
                            fprintf(stderr, "Error: Duplicate literal found - %s\n", operand);
                            duplicate_found = 1;
                            // Handle the error as needed
                            break;  // No need to check further
                        }
                    }
                    if (!duplicate_found) {
                        // Add the literal to the literal table
                        LTtab[LT_num].leng = length;
                        strcpy(LTtab[LT_num].name, operand);
                        LTtab[LT_num].addr = -1;  // Not assigned an address yet
                        LT_num++;
                    }
                }
            }

        }
        //Read Next
        token_line++;

    }

    csect_table[sec].program_length = locctr;
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

    fprintf(symtab_output_file, "Symbol\tAddress\tSection\n");
    fprintf(symtab_output_file, "-----------------------\n");

    for (int i = 0; i < sym_index; i++) {
        if (sym_table[i].addr != -1) {
            fprintf(symtab_output_file, "%s\t%04X\t%d\n", sym_table[i].symbol, sym_table[i].addr, sym_table[i].sec);
        }
    }

    fprintf(symtab_output_file, "\nLiteral\tAddress\tPoolNum\n");
    fprintf(symtab_output_file, "-----------------------\n");
    for (int i = 0; i < LT_num; i++) {
        if (LTtab[i].addr != -1) {
            fprintf(symtab_output_file, "%s\t%04X\t%d\n", LTtab[i].name, LTtab[i].addr, i);
        }
    }
    fclose(symtab_output_file);
}


void write_intermediate_file(uchar *str, int locctr)
{
    FILE *intermediate_file;
    if (is_first_write)
    {
        intermediate_file = fopen("intermediate.txt", "w");
        if (intermediate_file == NULL)
        {
            fprintf(stderr, "Error: Unable to open intermediate file for writing.\n");
            return;
        }
        is_first_write = 0;
    }
    else
    {
        intermediate_file = fopen("intermediate.txt", "a");
        if (intermediate_file == NULL)
        {
            fprintf(stderr, "Error: Unable to open intermediate file for writing.\n");
            return;
        }
    }

    fprintf(intermediate_file, "%04X %s\n", locctr, str);
    fclose(intermediate_file);
}

// Function to evaluate an expression
int evaluate_expression(uchar *expr) {
    // Tokenize the expression based on the '-' operator
    char *token = strtok(expr, "-");

    // Parse the first operand (BUFEND)
    int operand1 = -1;
    if (token != NULL) {
        operand1 = search_symtab(token);
        if (operand1 == -1) {
            // Handle the case where the symbol is not found in the symbol table
            fprintf(stderr, "Error: Symbol not found - %s\n", token);
            return -1;
        }
    } else {
        // Handle the case where the expression is not properly formatted
        fprintf(stderr, "Error: Invalid expression format - %s\n", expr);
        return -1;
    }

    // Parse the second operand 
    token = strtok(NULL, " ");
    int operand2 = -1;
    if (token != NULL) {
        operand2 = search_symtab(token);
        if (operand2 == -1) {
            // Handle the case where the symbol is not found in the symbol table
            fprintf(stderr, "Error: Symbol not found - %s\n", token);
            return -1;
        }
    } else {
        // Handle the case where the expression is not properly formatted
        fprintf(stderr, "Error: Invalid expression format - %s\n", expr);
        return -1;
    }

    // Return the result of the subtraction operation
    return operand1 - operand2;
}




void make_objectcode_output(uchar *file_name, uchar *list_name) {
    // Open the object code output file
   if (first_write)
    {
        object_code_file = fopen(file_name, "w");
        listing_file = fopen(list_name, "w");
        if (object_code_file == NULL)
        {
            fprintf(stderr, "Error: Unable to open objfile and lstfile for writing.\n");
            fclose(object_code_file);
            fclose(listing_file);
            exit(1);
        }
        first_write = 0;
    }
    else
    {
        object_code_file = fopen(file_name, "a");
        listing_file = fopen(list_name, "a");
        if (object_code_file == NULL)
        {
            fprintf(stderr, "Error: Unable to open objfile and lstfile for writing.\n");
            fclose(object_code_file);
            fclose(listing_file);
            exit(1);
        }
    }


}

// Function to generate object code based on the instruction format
void generate_object_code(token *tok, int format, int opcode_index,
int locctr, int objcount) {
    int object_code;
    int nixbpe ;
    uchar *op = tok->operator;
    // Check for format 1 instruction
    if (format == 1) {
        // Format 1: opcode only
        object_code = inst_table[opcode_index]->op;
    }
    // Check for format 2 instruction
    else if (format == 2) {
        // Format 2: opcode + registers
        int reg1 = atoi(tok->operand[0]);
        int reg2 = atoi(tok->operand[1]);
        object_code = (inst_table[opcode_index]->op << 8) + (reg1 << 4) + reg2;
    }
    // Check for format 3/4 instruction
    else {
        // Format 3/4: opcode + (nixbpe) + displacement
        nixbpe = 0;

        // Set 'n' bit
        if ( op[0]=='+'|| tok->operand[0][0] == '@') {
            nixbpe |= 1 << 5;
        }

        // Set 'i' bit
        if (tok->operand[0][0] != '#') {
            nixbpe |= 1 << 4;
        }

        // Set 'x' bit
        if (tok->operand[1] != NULL && strcmp(tok->operand[1], ",X") == 0) {
            nixbpe |= 1 << 3;
        }
        int disp;
        if (op[0]=='+') {
            // Format 4: 20-bit address in operand field
            nixbpe |= 1 << 1;  // Set 'b' bit
            nixbpe |= 1;       // Set 'p' bit

            // Calculate the 20-bit address
            disp = strtol(tok->operand[0] + 1, NULL, 16);
        } 
        if (format == 3) {
            // Format 3: 12-bit displacement from base or PC
            int target_address = search_symtab(tok->operand[0]);

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
    sprintf(obj_codes[objcount].code, "%06X", object_code);
    obj_codes[objcount].address = object_code;
    tok->nixbpe = nixbpe;
    // Add object code to Text record
    strcat(text_record[text_record_count], obj_codes[objcount].code);
    text_record_count++;
    printf("Object code: %06X\n", object_code);
}

// Helper function to write a Text record to the object program
void write_text_record(int start_address, int length) {
    fprintf(object_code_file, "T%06X%02X%s\n", start_address, length / 2, text_record[1]);
}

// Helper function to convert constant to object code
void convert_constant_to_object_code(uchar *constant, uchar *object_code) {
    // Extract the content between single quotes and convert accordingly

    // Initialize object_code
    object_code[obj_count] = '\0';

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
    obj_count++; 
}

void write_listing_line(uchar *line, int locctr, uchar *object_code) {
    fprintf(listing_file, "%-30s%04X    %s\n", line, locctr, object_code);
}

// Function to write the Header record to the object program
void write_header_record(uchar *program_name, int starting_address, int program_length) {
    fprintf(object_code_file, "H%-6s%06X%06X\n", program_name, starting_address, program_length);
}

// Function to initialize a Text record
void initialize_text_record(int locctr) {
    for (int i = 0; i < MAX_LINES; ++i) {
        text_record[i][0] = '\0';
    }
    text_record[0][0] = '\0';
    text_record[1][0] = '\0';
}

// Function to write the End record to the object program
void write_end_record(int starting_address) {
    fprintf(object_code_file, "E%06X\n", starting_address);
}

// Helper function to write a Modification record to the object program
void write_modification_record(int start, int length) {
    fprintf(object_code_file, "M%06X%02X\n", start, length);
}

// Helper function to write modification records to object program
void write_modification_records() {
    for (int i = 0; i < mod_record_count; ++i) {
        write_modification_record(mod_records[i].start, mod_records[i].length);
    }
}

// Helper function to write EXTDEF and EXTREF records
void write_extdef_extref_records(uchar *record_type) {
    // Write EXTDEF or EXTREF records
    if (strcmp(record_type, "D") == 0 && extDefCount > 0) {
        fprintf(object_code_file, "D");

        for (int i = 0; i < extDefCount; ++i) {
            fprintf(object_code_file, "%-6s%06X", extDef[i].symbol, extDef[i].addr);
        }

        fprintf(object_code_file, "\n");
    } else if (strcmp(record_type, "R") == 0 && extRefCount > 0) {
        fprintf(object_code_file, "R");
        for (int i = 0; i < extRefCount; ++i) {
            if(extRef[i].sec == sec)
            fprintf(object_code_file, "%-6s", extRef[i].symbol);
        }
        fprintf(object_code_file, "\n");
    }
}


// Function to handle pass 2 of the assembler
static int assem_pass2(void) {
    // Reset variables for pass 2
    obj_count = 0;
    text_record_count = 0;
    mod_record_count = 0;
    token_line = 0;

    // Initialize the object code output and listing files
    make_objectcode_output("objectcode.txt", "listing.txt");

    // Initialize variables for text record generation
    int text_record_start, text_record_length;

    // Initialize variables for control section handling
    sec = 0;
    // Initialize variables for EXTDEF and EXTREF
    extDefCount = 0;

    // Process lines until OPCODE is 'END'
    while (token_line < MAX_LINES) {
        // Read the next input line
        token *current_line = token_table[token_line];
                // Handle START directive
                if (strcmp(current_line->operator, "START") == 0) {
                    // Extract starting address from operand
                    starting_address = strtol(current_line->operand[0], NULL, 16);
                    // Write the Header record
                    write_header_record(current_line->label, starting_address, csect_table[sec].program_length);
                    // Initialize first Text record
                    initialize_text_record(locctr);
                    write_listing_line(input_data[token_line], locctr, text_record[text_record_count]);
                    token_line++;
                    continue;  // Skip to the next iteration
                }
                if (strcmp(current_line->operator, "CSECT") == 0) {
                    sec++;
                    // Initialize variables for the new section
                    csect_start_address = starting_address;
                    csect_length = csect_table[sec].program_length;

                    // Write Header record for the new section
                    fprintf(object_code_file, "\n");
                    write_header_record(current_line->label, csect_start_address, csect_length);
                    // Initialize first Text record
                    initialize_text_record(locctr);                
                    token_line++;
                    continue;  // Skip the rest of the loop for CSECT
                } 
                if (strcmp(current_line->operator, "EXTDEF") == 0) {
                    // Handle EXTDEF directive
                    // Record the symbols defined in EXTDEF
                    for (int i = 0; i < MAX_OPERAND && current_line->operand[i][0] != '\0'; ++i) {
                        strcpy(extDef[extDefCount].symbol, current_line->operand[i]);
                        extDef[extDefCount].addr = search_symtab(current_line->operand[i]);
                        extDefCount++;
                    }
                    // Write Define (EXTDEF) records
                    write_extdef_extref_records("D");
                } if (strcmp(current_line->operator, "EXTREF") == 0) {
                    // Write Refer (EXTREF) records
                    write_extdef_extref_records("R");
                }

        if (current_line != NULL) {
            int opcode_index = search_opcode(current_line->operator);

            if (opcode_index != -1) {
                int format = inst_table[opcode_index]->format;

                // Handle directives
                if (format >= 3) {
                    // Handle instructions with format 3/4
                    // Search SYMTAB for OPERAND and store symbol value as operand address
                    operand_address = search_symtab(current_line->operand[0]);

                    if (operand_address != -1) {
                        generate_object_code(current_line,format, opcode_index, operand_address, obj_count);
                        write_text_record(text_record_start, text_record_length);
                    } else {
                        // Store 0 as operand address
                        operand_address = 0;
                        fprintf(stderr, "Error: Symbol %s not found in SYMTAB\n", current_line->operand[0]);
                        // Set error flag (undefined symbol)
                    }
                } if (strcmp(current_line->operator, "BYTE") == 0 ||
                           strcmp(current_line->operator, "WORD") == 0) {
                    // Handle BYTE and WORD directives
                    // Convert constant to object code
                    convert_constant_to_object_code(current_line->operand[0], text_record[0]);

                    // Add object code to Text record
                    strcat(text_record[1], text_record[0]);
                    }

                // Check if object code will fit into the current Text record
                if (strlen(text_record[1]) / 2 >= MAX_TEXT_RECORD_LENGTH) {
                    // Write the Text record to the object program
                    write_text_record(text_record_start, text_record_length);
                    // Initialize new Text record
                    initialize_text_record(locctr);
                    }

                // Write listing line
                write_listing_line(input_data[token_line], locctr, text_record[0]);
                
            }
        }

        // Check for END directive
        if (strcmp(current_line->operator, "END") == 0) {
            // Write last Text record to the object program
            write_text_record(text_record_start, text_record_length);

            // Write End record to the object program
            write_end_record(starting_address);

            // Write modification records
            write_modification_records();

            // Break out of the loop
            break;
        }

        // Read Next
        token_line++;
    }

    // Close the output files
    fclose(object_code_file);
    fclose(listing_file);

    return 0;
}



