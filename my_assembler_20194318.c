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
            // Literal found, return itext_record_start index
            return i;
        }
    }
    // Literal not found
    return -1;
}

int search_literaladdr(uchar *operand) {
    for (int i = 0; i < LT_num; ++i) {
        if (strcmp(LTtab[i].name, operand) == 0) {
            // Literal found, return itext_record_start index
            return LTtab[i].addr;
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
                // Each hexadecimal digit requires 4 bitext_record_start, so divide the length by 2
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
        // Each hexadecimal digit requires 4 bitext_record_start, so divide the length by 2
        int length = 0;
        for (int i = 2; operand[i] != '\''; ++i) {
            length++;
        }
        return (length + 1) / 2; // Adjust for hexadecimal format
    } 
}

int search_symtab(uchar *symbol, int section) {
    for (int i = 0; i < sym_index; ++i) {
        if ((strcmp(sym_table[i].symbol, symbol) == 0) && (sym_table[i].sec == section)){
            return  sym_table[i].addr; // Symbol found, return itext_record_start index
        }
    }
    return -1; // Symbol not found
}

int search_extRtab(uchar *symbol, int section) {
    for (int i = 0; i < MAX_EXTREF; ++i) {
        if ((strcmp(extRef[i].symbol, symbol) == 0) && (extRef[i].sec == section)) {
            return extRef[i].addr;
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
    inst_count = inst_index;
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
       // Check if the line startext_record_start with a dot (comment)
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

    // Check commentext_record_start and empty Line 
   if (str[0] == '\0' || str[0] == '\n' || str[0] == '.') {
        return 0;
    }
    uchar *scopy = strdup((uchar *) str);

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
        // Skip commentext_record_start and empty lines
        return -1;
    }
    
    for (int i = 0; i < inst_count; i++) {
        if (strcmp(str, inst_table[i]->str) == 0 || (str[0] == '+' && strcmp(str + 1, inst_table[i]->str) == 0)) {
            // Test print searched operator on terminal
            printf("Operator: %s / ", inst_table[i]->str);
            printf("Format: %d / ", inst_table[i]->format);
            printf("Opcode Value: 0x%02X\n", inst_table[i]->op);
            return i; 
        }
    }

    // Search Faild
    fprintf(stderr, "Error: Opcode not found for %s\n", str);
    return -1;
}

int tok_search_opcode(uchar *str) {
        if (str == NULL || str[0] == '\0' || str[0] == '.') {
        // Skip commentext_record_start and empty lines
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
    extDef[extDefCount].addr = search_symtab(symbol, sec);
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
    current_pool = -1;  // Track the current LTORG pool

    // Iterate through the literal table
    for (int i = 0; i < LT_num; i++) {
        // Check if the literal has been assigned an address
        if (LTtab[i].addr == -1) {
            // If not assigned, assign the next available address in the current LTORG pool
            if (current_pool == -1) {
                // Start a new LTORG pool
                current_pool = i;
            }

            LTtab[i].addr = locctr + literal_length;
            literal_length += LTtab[i].leng;
            LTtab[i].value = current_pool;
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
    LT_num = 0;
    // Initialize the symbol table
    for (int i = 0; i < MAX_LINES; ++i) {
        sym_table[i].sec = -1;
        sym_table[i].addr = -1;
        sym_table[i].symbol[0] = '\0';
    }
    
    for(int i = 0; i < MAX_LITERALS; i++){
        LTtab[i].name[0] = '\0';
        LTtab[i].addr = -1;
        LTtab[i].leng = -1;
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
    
    for(int i = 0; i < line_num; i++) {
        printf("line : %d\n Locctr : %04X\n", i+1, token_table[i]->addr);
    }
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
    fprintf(symtab_output_file, "-------------------------\n");

    for (int i = 0; i < sym_index; i++) {
        if (sym_table[i].addr != -1) {
            fprintf(symtab_output_file, "%s\t%04X\t%d\n", sym_table[i].symbol, sym_table[i].addr, sym_table[i].sec);
        }
    }

    fprintf(symtab_output_file, "\nLiteral\tAddress\tPoolNum\n");
    fprintf(symtab_output_file, "----------------------------\n");
    for (int i = 0; i < LT_num; i++) {
        if (LTtab[i].addr != -1) {
            fprintf(symtab_output_file, "%s\t%04X\t%d\n", LTtab[i].name, LTtab[i].addr, LTtab[i].value);
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
    uchar *token = strtok(expr, "-");

    // Parse the first operand (BUFEND)
    int operand1 = -1;
    if (token != NULL) {
        operand1 = search_symtab(token, sec);
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
        operand2 = search_symtab(token, sec);
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

// Function to get the numerical value of a register
int getREGnum(uchar *register_name) {
    if (strcmp(register_name, "A") == 0) {
        return 0;
    } else if (strcmp(register_name, "X") == 0) {
        return 1;
    } else if (strcmp(register_name, "L") == 0) {
        return 2;
    } else if (strcmp(register_name, "B") == 0) {
        return 3;
    } else if (strcmp(register_name, "S") == 0) {
        return 4;
    } else if (strcmp(register_name, "T") == 0) {
        return 5;
    } else if (strcmp(register_name, "F") == 0) {
        return 6;
    } else {
        // Handle an invalid register name (this depends on your error handling strategy)
        fprintf(stderr, "Error: Invalid register name : %s\n", register_name);
        exit(EXIT_FAILURE);
    }
}

// Function to generate object code based on the instruction format
int generate_object_code(int format) {
    int dxx;
    int disp;
    int loc;
    int loc2;
    int op_index = search_opcode(token_table[token_line]->operator);
    int opcode = inst_table[op_index]->op;
    int lt_index = search_literal(token_table[token_line]->operand[0]);
    token *ct = token_table[token_line];
    token *nt = token_table[token_line + 1];
  
  switch(format) {
    case 1: // 1형식 명령어(FIX, FLOAT, HIO, NORM, SIO, TIO)
      object_code[0] = opcode;
      object_code[1] = '\0';
      break;

    case 2: // 2형식 명령어(operand 형식은 r1 또는 r1,r2 또는 n 또는 r1,n)
      object_code[0] = opcode;
      if (ct->operand[0][0] == '\0')
        printf("Error! Operand expected\n");

      if (ct->operand[0][0] < 'A') { // op1이 레지스터가 아닌 상수값(n)
        sscanf(ct->operand[0], "%d", &dxx);
        object_code[1] = dxx << 4; // n
      } else {
        object_code[1] = getREGnum(ct->operand[0]) << 4; // r1
      }
      if (ct->operand[1][0] != '\0') {
        if (ct->operand[1][0] < 'A') { // op2가 레지스터가 아닌 상수값(n)
          sscanf(ct->operand[1]+1, "%d", &dxx);
          object_code[1] = object_code[1] | dxx; // n
        } else {
          object_code[1] = object_code[1] | getREGnum(ct->operand[1]); // r2
        }
      }
      break;

    case 3: // 3형식 명령어
      switch (ct->operand[0][0]) {
        case '#': // 즉시 주소 지정(Immediate addressing)
          object_code[0] = opcode + 1; // n=0,i=1 이므로 +1 함
          if (ct->operand[0][1] >= 'A') { // PC 상대주소 + 즉시 주소 지정인 경우(예, #LENGTH)
            dxx = search_symtab(ct->operand[0]+1, sec);
            if (dxx == -1)
              printf("Error: Undefind symbol: %s\n", ct->operand[0]+1);
            else
              dxx -= nt->addr; // op1과 다음 명령어를 가리키고 있는 LOCCTR 과의 차이
            object_code[1] = (dxx >> 8) & 15; // TA 상위 1 니블
            object_code[1] = object_code[1] | 32; // p=1 (PC 상대주소)
            object_code[2] = dxx & 255; // TA 하위 2 니블
          } else { // 상수값
             sscanf(ct->operand[0]+1, "%d", &dxx);
             object_code[1] = (dxx >> 8) & 15; // TA 상위 1 니블
             object_code[2] = dxx & 255; // TA 하위 2 니블
          }
          break;
        case '\0': // operand가 없다(RSUB)
           object_code[0] = opcode + 3; // n=1,i=1 이므로 +3 함
           object_code[1] = 0;
           object_code[2] = 0;
           break;
        default:
          if (ct->operand[0][0] == '@') { // 간접 주소 지정(Indirect addressing)
            object_code[0] = opcode + 2; // n=1,i=0 이므로 +2 함
            loc = search_symtab(ct->operand[0]+1, sec);
            disp =loc - nt->addr; // op1과 다음 명령어를 가리키고 있는 LOCCTR 과의 차이
          }
          else if (ct->operand[0][0] == '=') { // 리터럴
            object_code[0] = opcode + 3; // n=1,i=1 이므로 +3 함
            loc = LTtab[lt_index].addr; // Literal Table(pool) 에서 검색
            disp = loc - nt->addr; // op1과 다음 명령어를 가리키고 있는 LOCCTR 과의 차이
          }
          else {
            object_code[0] = opcode + 3; // n=1,i=1 이므로 +3 함
            loc = search_symtab(ct->operand[0], sec);
            disp = loc - nt->addr; // op1과 다음 명령어를 가리키고 있는 LOCCTR 과의 차이
          }

          if (loc < 0) {
            printf("Error: Undefinded symbol: %s\n", ct->operand[0]);
            loc = 0;
          }

          if ((abs(disp) >= 4096) && (loc >= 0)) { // base 상대주소를 사용해야하는 경우
            disp = abs(BASEADDR - loc);
            object_code[1] = (disp >> 8) & 15; // TA 상위 1 니블
            object_code[1] = object_code[1] | 64; // b=1 (base 상대주소)
            object_code[2] = disp & 255; // TA 하위 2 니블
            printf("opcode->%s base->%X loc->%X disp->%d\n", opcode, BASEADDR, loc, disp);
          }
			else if ((disp < 4096) && (loc >= 0)) {
            object_code[1] = (disp >> 8) & 15; // TA 상위 1 니블
            object_code[1] = object_code[1] | 32; // p=1 (PC 상대주소)
            object_code[2] = disp & 255; // TA 하위 2 니블
          }
          else {
            int ddd;
            if (ct->operand[0][0] == '@') // Indirect addressing
              sscanf(ct->operand[0]+1, "%d", &ddd);
            else
              sscanf(ct->operand[0], "%d", &ddd);
            object_code[1] = (ddd >> 8);
            object_code[2] = ddd & 255;
          }

          if (ct->operand[1][0] == 'X') // 인덱스 주소 지정
            object_code[1] = object_code[1] + 128; // x=1
      }
    printf("<Format 3> opcode->%s obj->%02X%02X%02X\n", inst_table[op_index]->str, object_code[0], object_code[1], object_code[2]);
      break;

    case 4: // 4형식
      switch (ct->operand[0][0]) {
        case '#': // 즉시 주소 지정(Immediate addressing)
          object_code[0] = opcode + 1; // n=0,i=1 이므로 +1 함
          if (ct->operand[0][1] >= 'A') { // PC 상대주소 + 즉시 주소 지정인 경우(예, #LENGTH)
            dxx = search_symtab(ct->operand[0]+1, sec);
            if (dxx== -1)
              printf("Error: Undefind symbol: %s\n", ct->operand[0]+1);
            else
              dxx -= nt->addr; // op1과 다음 명령어를 가리키고 있는 LOCCTR 과의 차이
          } else  // 상수값
            sscanf(ct->operand[0]+1, "%d", &dxx);
          object_code[1] = 16; // e=1
          object_code[2] = (dxx >> 8) & 255; // TA 상위 1 바이트
          object_code[3] = dxx & 255; // TA 하위 1 바이트
          break;
        default:
          if (ct->operand[0][0] == '@') { // 간접 주소 지정(Indirect addressing)
            object_code[0] = opcode + 2; // n=1,i=0 이므로 +2 함
            loc2 = search_symtab(ct->operand[0]+1, sec);
          }
          else {
            object_code[0] = opcode + 3; // n=1,i=1 이므로 +3 함
            loc2 = search_symtab(ct->operand[0], sec);
            if (loc2 < 0) { // 현재 섹션에서 찾았는데 없다
              loc2 = search_extRtab(ct->operand[0], sec); // 외부 참조인지 검사
              if (loc2 >= 0) {
                // 수정 레코드(Modification record)에 추가
                // L2-STARTADDR+1 -> 현 제어섹션의 처음부터 상대적으로 표기된 수정될 필드의 주소
                // 05 -> 하프 바이트로 나타낸 수정될 레코드의 길이(여기서는 05로 fix시킴)
                // + -> 수정 플래그(여기서는 +로 fix시킴)
                sprintf(mod_record[mod_record_count], "M%06X05+%-6s\n", ct->addr-csect_start_address+1, ct->operand[0]);
                mod_record_count++;
              }
            }
            if (loc2 < 0) {
               printf("Error: Undefinded symbol: %s\n", ct->operand[0]);
               loc2=0;
            }
          }
          if (loc2 >= 0) {
            object_code[1] = 16; // e=1
            object_code[2] = (loc2 >> 8); // TA 상위 1 바이트
            object_code[3] = loc2 & 255;  // TA 하위 1 바이트
          }
          else {
            int ddd;
            if (ct->operand[0][0] == '@') // Indirect addressing
              sscanf(ct->operand[0]+1, "%d", &ddd);
            else
              sscanf(ct->operand[0], "%d", &ddd);
            object_code[1] = 16; // e=1
            object_code[2] = (ddd >> 8); // TA 상위 1 바이트
            object_code[3] = ddd & 255;  // TA 하위 1 바이트
          }
          if (ct->operand[1][0] == 'X') // 인덱스 주소 지정
            object_code[1] = object_code[1] + 128; // x=1
      }
      printf("<Format 4> opcode->%s obj->%02X%02X%02X%02X\n", inst_table[op_index]->str, object_code[0], object_code[1], object_code[2], object_code[3]);
      break;
  }
}

uchar HEXTAB[]= {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

int hexstr2dec(char H)
{
  int i;
  for (i = 0; i <= 15; i++)
    if (HEXTAB[i] == H)
      return (i);
  return (-1);
}

int write_literal()
{
    int n, len=0;
    token *ct = token_table[token_line];
    for (; (LT_num < current_pool) && (LTtab[LT_num].value == LT_num); LT_num++) {
        len = 0;
    if (LTtab[LT_num].name[1] == 'C') { // 지정한 문자로 초기화되는 바이트 수
      n = 0;
      while (LTtab[LT_num].name[n+3] != '\'') {
        object_code[len] = LTtab[LT_num].name[n+3]; // character를 byte로 환산
        n++;
        len++;
      }
      object_code[len] = '\0';
    }
    else if (LTtab[LT_num].name[1] == 'X') { // 지정한 16진수로 초기화되는 바이트 수
      n = 0;
      while (LTtab[LT_num].name[n+3] != '\'') {
        // 16진수 두 글자를 1 byte로 환산
        object_code[len] = hexstr2dec(LTtab[LT_num].name[n+3])*16 + hexstr2dec(LTtab[LT_num].name[n+4]);
        n += 2;
        len++;
      }
      object_code[len] = '\0';
    }
    else {
      sscanf(ct->operand[0]+1, "%d", object_code);
      len++;
    }

    if (ct->operand[0][1] != '=')
      write_listing_line(0);
    else
      write_listing_line(len);

  } // for
  return(len);
}

int handlepass2(){
  int n = 0;
  token *ct = token_table[token_line];
  token *nt = token_table[token_line+1];
  inst_index = search_opcode(ct->operator);
  int format = inst_table[inst_index]->format; // 명령어 길이를 구함(지시자의 길이는 0)
  if(ct->operator[0] == '+') {
    format = 4;
  }
  if (format > 0) { // 어셈블러 지시자가 아님
    generate_object_code(format);
    write_listing_line(format);
  }
  else if (format == 0) { // 어셈블러 지시자
    if (strcmp(ct->operator, "BASE") == 0) { // 베이스 레지스터의 값을 어셈블러에게 알려준다
      write_listing_line(format);
      BASEADDR = search_symtab(ct->operand[0], sec); // 기호 테이블에서 검색
      if (BASEADDR < 0)
        BASEADDR = 0;
    }
    else if (strcmp(ct->operator, "NOBASE") == 0) { // 베이스 레지스터의 값을 해제한다
      write_listing_line(format);
      BASEADDR = 0;
    }
    else if (strcmp(ct->operator, "WORD") == 0) {
      if (ct->operand[0][0] > 'A') { // 기호 사용
        int W3, W4=0;
        W3 = search_symtab(ct->operand[0], sec); // 기호 테이블에서 검색
        if (W3 < 0) {
          W3 = search_extRtab(ct->operand[0], sec); // 외부 참조인지 검사
          if (W3 < 0) {
            printf("Error: Undefinded symbol: %s\n", ct->operand[0]);
            object_code[0] = object_code[1] = object_code[2] = 0;
            return(3); 
          }
          // 수정 레코드(Modification record)에 추가
          // L2-STARTADDR+1 -> 현 제어섹션의 처음부터 상대적으로 표기된 수정될 필드의 주소
          // 05 -> 하프 바이트로 나타낸 수정될 레코드의 길이(여기서는 05로 fix시킴)
          // + -> 수정 플래그(여기서는 +로 fix시킴)
          sprintf(mod_record[mod_record_count], "M%06X06+%-6s\n", ct->addr-csect_start_address, ct->operand[0]);
          mod_record_count;
        }

        int isplus = strchr(ct->operand[0], '+');
        int issub = strchr(ct->operand[0], '-');
        uchar exp = '\0';
        if (isplus){
            exp = '+';
        }else if(issub){
            exp = '-';
        }

        uchar exop[2][10];
        exop[0][0] = '\0';
        int exopi = 0; 
        // 아래 문장은 복수개의 기호(MAXLEN WORD BUFEND-BUFFER 같은 문장)을 처리하기 위해...
        if ((strchr(ct->operand[0], '+')) || (strchr(ct->operand[0], '-'))) { // 수식의 연산은 +, - 만 지원됨
            uchar *tok = strtok(ct->operand[0], "+-");
            strcpy(exop[exopi], tok);
            tok = strtok(NULL," \n\t");
            strcpy(exop[exopi+1], tok);
            while (exopi < 1) {
            W4 = search_symtab(exop[exopi], sec);
            if (W4 < 0) {
              W4 = search_extRtab(exop[exopi], sec);
              if (W4 < 0) {
                printf("Error: Undefinded symbol: %s\n", exop[exopi]);
                object_code[0] = object_code[1] = object_code[2] = 0;
                return(3);
              }
              sprintf(mod_record[mod_record_count], "M%06X06%c%-6s\n", ct->addr-csect_start_address, exp, exop[exopi]);
              mod_record_count++;
            }
            if (isplus){
              W3 = W3 + W4;}
            else if(issub){
              W3 = W3 - W4;}
            exopi++;
            }
        }

        object_code[0] = (W3 >> 16) & 255;
        object_code[1] = (W3 >> 8) & 255;
        object_code[2] = W3 & 255;
      }
      else {
        sscanf(ct->operand[0], "%d", &n);
        object_code[0] = 0;
        object_code[1] = (n >> 8) & 255;
        object_code[2] = n & 255;
      }
      write_listing_line(3); // 3형식이 아니라 단지 인쇄를 위해
      return (3);
    }
    else if (strcmp(ct->operator, "RESW") == 0) {
      write_listing_line(format);
      sscanf(ct->operand[0], "%d", &n); // n은 예약되는 워드(3 byte)의 갯수
      write_text_record(); // 이전 까지의 텍스트 레코드(Text record)의 출력
      // TS -> 텍스트 레코드에 포함될 목적 코드 시작주소
      // LOCCTR를 대입하는것은 현재 RESW가 할당받는 공간을 넘어서 주소가 시작되어야 하므로...
      text_record_start = ct->addr;
    }
    else if (strcmp(ct->operator, "RESB") == 0) {
      write_listing_line(format);
      sscanf(ct->operand[0], "%d", &n); // n은 예약되는 byte의 갯수
      write_text_record(); // 이전 까지의 텍스트 레코드(Text record)의 출력
      // TS -> 텍스트 레코드에 포함될 목적 코드 시작주소
      // LOCCTR를 대입하는것은 현재 RESW가 할당받는 공간을 넘어서 주소가 시작되어야 하므로...
      text_record_start = ct->addr;
    }
    else if (strcmp(ct->operator, "BYTE")==0)
    {
      if (ct->operand[0][0] == 'C') { // 지정한 문자로 초기화되는 바이트 수
        n = 0;
        while (ct->operand[0][n+2] != '\'') {
          object_code[n] = ct->operand[0][n+2]; // character를 byte로 환산
          n++;
        }
        object_code[n] = '\0';
        write_listing_line(n);
        return(n);
      }
      else if (ct->operand[0][0] == 'X') { // 지정한 16진수로 초기화되는 바이트 수
        int len=0;
        n = 0;
        while (ct->operand[0][n+2] != '\'') {
          object_code[len] = hexstr2dec(ct->operand[0][n+2])*16 + hexstr2dec(ct->operand[0][n+3]); // 16진수 두 글자를 1 byte로 환산
          n += 2;
          len++;
        }
        object_code[len] = '\0';
        write_listing_line(len);
        return(len);
      }
      else {
        sscanf(ct->operand[0], "%d", &n);
        object_code[0] = n;
        write_listing_line(n);
        return(1);
      }
    }
    else if (strcmp(ct->operator, "LTORG") == 0) {
      write_listing_line(format);
      n = write_literal(); // Literal Table(pool) 출력
      current_pool++;
      return(n);
    }
    else if (strcmp(ct->operator, "CSECT")==0)
    {
        text_record_start = 0; // TS -> 텍스트 레코드에 포함될 목적 코드 시작주소

        fprintf(listing_file, "\n");
        write_listing_line(format);

        sec++; // 제어섹션(control section) 번호 증가
        write_text_record(); // 이전 까지의 텍스트 레코드(Text record)의 출력
        while (mod_last < mod_record_count) { // 수정 레코드(Modification record)를 출력한다
        fprintf(object_code_file, "%s", mod_record[mod_last]);
        mod_last++;
        }
        if (sec == 1){ // 기본 섹션(default section)
         fprintf(object_code_file, "E%06X\n\n", FEI);} // 엔드 레코드(End record)
        else{
         fprintf(object_code_file, "E\n\n");}
        fprintf(object_code_file, "H%-6s%06X%06X\n", ct->label, 0, csect_table[sec].program_length);
    }
    else if (strcmp(ct->operator, "EQU") == 0) {
      line_num += 5;
        fprintf(listing_file, "%4d    %04X  %-30s ", line_num, ct->addr, input_data[token_line]); // 맨 앞에 Loc 출력
        fprintf(listing_file, "\n");
    }
    else if (strcmp(ct->operator, "EXTDEF") == 0) // 외부 정의(external definition)
    {
        for (int i = 0; i < MAX_OPERAND && ct->operand[i][0] != '\0'; ++i) {
            strcpy(extDef[extDefCount].symbol, ct->operand[i]);
            extDef[extDefCount].addr = search_symtab(ct->operand[i], sec);
            extDefCount++;
        }
        fprintf(object_code_file, "D"); // 정의 레코드(define record)
        for(int i = 0; i < MAX_OPERAND && ct->operand[i][0] != '\0'; ++i) {
        int a1 = search_extDtab(ct->operand[i]);
        fprintf(object_code_file, "%-6s%06X", ct->operand[i], a1);
        } // 현 제어 섹션에서 정의된 외부기호이름,상대주소
        line_num += 5;
        fprintf(listing_file, "%4d          %-30s ", line_num, input_data[token_line]);
        fprintf(object_code_file, "\n");
        fprintf(listing_file, "\n");
    }
    else if (strcmp(ct->operator, "EXTREF") == 0) // 외부 참조(external reference)
    {
        fprintf(object_code_file, "R"); // 참조 레코드(Refer record)
        for(int i = 0; i < MAX_OPERAND && ct->operand[i][0] != '\0'; ++i) {
        fprintf(object_code_file, "%-6s", ct->operand[i]);
        }

        line_num += 5;
        fprintf(listing_file, "%4d          %-30s", line_num, input_data[token_line]);
        fprintf(object_code_file, "\n");
        fprintf(listing_file, "\n");
    }
  }
  
  return (format);
}

// Helper function to write a Text record to the object program
void write_text_record() {
    if(text_record_ctr > 0){
    fprintf(object_code_file, "T%06X%02X%s\n", text_record_start, text_record_ctr / 2, text_record);
    }
}

int write_listing_line(int format) {
    line_num += 5; // 행번호
    token *current_token = token_table[token_line];
    token *nt = token_table[token_line + 1];
  switch (format) {
    case 0: // 에셈블러 지시자
      if (current_token->label == NULL)
        fprintf(listing_file, "%4d          %-30s \n", line_num, input_data[token_line]);
      else
        fprintf(listing_file, "%4d    %04X  %-30s \n", line_num, current_token->addr, input_data[token_line]);
      break;

    case 1:
      if ((text_record_ctr + 2) > 60) {
        write_text_record();
        text_record_start = current_token->addr;
      }
      sprintf(text_record+text_record_ctr, "%02X", object_code[0]);
      text_record_ctr += 2;
      fprintf(listing_file, "%4d    %04X  %-30s     %02X\n", line_num, current_token->addr,
       input_data[token_line], object_code[0]);
      break;

    case 2: // 2형식
      if ((text_record_ctr + 4) > 60) {
        write_text_record(); // 이전 까지의 텍스트 레코드(Text record)의 출력
        // text_record_start -> 텍스트 레코드에 포함될 목적 코드 시작주소
        // current_token->addr를 대입하는것은 현재 명령어가 다음 텍스트 레코드의 시작주소이므로...
        text_record_start = current_token->addr;
      }
      sprintf(text_record+text_record_ctr, "%02X%02X", object_code[0], object_code[1]);
      text_record_ctr += 4;

      fprintf(listing_file,"%4d    %04X  %-30s ", line_num, current_token->addr,
       input_data[token_line]);

      fprintf(listing_file," %02X%02X\n", object_code[0], object_code[1]);
      break;

    case 3: // 3형식
      if ((text_record_ctr + 6) > 60) {
        write_text_record(); // 이전 까지의 텍스트 레코드(Text record)의 출력
        // text_record_start -> 텍스트 레코드에 포함될 목적 코드 시작주소
        // current_token->addr를 대입하는것은 현재 명령어가 다음 텍스트 레코드의 시작주소이므로...
        text_record_start = current_token->addr;
      }
      sprintf(text_record+text_record_ctr, "%02X%02X%02X", object_code[0], object_code[1], object_code[2]);
      text_record_ctr += 6;
      fprintf(listing_file,"%4d    %04X  %-30s ", line_num, current_token->addr, input_data[token_line]);

      fprintf(listing_file," %02X%02X%02X\n", object_code[0], object_code[1], object_code[2]);
      break;

    case 4: // 4형식
      if ((text_record_ctr + 8) > 60) {
        write_text_record(); // 이전 까지의 텍스트 레코드(Text record)의 출력
        // text_record_start -> 텍스트 레코드에 포함될 목적 코드 시작주소
        // current_token->addr를 대입하는것은 현재 명령어가 다음 텍스트 레코드의 시작주소이므로...
        text_record_start = current_token->addr;
      }
      sprintf(text_record+text_record_ctr, "%02X%02X%02X%02X", object_code[0], object_code[1], object_code[2], object_code[3]);
      text_record_ctr += 8;
      fprintf(listing_file,"%4d    %04X  %-30s ", line_num, current_token->addr , input_data[token_line]);
      
      fprintf(listing_file," %02X%02X%02X%02X\n", object_code[0], object_code[1], object_code[2], object_code[3]);
      break;
  }
}


// Function to initialize a Text record
void initialize_text_record() {
    text_record_ctr = 0;
    text_record[0] = '\0';
}

int assem_pass2()
{
    int i;
    line_num = 0;
    LT_num = 0;
    token_line = 0;
    inst_index = 0;
    sec = 0;
    // Initialize the object code output and listing files
    make_objectcode_output("objectcode.txt", "listing.txt");
    initialize_text_record();

    token *fl = token_table[0];
    if (strcmp(fl->operator, "START") == 0) {
        starting_address = strtol(fl->operand[0], NULL, 16);
        write_listing_line(0); // Lst 파일의 출력 시작
        // Obj 파일의 출력 시작(Header)
        fprintf(object_code_file, "H%-6s%06X%06X\n", fl->label, starting_address, csect_table[sec].program_length);
        token_line++;
    } else {
        printf("Error: START directive not found.\n");
    }

    while (1) {
    // 리스트, 목적코드 출력
	    handlepass2();

        if (strcmp(token_table[token_line]->operator, "END") == 0) {
            write_listing_line(0);
            write_text_record(); // 이전 까지의 텍스트 레코드(Text record)의 출력

            text_record_start = token_table[token_line]->addr; // TS -> 텍스트 레코드에 포함될 목적 코드 시작주소
            i = write_literal(); // Literal Table(pool) 출력
            write_text_record(); // Literal Table(pool) 에 대한 텍스트 레코드(Text record)의 출력

            while (mod_last < mod_record_count) { // 수정 레코드(Modification record)를 출력한다
                fprintf(object_code_file, "%s", mod_record[mod_last]);
                mod_last++;
            }
            if (sec == 1){ // 기본 섹션(default section)
                fprintf(object_code_file, "E%06X\n\n", FEI);} // 엔드 레코드(End record)
            else{
                fprintf(object_code_file, "E\n\n");}
            break;
        }
    token_line++;
    }

  fclose(listing_file);
  fclose(object_code_file);
  return (0);
}


