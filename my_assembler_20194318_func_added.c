#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include "my_assembler_20194318.h"

void write_to_intermediate_file(FILE *intermediate_file) {
    if (intermediate_file == NULL) {
        fprintf(stderr, "Error: Intermediate file not initialized.\n");
        // Handle the error as needed
        return;
    }
    fprintf(intermediate_file, "%4d    %04X  %s\t%-10s\t", token_line * 5, locctr, token_table[token_line]->label, token_table[token_line]->operator);

    // Print operands
    for (int i = 0; i < MAX_OPERAND; i++) {
        if (token_table[token_line]->operand[i][0] != '\0') {
            fprintf(intermediate_file, "%s", token_table[token_line]->operand[i]);
            if (token_table[token_line]->operand[i + 1][0] != '\0') {
                fprintf(intermediate_file, ",");
            }
        }
    }

    // Print the rest of the fields
    fprintf(intermediate_file, "\t%-8s\n", token_table[token_line]->comment);
}


int search_symtab(uchar *symbol) {
    for (int i = 0; i < MAX_LINES; ++i) {
        if (strcmp(sym_table[i].symbol, symbol) == 0) {
            return i; // Symbol found, return its index
        }
    }
    return -1; // Symbol not found
}

void add_to_symtab(uchar *symbol, int address) {
    strcpy(sym_table[locctr].symbol, symbol);
    sym_table[locctr].addr = address;

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
