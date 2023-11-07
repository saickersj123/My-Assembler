#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include <malloc.h>
#include "my_assembler_20194318.h"

int init_my_assembler(void){
    init_inst_file("inst.data");
    init_input_file("input.txt");
}

int init_inst_file(uchar *inst_file){
    FILE *inst = fopen(inst_file, "r");
    inst_index = 0;
    int i;
    while(!feof(inst)){
        fgets(inst_table, sizeof(inst_table)-1,inst);
        inst_index++;
        printf("Inst Line [%d] : %s", inst_index, inst_table);
    }
    fclose(inst);
}
int init_input_file(uchar *input_file){
    FILE *inp = fopen(input_file,"r");
    line_num = 0;
    while(!feof(inp)){
        fgets(input_data, sizeof(input_data)-1,inp);
        line_num++;
        printf("\nInput Line [%d] : %s", line_num, input_data);

    }
    fclose(inp);
}

int token_parsing(uchar *str);
int search_opcode(uchar *str);
static int assem_pass1(void);
void make_opcode_output(uchar *file_name);

void make_symtab_output(uchar *file_name);
static int assem_pass2(void);
void make_objectcode_output(uchar *file_name);

int main(){
    init_my_assembler();
}