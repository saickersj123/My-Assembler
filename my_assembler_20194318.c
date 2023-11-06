#include <stdio.h>
#include <stdlib.h>	
#include <string.h>	// 문자열 관리를 위함
#include <math.h>	// log 함수를 사용하기 위함 (자리수 구하기)
#include <malloc.h>	// 동적할당을 위함
#include ".\my_assembler_20194318.h"

int init_my_assembler(void);
int init_inst_file(uchar *inst_file);
int init_input_file(uchar *input_file);
int token_parsing(uchar *str);
int search_opcode(uchar *str);
static int assem_pass1(void);
void make_opcode_output(uchar *file_name);

void make_symtab_output(uchar *file_name);
static int assem_pass2(void);
void make_objectcode_output(uchar *file_name);
