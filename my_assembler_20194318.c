#include "my_assembler_20194318.h"

// Function to add a symbol to the symbol table
void add_to_symtab(const uchar *label, int loc, int is_equ, int sec) {
    // Check for duplicate symbol
    for (int i = 0; i < sym_index; i++) {
        if (strcmp(sym_table[i].symbol, label) == 0 && sym_table[i].sec == sec) {
            if (is_equ) {
                fprintf(stderr, "Error: Duplicate symbol found - %s\n", label);
                // Handle the error as needed
                return;
            }
            
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
}

// Function to search for a literal in the literal table
int search_literal(uchar *operand) {
    for (int i = 0; i < LT_num; ++i) {
        if (strcmp(LTtab[i].name, operand) == 0) {
            // Literal found, return literal index
            return i;
        }
    }
    // Literal not found
    return -1;
}

// Function to calculate the length of a BYTE or WORD operand
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
    // Handle BYTE literals with C or X prefixes
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

// Function to search for a symbol in the symbol table
int search_symtab(uchar *symbol, int section) {
    for (int i = 0; i < MAX_LINES; ++i) {
        // Check if the symbol and section match a table entry
        if ((strcmp(sym_table[i].symbol, symbol) == 0) && (sym_table[i].sec == section)) {
            // Symbol found, return its address
            return sym_table[i].addr;
        }
    }
    // Symbol not found in the symbol table for the specified section
    return -1;
}

// Function to search for an external reference in the external reference table
int search_extRtab(uchar *symbol, int section) {
    for (int i = 0; i < MAX_EXTREF; ++i) {
        // Check if the symbol and section match an external reference entry
        if ((strcmp(extRef[i].symbol, symbol) == 0) && (extRef[i].sec == section)) {
            // External reference found, return its address
            return extRef[i].addr;
        }
    }
    // External reference not found, report an error
    printf("Error: Undefined external reference - %s\n", symbol);
    return -1;
}

// Function to search for an external definition in the external definition table
int search_extDtab(uchar *symbol) {
    for (int i = 0; i < MAX_EXTDEF; ++i) {
        // Check if the symbol matches an external definition entry
        if (strcmp(extDef[i].symbol, symbol) == 0) {
            // External definition found, return its address
            return extDef[i].addr;
        }
    }
    // External definition not found
    return -1;
}


// Function to initialize the assembler by loading instruction data and source code
int init_my_assembler(void) {
    // Initialize instruction data
    if (init_inst_file("inst.data") == -1) {
        fprintf(stderr, "Error: Instruction Initiation Failed\n");
        return -1;
    }

    // Initialize source code file ("input.txt")
    if (init_input_file("input.txt") == -1) {
        fprintf(stderr, "Error: Source Code Initiation Failed\n");
        return -1;
    }

    // Generate opcode output file ("optab.txt")
    make_opcode_output("optab.txt");

    // Initialize the token table
    init_token_table();

    return 0;
}

// Function to initialize the instruction data from a file
int init_inst_file(uchar *inst_file) {
    FILE *file = fopen(inst_file, "r");

    if (file == NULL) {
        perror("Error opening inst.data file");
        return -1;
    }

    char line[100];  // Assuming each line in inst.data is not longer than 100 characters

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

// Function to initialize the source code from a file
int init_input_file(uchar *input_file) {
    FILE *input_fp = fopen(input_file, "r");
    if (input_fp == NULL) {
        fprintf(stderr, "Error opening input file: %s\n", input_file);
        return -1;
    }

    char line[100];
    int line_index = 0;

    while (fgets(line, sizeof(line), input_fp) != NULL) {
        // Remove trailing newline character
        line[strcspn(line, "\n")] = '\0';

        // Check if the line starts with a dot (comment)
        if (line[0] != '.') {
            // Save the input into input_data
            input_data[line_index] = strdup(line);

            // Move to the next line
            line_index++;
        }
    }

    // Save the total line number
    line_num = line_index;

    printf("Input Data:\n");
    for (int i = 0; i < line_num; i++) {
        printf("%d: %s\n", i + 1, input_data[i]);
    }
    printf("Total Lines: %d\n", line_num);

    fclose(input_fp);
    return 0;
}

// Function to initialize the token table
int init_token_table(void) {
    // Initialize each element in the token table
    for (int i = 0; i < MAX_LINES; i++) {
        token_table[i] = malloc(sizeof(token));
        if (token_table[i] == NULL) {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            return -1;
        }
        // Initialize fields to default values
        token_table[i]->label = NULL;
        token_table[i]->operator = NULL;
        for (int j = 0; j < MAX_OPERAND; j++) {
            token_table[i]->operand[j][0] = '\0';
        }
        token_table[i]->comment[0] = '\0';
    }
    return 0;
}

// Function to parse a line of code into tokens and store them in the token table
int token_parsing(uchar *str) {
    // Initialize memory for the token entry
    token_table[token_line] = malloc(sizeof(token));
    if (token_table[token_line] == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return -1;
    }

    // Create a duplicate of the input string for tokenization
    uchar *scopy = strdup((uchar *)str);

    // Tokenize using whitespace or tab characters
    uchar *token_str = strtok(scopy, " \t");

    // Check if the first token is an operator
    int is_operator = search_opcode(token_str);

    if (is_operator >= 0) {
        // If the first token is an operator
        token_table[token_line]->label = NULL;
        token_table[token_line]->operator = strdup(token_str);
        token_str = strtok(NULL, " \t"); // Skip parsing the operator
    } else {
        // If the first token is a label
        token_table[token_line]->label = strdup(token_str);
        token_str = strtok(NULL, " \t\n"); // Move to the next token

        // Parsing the operator
        if (token_str != NULL) {
            token_table[token_line]->operator = strdup(token_str);
            token_str = strtok(NULL, " \t\n"); // Move to the next token
        } else {
            fprintf(stderr, "Error: Operator missing.\n");
            return -1;
        }
    }

    // Parsing operands
    int operand_index = 0;
    while (token_str != NULL) {
        if (operand_index < MAX_OPERAND) {
            // Check if there are multiple operands separated by commas
            if (strchr(token_str, ',')) {
                char *operand_token = strtok(token_str, ",");
                while (operand_token != NULL) {
                    strcpy((char *)token_table[token_line]->operand[operand_index], operand_token);
                    operand_token = strtok(NULL, ",\t");
                    operand_index++;
                }
            } else {
                // If there is no operand, set '\0' in the first element of the operand array
                if (token_str != NULL) {
                    strcpy((char *)token_table[token_line]->operand[operand_index], token_str);
                    operand_index++;
                } else {
                    token_table[token_line]->operand[operand_index][0] = '\0';
                }
                token_str = strtok(NULL, "\t");
                break;
            }
        } else {
            fprintf(stderr, "Error: Too many operands.\n");
            free(scopy);
            return -1;
        }
        token_str = strtok(NULL, " \t\n"); // Move to the next token
    }

    // Free the duplicated string used for tokenization
    free(scopy);

    // Test output
    printf("Line %d\n", token_line + 1);
    printf("Label: %s\n", token_table[token_line]->label);
    printf("Operator: %s\n", token_table[token_line]->operator);
    printf("Operands: \n");
    for (int i = 0; i < operand_index; i++) {
        printf("  %s\n", token_table[token_line]->operand[i]);
    }

    return 0;
}

// Function to search for an opcode in the instruction table
int search_opcode(uchar *str) {
    for (int i = 0; i < inst_count; i++) {
        // Check if the given string matches the opcode mnemonic (with or without the '+' prefix)
        if (strcmp(str, inst_table[i]->str) == 0 || (str[0] == '+' && strcmp(str + 1, inst_table[i]->str) == 0)) {
            // Test print the searched operator information on the terminal
            printf("Operator: %s / ", inst_table[i]->str);
            printf("Format: %d / ", inst_table[i]->format);
            printf("Opcode Value: 0x%02X\n", inst_table[i]->op);
            return i; // Return the index of the found opcode in the instruction table
        }
    }

    // Opcode not found, report an error
    fprintf(stderr, "Error: Opcode not found for %s\n", str);
    return -1;
}

// Function to handle EXTDEF directive
void handle_extdef(uchar *symbol) {
    // Check for duplicates before adding
    for (int i = 0; i < MAX_EXTDEF; i++) {
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
    for (int i = 0; i < MAX_EXTREF; i++) {
        if (strcmp(extRef[i].symbol, symbol) == 0 && extRef[i].sec == section) {
            fprintf(stderr, "Error: Duplicate EXTREF found - %s\n", symbol);
            // Handle the error as needed
            return;
        }
    }
    // Add the symbol to the EXTREF table
    extRef[extRefCount].sec = section;
    extRef[extRefCount].addr = locctr;
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

// Function for the first pass of the assembler
static int assem_pass1(void) {
    // Initialize various variables and data structures
    locctr = 0;
    starting_address = 0;
    csect_start_address = 0;
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

    // Initialize the literal table
    for (int i = 0; i < MAX_LITERALS; i++) {
        LTtab[i].name[0] = '\0';
        LTtab[i].addr = -1;
        LTtab[i].leng = -1;
    }

    // Initialize the control section table
    for (int i = 0; i < MAX_CSECT; ++i) {
        csect_table[i].sec = -1;
        csect_table[i].program_length = 0;
    }

    // Initialize the external definition and reference tables
    for (int i = 0; i < MAX_EXTDEF; i++) {
        extDef[i].addr = -1;
        extDef[i].sec = -1;
        extDef[i].symbol[0] = '\0';
        extRef[i].addr = -1;
        extRef[i].sec = -1;
        extRef[i].symbol[0] = '\0';
    }

    // Read the first input line and perform token parsing
    uchar *current_line = input_data[0];
    token_parsing(current_line);

    // Check for START directive
    if (strcmp(token_table[0]->operator, "START") == 0) {
        // Save #[OPERAND] as the starting address
        starting_address = strtol(token_table[0]->operand[0], NULL, 16);

        // Initialize LOCCTR to the starting address
        locctr = starting_address;

        // Write the line to the intermediate file
        token_table[token_line]->addr = locctr;
        write_intermediate_file(current_line, locctr);

        token_line++;

    } else {
        // If no START directive, initialize LOCCTR to 0
        locctr = 0;
        token_table[token_line]->addr = locctr;
        write_intermediate_file(current_line, locctr);
    }

    // Process lines until the OPCODE is 'END'
    while (token_line < line_num) {
        current_line = input_data[token_line];
        token_parsing(current_line);

        if (token_table[token_line] != NULL) {

            // Check for CSECT directive
            if (strcmp(token_table[token_line]->operator, "CSECT") == 0) {
                for (int i = 2; i < token_line; i++) {
                    if (locctr < token_table[token_line - i]->addr) {
                        csect_table[sec].program_length = token_table[token_line - i]->addr - csect_start_address;
                        break;
                    } else {
                        csect_table[sec].program_length = locctr - csect_start_address;
                        break;
                    }
                }
                csect_table[sec].sec = sec;
                // Start a new section, reset the program counter
                locctr = starting_address;
                csect_start_address = locctr;
                sec++;

                // Add the section name to the symbol table
                if (token_table[token_line]->label != NULL) {
                    add_to_symtab(token_table[token_line]->label, locctr, 0, sec);
                }
                // Write the line to the intermediate file
                write_intermediate_file(current_line, locctr);
                token_table[token_line]->addr = locctr;
                token_line++;
                continue;  // Skip the rest of the loop for CSECT
            }
            // Check for EQU directive
            if (strcmp(token_table[token_line]->operator, "EQU") == 0) {
                handle_equ_directive(token_table[token_line]->label, token_table[token_line]->operand[0]);
            }
            // Check for EXTREF directive
            if (strcmp(token_table[token_line]->operator, "EXTREF") == 0) {
                // Process EXTREF directive
                for (int i = 0; i < MAX_OPERAND && token_table[token_line]->operand[i][0] != '\0'; i++) {
                    handle_extref(token_table[token_line]->operand[i], sec);
                }
            }
            // Add the label to the symbol table
            if (token_table[token_line]->label != NULL) {
                add_to_symtab(token_table[token_line]->label, locctr, 0, sec);
            }

            // Write the line to the intermediate file for other directives and instructions
            write_intermediate_file(current_line, locctr);
            token_table[token_line]->addr = locctr;

            // Search for the opcode index in the instruction table
            int opcode_index = search_opcode(token_table[token_line]->operator);

            if (opcode_index != -1) {
                // Check for 4-format instruction
                if (token_table[token_line]->operator[0] == '+') {
                    locctr += 4;
                } else {
                    locctr += inst_table[opcode_index]->format;
                }

                // Handle specific directives that affect the location counter
                if (strcmp(token_table[token_line]->operator, "WORD") == 0) {
                    locctr += 3;
                } if (strcmp(token_table[token_line]->operator, "RESW") == 0) {
                    locctr += 3 * atoi(token_table[token_line]->operand[0]);
                } if (strcmp(token_table[token_line]->operator, "RESB") == 0) {
                    locctr += atoi(token_table[token_line]->operand[0]);
                } if (strcmp(token_table[token_line]->operator, "BYTE") == 0) {
                    locctr += calculate_byte_length(token_table[token_line]->operand[0]);
                } if (strcmp(token_table[token_line]->operator, "LTORG") == 0) {
                    // Handle LTORG directive
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
                int length;
                uchar *operand = token_table[token_line]->operand[i];
                if (operand[0] == '=' && (operand[1] == 'C' || operand[1] == 'X')) {
                    // Found a literal
                    length = calculate_byte_length(operand);
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
        // Read Next
        token_line++;
    }

    // Set the program length for the last control section
    csect_table[sec].program_length = locctr;

    // Generate the symbol table output
    make_symtab_output("symtab.txt");

    // Print the addresses for each line for debugging
    for (int i = 0; i < line_num; i++) {
        printf("Line: %d\n Locctr: %04X\n", i + 1, token_table[i]->addr);
    }

    return 0;
}

// Function to generate the opcode output file
void make_opcode_output(uchar *file_name) {
    FILE *output_file = fopen(file_name, "w");

    // Check if the file is opened successfully
    if (output_file == NULL) {
        fprintf(stderr, "Error opening output file.\n");
        return;
    }

    // Output header to the file
    fprintf(output_file, "Mnemonic  MachineCode Format\n");

    // Iterate through the instruction table to print information
    for (int i = 0; i < inst_index; i++) {
        fprintf(output_file, "%-10s %02X %10d\n",
                inst_table[i]->str, inst_table[i]->op, inst_table[i]->format);
    }

    // Close the output file
    fclose(output_file);
}

// Function to generate the symbol table output file
void make_symtab_output(uchar *file_name) {
    FILE *symtab_output_file = fopen(file_name, "w");

    // Check if the file is opened successfully
    if (symtab_output_file == NULL) {
        fprintf(stderr, "Error opening %s for writing.\n", file_name);
        exit(EXIT_FAILURE);
    }

    // Output header for symbol table to the file
    fprintf(symtab_output_file, "Symbol\tAddress\tSection\n");
    fprintf(symtab_output_file, "-------------------------\n");

    // Iterate through the symbol table to print information
    for (int i = 0; i < sym_index; i++) {
        if (sym_table[i].addr != -1) {
            fprintf(symtab_output_file, "%s\t%04X\t%d\n", sym_table[i].symbol, sym_table[i].addr, sym_table[i].sec);
        }
    }

    // Output header for literal table to the file
    fprintf(symtab_output_file, "\nLiteral\tAddress\tPoolNum\n");
    fprintf(symtab_output_file, "----------------------------\n");

    // Iterate through the literal table to print information
    for (int i = 0; i < LT_num; i++) {
        if (LTtab[i].addr != -1) {
            fprintf(symtab_output_file, "%s\t%04X\t%d\n", LTtab[i].name, LTtab[i].addr, LTtab[i].value);
        }
    }

    // Close the symbol table output file
    fclose(symtab_output_file);
}

// Function to write an entry to the intermediate file with the given string and location counter (locctr)
void write_intermediate_file(uchar *str, int locctr) {
    FILE *intermediate_file;

    // Check if this is the first write to the file
    if (is_first_write) {
        intermediate_file = fopen("intermediate.txt", "w");
        if (intermediate_file == NULL) {
            fprintf(stderr, "Error: Unable to open intermediate file for writing.\n");
            return;
        }
        is_first_write = 0; // Update the flag indicating the first write
    } else {
        // If not the first write, open the file in append mode
        intermediate_file = fopen("intermediate.txt", "a");
        if (intermediate_file == NULL) {
            fprintf(stderr, "Error: Unable to open intermediate file for writing.\n");
            return;
        }
    }

    // Write the formatted entry to the intermediate file
    fprintf(intermediate_file, "%04X %s\n", locctr, str);

    // Close the intermediate file
    fclose(intermediate_file);
}

// Function to evaluate an expression given in the form of a string (expr)
int evaluate_expression(uchar *expr) {
    sign = '\0'; // Initialize the sign variable

    // Check for the presence of '+' or '-' in the expression
    if (strchr(expr, '+')) {
        sign = '+'; // Set the sign to '+'
    } else if (strchr(expr, '-')) {
        sign = '-'; // Set the sign to '-'
    }

    // Tokenize the expression based on the '-' or '+' operators
    uchar *token = strtok(expr, "-+");
    
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

    // Evaluate the expression based on the sign
    if (sign == '+') {
        return operand1 + operand2;
    } else if (sign == '-') {
        return operand1 - operand2;
    }
}

// Function to open the object code output file and the listing file for writing
void make_objectcode_output(uchar *file_name, uchar *list_name) {
    // Check if it is the first write
    if (first_write) {
        // Open the object code output file in write mode
        object_program_file = fopen(file_name, "w");
        // Open the listing file in write mode
        listing_file = fopen(list_name, "w");

        // Check if both files are successfully opened
        if (object_program_file == NULL || listing_file == NULL) {
            fprintf(stderr, "Error: Unable to open objfile and lstfile for writing.\n");
            // Close the files if there is an error
            fclose(object_program_file);
            fclose(listing_file);
            exit(1); // Exit the program with an error code
        }

        // Update the first write flag
        first_write = 0;
    } else {
        // If not the first write, open the object code output file and listing file in append mode
        object_program_file = fopen(file_name, "a");
        listing_file = fopen(list_name, "a");

        // Check if both files are successfully opened
        if (object_program_file == NULL || listing_file == NULL) {
            fprintf(stderr, "Error: Unable to open objfile and lstfile for writing.\n");
            // Close the files if there is an error
            fclose(object_program_file);
            fclose(listing_file);
            exit(1); // Exit the program with an error code
        }
    }
}

// Function to get the numerical value of a register
int getREGnum(uchar *register_name) {
    // Compare the register name and return the corresponding numerical value
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
        exit(EXIT_FAILURE); // Exit the program with an error code
    }
}

// Function to generate object code based on the instruction format
int generate_object_code(int format) {
    int dxx;    // Variables for temporary storage
    int disp;
    int loc;
    int loc2;
    int op_index = search_opcode(token_table[token_line]->operator); // Get the index of the opcode in the instruction table
    int opcode = inst_table[op_index]->op; // Get the opcode value
    token *ct = token_table[token_line]; // Current token
    token *nt = token_table[token_line + 1]; // Next token

    switch (format) {
        case 1: // Format 1 instructions (FIX, FLOAT, HIO, NORM, SIO, TIO)
            object_code[0] = opcode;
            object_code[1] = '\0';
            break;

        case 2: // Format 2 instructions (operand can be r1 or r1,r2 or n or r1,n)
            object_code[0] = opcode;
            if (ct->operand[0][0] == '\0')
                printf("Error! Operand expected\n");

            if (ct->operand[0][0] < 'A') { // Operand is a constant (n)
                sscanf(ct->operand[0], "%d", &dxx);
                object_code[1] = dxx << 4; // n
            } else {
                object_code[1] = getREGnum(ct->operand[0]) << 4; // r1
            }

            if (ct->operand[1][0] != '\0') {
                if (ct->operand[1][0] < 'A') { // Operand 2 is a constant (n)
                    sscanf(ct->operand[1]+1, "%d", &dxx);
                    object_code[1] = object_code[1] | dxx; // n
                } else {
                    object_code[1] = object_code[1] | getREGnum(ct->operand[1]); // r2
                }
            }
            break;

        case 3: // Format 3 instructions
            switch (ct->operand[0][0]) {
                case '#': // Immediate addressing
                    object_code[0] = opcode + 1; // n=0, i=1, so +1
                    if (ct->operand[0][1] >= 'A') { // PC-relative + immediate addressing (e.g., #LENGTH)
                        dxx = search_symtab(ct->operand[0]+1, sec);
                        if (dxx == -1)
                            printf("Error: Undefined symbol: %s\n", ct->operand[0]+1);
                        else
                            dxx -= nt->addr; // Difference between op1 and the next instruction's LOCCTR
                        object_code[1] = (dxx >> 8) & 15; // TA upper 4 bits
                        object_code[1] = object_code[1] | 32; // p=1 (PC-relative)
                        object_code[2] = dxx & 255; // TA lower 8 bits
                    } else { // Constant value
                        sscanf(ct->operand[0]+1, "%d", &dxx);
                        object_code[1] = (dxx >> 8) & 15; // TA upper 4 bits
                        object_code[2] = dxx & 255; // TA lower 8 bits
                    }
                    break;

                case '\0': // No operand (RSUB)
                    object_code[0] = opcode + 3; // n=1, i=1, so +3
                    object_code[1] = 0;
                    object_code[2] = 0;
                    break;

                default: // Other cases
                    if (ct->operand[0][0] == '@') { // Indirect addressing
                        object_code[0] = opcode + 2; // n=1, i=0, so +2
                        loc = search_symtab(ct->operand[0]+1, sec);
                        disp = loc - nt->addr; // Difference between op1 and the next instruction's LOCCTR
                    } else if (ct->operand[0][0] == '=') { // Literal
                        object_code[0] = opcode + 3; // n=1, i=1, so +3
                        loc = LTtab[current_pool-1].addr; // Search in the Literal Table (pool)
                        disp = loc - nt->addr; // Difference between op1 and the next instruction's LOCCTR
                    } else {
                        object_code[0] = opcode + 3; // n=1, i=1, so +3
                        loc = search_symtab(ct->operand[0], sec);
                        disp = loc - nt->addr; // Difference between op1 and the next instruction's LOCCTR
                    }

                    if (loc < 0) {
                        printf("Error: Undefined symbol: %s\n", ct->operand[0]);
                        loc = 0;
                        object_code[1] = 0;
                        object_code[2] = 0;
                        break;
                    }

                    if ((abs(disp) >= 4096) && (loc >= 0)) { // Use base-relative addressing
                        disp = abs(BASEADDR - loc);
                        object_code[1] = (disp >> 8) & 15; // TA upper 4 bits
                        object_code[1] = object_code[1] | 64; // b=1 (base-relative)
                        object_code[2] = disp & 255; // TA lower 8 bits
                        printf("opcode->%s base->%X loc->%X disp->%d\n", inst_table[op_index]->str, BASEADDR, loc, disp);
                    } else if ((disp < 4096) && (loc >= 0)) {
                        object_code[1] = (disp >> 8) & 15; // TA upper 4 bits
                        object_code[1] = object_code[1] | 32; // p=1 (PC-relative)
                        object_code[2] = disp & 255; // TA lower 8 bits
                    } else {
                        int ddd;
                        if (ct->operand[0][0] == '@') // Indirect addressing
                            sscanf(ct->operand[0]+1, "%d", &ddd);
                        else
                            sscanf(ct->operand[0], "%d", &ddd);
                        object_code[1] = (ddd >> 8); // TA upper 4 bits
                        object_code[2] = ddd & 255; // TA lower 8 bits
                    }

                    if (ct->operand[1][0] == 'X') // Indexing
                        object_code[1] = object_code[1] + 128; // x=1
            }
            printf("<Format 3> opcode->%s obj->%02X%02X%02X\n", inst_table[op_index]->str, object_code[0], object_code[1], object_code[2]);
            break;

        case 4: // Format 4
            switch (ct->operand[0][0]) {
                case '#': // Immediate addressing
                    object_code[0] = opcode + 1; // n=0, i=1, so +1
                    if (ct->operand[0][1] >= 'A') { // PC-relative + immediate addressing (e.g., #LENGTH)
                        dxx = search_symtab(ct->operand[0]+1, sec);
                        if (dxx == -1)
                            printf("Error: Undefined symbol: %s\n", ct->operand[0]+1);
                        else
                            dxx -= nt->addr; // Difference between op1 and the next instruction's LOCCTR
                    } else  // Constant value
                        sscanf(ct->operand[0]+1, "%d", &dxx);
                    object_code[1] = 16; // e=1
                    object_code[2] = (dxx >> 8) & 255; // TA upper 1 byte
                    object_code[3] = dxx & 255; // TA lower 1 byte
                    break;

                default:
                    if (ct->operand[0][0] == '@') { // Indirect addressing
                        object_code[0] = opcode + 2; // n=1, i=0, so +2
                        loc2 = search_symtab(ct->operand[0]+1, sec);
                    } else {
                        object_code[0] = opcode + 3; // n=1, i=1, so +3
                        loc2 = search_symtab(ct->operand[0], sec);
                        if (loc2 < 0) { // Not found in the current section
                            loc2 = search_extRtab(ct->operand[0], sec); // Check if it's an external reference
                            if (loc2 >= 0) {
                                // Add to Modification record
                                // L2-STARTADDR+1 -> Address of the field to be modified relative to the beginning of the current control section
                                // 05 -> Length of the record expressed in half bytes (fixed to 05 here)
                                // + -> Modification flag (fixed to + here)
                                sprintf(mod_record[mod_record_count], "M%06X05+%-6s\n", ct->addr-csect_start_address+1, ct->operand[0]);
                                mod_record_count++;
                            }
                        }
                        if (loc2 < 0) {
                            printf("Error: Undefined symbol: %s\n", ct->operand[0]);
                            loc2 = 0;
                        }
                    }

                    if (loc2 >= 0) {
                        object_code[1] = 16; // e=1
                        object_code[2] = (loc2 >> 8); // TA upper 1 byte
                        object_code[3] = loc2 & 255;  // TA lower 1 byte
                    } else {
                        int ddd;
                        if (ct->operand[0][0] == '@') // Indirect addressing
                            sscanf(ct->operand[0]+1, "%d", &ddd);
                        else
                            sscanf(ct->operand[0], "%d", &ddd);
                        object_code[1] = 16; // e=1
                        object_code[2] = (ddd >> 8); // TA upper 1 byte
                        object_code[3] = ddd & 255;  // TA lower 1 byte
                    }

                    if (ct->operand[1][0] == 'X') // Indexing
                        object_code[1] = object_code[1] + 128; // x=1
            }
            printf("<Format 4> opcode->%s obj->%02X%02X%02X%02X\n", inst_table[op_index]->str, object_code[0], object_code[1], object_code[2], object_code[3]);
            break;
    }
}


// Function to convert a hexadecimal character to its decimal equivalent
int hexstr2dec(char H) {
    int i;
    for (i = 0; i <= 15; i++)
        if (HEXTAB[i] == H)
            return (i);
    return (-1);
}

// Function to write literals to the object code
int write_literal(void) {
    int n, len = 0;
    is_lt = 0; // Flag to indicate if the current instruction involves a literal
    token *ct = token_table[token_line]; // Current token

    // Loop through literals in the literal table
    for (; (LT_num < current_pool) && (LTtab[LT_num].value == LT_num); LT_num++) {
        len = 0; // Initialize the length of the literal

        // Check the type of literal (character or hexadecimal)
        if (LTtab[LT_num].name[1] == 'C') { // Character literal
            n = 0;
            // Extract characters within the quotes and convert to bytes
            while (LTtab[LT_num].name[n + 3] != '\'') {
                object_code[len] = LTtab[LT_num].name[n + 3]; // Convert character to byte
                n++;
                len++;
            }
            object_code[len] = '\0'; // Null-terminate the string
        } else if (LTtab[LT_num].name[1] == 'X') { // Hexadecimal literal
            n = 0;
            // Extract hexadecimal pairs and convert to bytes
            while (LTtab[LT_num].name[n + 3] != '\'') {
                // Convert two hexadecimal characters to one byte
                object_code[len] = hexstr2dec(LTtab[LT_num].name[n + 3]) * 16 + hexstr2dec(LTtab[LT_num].name[n + 4]);
                n += 2;
                len++;
            }
            object_code[len] = '\0'; // Null-terminate the string
        } else {
            // If not a character or hexadecimal literal, assume it is a decimal literal
            sscanf(ct->operand[0] + 1, "%d", object_code);
            len++;
        }

        is_lt++; // Set the literal flag
        write_listing_line(len); // Write the literal to the listing file
        is_lt = 0; // Reset the literal flag
    } // for

    return (len); // Return the total length of literals written
}


// Function to handle the second pass of the assembler
int handle_pass2(void) {
    int n = 0; // Variable to store the length of literals or constants
    token *ct = token_table[token_line]; // Current token
    token *nt = token_table[token_line + 1]; // Next token
    inst_index = search_opcode(ct->operator); // Index of the opcode in the instruction table
    int format = inst_table[inst_index]->format; // Length of the instruction (length of directives is 0)

    // Check if the instruction is of format 4
    if (ct->operator[0] == '+') {
        format = 4;
    }

    // If the format is greater than 0, it is not an assembler directive
    if (format > 0) {
        generate_object_code(format); // Generate the object code based on the instruction format
        write_listing_line(format); // Write the listing line for the instruction
    }

    // If the format is 0, it is an assembler directive
    if (format == 0) {
        if (strcmp(ct->operator, "BASE") == 0) { // Set the base register value for relative addressing
            write_listing_line(format);
            BASEADDR = search_symtab(ct->operand[0], sec); // Search the symbol table for the base address
            if (BASEADDR < 0)
                BASEADDR = 0;
        } else if (strcmp(ct->operator, "NOBASE") == 0) { // Release the base register value
            write_listing_line(format);
            BASEADDR = 0;
        } else if (strcmp(ct->operator, "WORD") == 0) { // Handle WORD directive
            if (ct->operand[0][0] > 'A') { // Symbol is used
                int W3, W4 = 0;
                W3 = search_symtab(ct->operand[0], sec); // Search the symbol table for the symbol
                // Handle external references
                if (W3 < 0) {
                    W3 = search_extRtab(ct->operand[0], sec); // Search external reference table for the symbol
                    if (W3 < 0) {
                        printf("Error: Undefined symbol: %s\n", ct->operand[0]);
                    }
                    // Handle expressions with + or -
                    if (strchr(ct->operand[0], '+') || strchr(ct->operand[0], '-')) {
                        if (strchr(ct->operand[0], '+')) {
                            sign2 = '+';
                        } else if (strchr(ct->operand[0], '-')) {
                            sign2 = '-';
                        }
                        int operand_index = 0;
                        uchar *optok = strtok(ct->operand[0], "+-");
                        while (optok != NULL) {
                            strcpy(ct->operand[operand_index], optok);
                            optok = strtok(NULL, ",\t");
                            operand_index++;
                        }
                        W3 = search_symtab(ct->operand[0], sec);
                        if (W3 < 0) {
                            W3 = search_extRtab(ct->operand[0], sec);
                            if (W3 < 0) {
                                printf("Error: Undefined symbol: %s, %S\n", ct->operand[0], ct->operand[1]);
                            }
                            sprintf(mod_record[mod_record_count], "M%06X06%c%-6s\n", ct->addr - csect_start_address, '+', ct->operand[0]);
                            mod_record_count++;
                        }
                        W4 = search_symtab(ct->operand[1], sec);
                        if (W4 < 0) {
                            W4 = search_extRtab(ct->operand[1], sec);
                            if (W4 < 0) {
                                printf("Error: Undefined symbol: %s, %S\n", ct->operand[0], ct->operand[1]);
                            }
                            sprintf(mod_record[mod_record_count], "M%06X06%c%-6s\n", ct->addr - csect_start_address, sign2, ct->operand[1]);
                            mod_record_count++;
                        }
                        if (sign2 == '+') {
                            W3 = W3 + W4;
                        }
                        if (sign2 == '-') {
                            W3 = W3 - W4;
                        }
                    }
                    // Write the constant value to memory
                    object_code[0] = (W3 >> 16) & 255;
                    object_code[1] = (W3 >> 8) & 255;
                    object_code[2] = W3 & 255;
                } else {
                    // Constant value
                    sscanf(ct->operand[0], "%d", &n);
                    object_code[0] = 0;
                    object_code[1] = (n >> 8) & 255;
                    object_code[2] = n & 255;
                }
                // Write to the listing file
                write_listing_line(3);
                return 3;
            }
        } else if (strcmp(ct->operator, "RESW") == 0) { // Handle RESW directive
            write_listing_line(format);
            write_text_record(); // Output the previous text record
            text_record_start = ct->addr; // Set the start address for the new text record
        } else if (strcmp(ct->operator, "RESB") == 0) { // Handle RESB directive
            write_listing_line(format);
            write_text_record(); // Output the previous text record
            text_record_start = ct->addr; // Set the start address for the new text record
        } else if (strcmp(ct->operator, "BYTE") == 0) { // Handle BYTE directive
            if (ct->operand[0][0] == 'C') { // Character literal
                n = 0;
                while (ct->operand[0][n + 2] != '\'') {
                    object_code[n] = ct->operand[0][n + 2]; // Convert character to byte
                    n++;
                }
                object_code[n] = '\0'; // Null-terminate the string
                write_listing_line(n);
                return n;
            } else if (ct->operand[0][0] == 'X') { // Hexadecimal literal
                int len = 0;
                n = 0;
                while (ct->operand[0][n + 2] != '\'') {
                    // Convert two hexadecimal characters to one byte
                    object_code[len] = hexstr2dec(ct->operand[0][n + 2]) * 16 + hexstr2dec(ct->operand[0][n + 3]);
                    n += 2;
                    len++;
                }
                object_code[len] = '\0'; // Null-terminate the string
                write_listing_line(len);
                return len;
            } else { // Numeric constant
                sscanf(ct->operand[0], "%d", &n);
                object_code[0] = n;
                write_listing_line(n);
                return 1;
            }
        } else if (strcmp(ct->operator, "LTORG") == 0) { // Handle LTORG directive
            text_record_start = ct->addr; // Set the start address for the text record
            write_listing_line(format);
            n = write_literal(); // Output the literal table (pool)
            current_pool++;
            return n;
        } else if (strcmp(ct->operator, "CSECT") == 0) { // Handle CSECT directive
            text_record_start = 0; // TS -> Start address for the text record
            csect_start_address = ct->addr; // Set the start address for the control section
            fprintf(listing_file, "\n");
            write_listing_line(format);

            sec++; // Increment the control section number
            write_text_record(); // Output the previous text record
            // Output modification records
            while (mod_last < mod_record_count) {
                fprintf(object_program_file, "%s", mod_record[mod_last]);
                mod_last++;
            }
            // Output the End record
            if (sec == 1) { // Default section
                fprintf(object_program_file, "E%06X\n\n", starting_address);
            } else {
                fprintf(object_program_file, "E\n\n");
            }
            // Output the Header record
            fprintf(object_program_file, "H%-6s%06X%06X\n", ct->label, 0, csect_table[sec].program_length);
        } else if (strcmp(ct->operator, "EQU") == 0) { // Handle EQU directive
            line_num += 5;
            fprintf(listing_file, "%4d    %04X  %-10s ", line_num, ct->addr, input_data[token_line]);
            fprintf(listing_file, "\n");
        } else if (strcmp(ct->operator, "EXTDEF") == 0) { // Handle EXTDEF directive (External Definitions)
            // Process each operand in the EXTDEF directive
            for (int i = 0; i < MAX_OPERAND && ct->operand[i][0] != '\0'; ++i) {
                handle_extdef(ct->operand[i]);
            }
            fprintf(object_program_file, "D"); // Define record
            // Output external symbols and their relative addresses
            for (int i = 0; i < MAX_OPERAND && ct->operand[i][0] != '\0'; ++i) {
                int a1 = search_extDtab(ct->operand[i]);
                fprintf(object_program_file, "%-6s%06X", ct->operand[i], a1);
            }
            line_num += 5;
            fprintf(listing_file, "%4d          %-10s ", line_num, input_data[token_line]);
            fprintf(object_program_file, "\n");
            fprintf(listing_file, "\n");
        } else if (strcmp(ct->operator, "EXTREF") == 0) { // Handle EXTREF directive (External References)
            fprintf(object_program_file, "R"); // Refer record
            // Output external symbols referenced in the current control section
            for (int i = 0; i < extRefCount; ++i) {
                if (extRef[i].sec == sec) {
                    fprintf(object_program_file, "%-6s", extRef[i].symbol);
                }
            }
            line_num += 5;
            fprintf(listing_file, "%4d          %-10s", line_num, input_data[token_line]);
            fprintf(object_program_file, "\n");
            fprintf(listing_file, "\n");
        }
    }

    return (format);
}

// Helper function to write a Text record to the object program
void write_text_record(void) {
    if (text_record_ctr > 0) { // Check if there is any content in the text record
        // Write the Text record to the object program file with the starting address, length, and content
        fprintf(object_program_file, "T%06X%02X%s\n", text_record_start, text_record_ctr / 2, text_record);
        text_record_ctr = 0; // Reset the text record counter for the next record
    }
}


// Function to write a listing line to the listing file based on the instruction format
// It handles different formats (0, 1, 2, 3, 4) and writes corresponding information to the listing file
int write_listing_line(int format) {
    line_num += 5; // Increment line number by 5

    // Get the current token and the next token in the token table
    token *current_token = token_table[token_line];
    token *next_token = token_table[token_line + 1];

    // Switch based on the instruction format
    switch (format) {
        case 0: // Assembler directive
            // Check if there is a label and format the listing line accordingly
            if (current_token->label == NULL) {
                fprintf(listing_file, "%4d          %-10s \n", line_num, input_data[token_line]);
            } else {
                fprintf(listing_file, "%4d    %04X  %-10s \n", line_num, current_token->addr, input_data[token_line]);
            }
            break;

        case 1: // Format 1
            // Check if a new text record needs to be started
            if ((text_record_ctr + 2) > 60) {
                write_text_record();
                text_record_start = current_token->addr;
            }
            // Update the text record with the object code
            sprintf(text_record + text_record_ctr, "%02X", object_code[0]);
            text_record_ctr += 2;

            // Check if there is a literal and format the listing line accordingly
            if (is_lt > 0) {
                fprintf(listing_file, "%4d    %04X  *  %s ", line_num, current_token->addr, LTtab[current_pool - 1].name);
            } else {
                fprintf(listing_file, "%4d    %04X  %-10s ", line_num, current_token->addr, input_data[token_line]);
            }
            // Write the object code to the listing file
            fprintf(listing_file, "%02X\n", object_code[0]);
            break;

        case 2: // Format 2
            // Check if a new text record needs to be started
            if ((text_record_ctr + 4) > 60) {
                write_text_record();
                text_record_start = current_token->addr;
            }
            // Update the text record with the object code
            sprintf(text_record + text_record_ctr, "%02X%02X", object_code[0], object_code[1]);
            text_record_ctr += 4;

            // Check if there is a literal and format the listing line accordingly
            if (is_lt > 0) {
                fprintf(listing_file, "%4d    %04X  *  %s ", line_num, current_token->addr, LTtab[current_pool - 1].name);
            } else {
                fprintf(listing_file, "%4d    %04X  %-10s ", line_num, current_token->addr, input_data[token_line]);
            }
            // Write the object code to the listing file
            fprintf(listing_file, " %02X%02X\n", object_code[0], object_code[1]);
            break;

        case 3: // Format 3
            // Check if a new text record needs to be started
            if ((text_record_ctr + 6) > 60) {
                write_text_record();
                text_record_start = current_token->addr;
            }
            // Update the text record with the object code
            sprintf(text_record + text_record_ctr, "%02X%02X%02X", object_code[0], object_code[1], object_code[2]);
            text_record_ctr += 6;

            // Check if there is a literal and format the listing line accordingly
            if (is_lt > 0) {
                fprintf(listing_file, "%4d    %04X  *  %s ", line_num, current_token->addr, LTtab[current_pool - 1].name);
            } else {
                fprintf(listing_file, "%4d    %04X  %-10s ", line_num, current_token->addr, input_data[token_line]);
            }
            // Write the object code to the listing file
            fprintf(listing_file, " %02X%02X%02X\n", object_code[0], object_code[1], object_code[2]);
            break;

        case 4: // Format 4
            // Check if a new text record needs to be started
            if ((text_record_ctr + 8) > 60) {
                write_text_record();
                text_record_start = current_token->addr;
            }
            // Update the text record with the object code
            sprintf(text_record + text_record_ctr, "%02X%02X%02X%02X", object_code[0], object_code[1], object_code[2], object_code[3]);
            text_record_ctr += 8;

            // Check if there is a literal and format the listing line accordingly
            if (is_lt > 0) {
                fprintf(listing_file, "%4d    %04X  *  %s ", line_num, current_token->addr, LTtab[current_pool - 1].name);
            } else {
                fprintf(listing_file, "%4d    %04X  %-10s ", line_num, current_token->addr, input_data[token_line]);
            }
            // Write the object code to the listing file
            fprintf(listing_file, "%02X%02X%02X%02X\n", object_code[0], object_code[1], object_code[2], object_code[3]);
            break;
    }
}

// Function to perform pass 2 of the assembler
int assem_pass2() {
    // Initialize variables
    line_num = 0;
    LT_num = 0;
    token_line = 0;
    inst_index = 0;
    sec = 0;

    // Initialize the object code output and listing files
    make_objectcode_output("objectprogram.txt", "list.txt");

    // Get the first token from the token table
    token *fl = token_table[0];

    // Check if the first instruction is START
    if (strcmp(fl->operator, "START") == 0) {
        // Extract the starting address from the operand and convert it to hexadecimal
        starting_address = strtol(fl->operand[0], NULL, 16);

        // Write the listing line for the START directive
        write_listing_line(0); // Start listing file output

        // Write the header record to the object program file
        fprintf(object_program_file, "H%-6s%06X%06X\n", fl->label, starting_address, csect_table[sec].program_length);

        // Move to the next token
        token_line++;
    } else {
        // Display an error if the START directive is not found
        printf("Error: START directive not found.\n");
    }

    // Process tokens until the END directive is encountered
    while (1) {
        // Process pass 2 for the current token
        handle_pass2();

        // Check if the current token is the END directive
        if (strcmp(token_table[token_line]->operator, "END") == 0) {
            // Write the listing line for the END directive
            write_listing_line(0);

            // Write the text record for the current section
            write_text_record();

            // Update the text record start address for literals
            text_record_start = token_table[token_line]->addr;

            // Write the literal pool to the text record
            write_literal();

            // Write the text record for the literal pool
            write_text_record();

            // Write modification records to the object program file
            while (mod_last < mod_record_count) {
                fprintf(object_program_file, "%s", mod_record[mod_last]);
                mod_last++;
            }

            // Write the END record to the object program file
            if (sec == 1) { // Default section
                fprintf(object_program_file, "E%06X\n\n", starting_address);
            } else {
                fprintf(object_program_file, "E\n\n");
            }

            // Break out of the loop as END directive is encountered
            break;
        }

        // Move to the next token
        token_line++;
    }

    // Close the listing and object program files
    fclose(listing_file);
    fclose(object_program_file);

    // Return 0 to indicate successful execution
    return (0);
}