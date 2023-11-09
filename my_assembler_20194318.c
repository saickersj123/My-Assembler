#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include "my_assembler_20194318.h"

int init_my_assembler(void){
    init_inst_file("inst.data");
    init_input_file("input.txt");
}

int init_inst_file(uchar *inst_file){
    FILE *inst = fopen(inst_file, "r");
    inst_index = 0;
    uchar buffer[sizeof(inst_table)];
    int i;
    printf("Instruction File Initialized\n");
    while(!feof(inst)){

        fgets(buffer, sizeof(buffer),inst);
        uchar *tmp = strtok(buffer, " ");
        strcpy(inst_table[inst_index]->str, tmp);
        tmp = strtok(buffer," ");
        strcpy(inst_table[inst_index]->ops,tmp);
        tmp = strtok(buffer," ");
        strcpy(inst_table[inst_index]->format,tmp);
        tmp = strtok(NULL,"\n");
        strcpy(inst_table[inst_index]->op,tmp);
        inst_index++;
        printf("Inst Line [%d] : %s %d %d %s", inst_index, inst_table[inst_index]->str, inst_table[inst_index]->ops, inst_table[inst_index]->format, inst_table[inst_index]->op);
        
    }
    printf("\n");
    fclose(inst);
}

int init_input_file(uchar *input_file){
    FILE *inp = fopen(input_file,"r");
    line_num = 0;
    printf("Input File Initialized \n");
    while(!feof(inp)){
        fgets(input_data, sizeof(input_data),inp);
        line_num++;
        printf("Input Line [%d] : %s", line_num, input_data);

    }
    printf("\n");
    fclose(inp);
}

/*int token_parsing(uchar *str)
{
    uchar *temp = NULL;
    str = strtok_s(input_data, " ",&temp);
    while(str != NULL){
        str = strtok_s(NULL, " ", &temp);
        strcpy(token_table , str);
    
    if(str == "."){
        //건너뛰기 Skip the line
        }
    }
    if(str == NULL){

    }
}*/

int search_opcode(uchar *str);
static int assem_pass1(void);//search opcode, opcode output, symtab output
void make_opcode_output(uchar *file_name);
void make_symtab_output(uchar *file_name);

static int assem_pass2(void);
void make_objectcode_output(uchar *file_name);

int main(){
    init_my_assembler();
    token_parsing(input_data);
    return 0;
}