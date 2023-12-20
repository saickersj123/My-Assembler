/* 
 * Header file containing declarations and macros for the my_assembler_20194318.c
 */
#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include <ctype.h>

#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

typedef unsigned char uchar;

/* 
 * Structure to store information from the instruction list file
 * Represents one instruction per line
 */
struct inst_unit
{
	uchar str[10];
	//int ops; //	0:-		1:M		2:R		3:N		4:RR	5:RN
	int format; // 0:directive	1:format 1	2:format 2	3:format 3/4
	uchar op;
};
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
static int inst_index;
static int inst_count;

/*
 * Table to store the source code to be assembled. Managed line by line
 */
uchar *input_data[MAX_LINES];
static int line_num;

/* 
 * Structure to manage the source code at the token level
 */
struct token_unit
{
	uchar *label;
	uchar *operator;
	uchar operand[MAX_OPERAND][20];
	uchar comment[100];
	int addr;
};


typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;

/*
 * Structure to manage symbols
 * Symbol table consists of symbol name and its location
 * and Section
 */

struct symbol_unit
{
	uchar symbol[10];
	int addr; // Locctr
	int sec; // Section
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES]; // Symbol table
static int sym_index; // Symbol index
static int locctr; // Location counter

int init_my_assembler(void); // Initialize the assembler, includes init_inst_file() and init_input_file()
int init_inst_file(uchar *inst_file); // Read inst.data and store in inst_table[]
int init_input_file(uchar *input_file); // Read input.txt and store in input_data[]
int token_parsing(uchar *str); // Tokenize lines stored in input_data[] and store in token_table[]
int search_opcode(uchar *str); // Search opcode from inst_table[] returns opcode index
static int assem_pass1(void); // Process Pass 1
void make_opcode_output(uchar *file_name); // Make op table file
void make_symtab_output(uchar *file_name); // Make symbol table file
static int assem_pass2(void); // Process Pass 2
void make_objectcode_output(uchar *file_name, uchar *list_name); // Make object program and list file

// Additional functions and variables
void write_intermediate_file(uchar *str, int locctr); // Make and write locctr and input line to intermediate file
void add_to_symtab(const uchar *label, int loc, int is_equ, int sec); // Add symbol to sym_table[]
int search_symtab(uchar *symbol, int section); // Search symbol from sym_table[] returns address
int init_token_table(void); // Initialize token_table[]
int evaluate_expression(uchar *expr); // Evalutate expression ex) BUFEND-BUFFER returns result
int search_literal(uchar *operand); // Search literal from LTtab[] returns address
int calculate_byte_length(uchar *operand); // Calculate byte length of operand returns the length
int search_extRtab(uchar *symbol, int section); // Search external references of current section from extRef[] returns address
int search_extDtab(uchar *symbol); //  Search external definition from extDef[] returns address
void handle_extdef(uchar *symbol);// Handle EXTDEF: add symbol, address to extDef[]
void handle_extref(uchar *symbol, int section); // Handle EXTREF: add symbol, address to extRef[]
void handle_equ_directive(uchar *label, uchar *operand); // Handle EQU: update locctr, includes evaluate_expression()
void handle_ltorg_directive(void);// Handle LTORG: add literals of last section to LTtab[], update locctr
int hexstr2dec(char H);// Convert hex string to dec
int getREGnum(uchar *register_name);// Return register number
int generate_object_code(int format);// Generate object code by inst format and operands
int write_listing_line(int format);// Write line number, locctr, input line, object code to list file by format
void write_text_record(void);// Write text record to object program
int write_literal(void);// Write literal to list file
int handle_pass2(void);// Handle pass2 by directives: write records and listings

/*
 * Structure to manage csects
 * csect table consists of section and its program length
 */
typedef struct {
    int sec;             // Section number
	//int locctr;        // Location counter
    int program_length;  // Program length
} csect;

#define MAX_CSECT 10 // Maximum number of Section
csect csect_table[MAX_CSECT];
int csect_start_address; 

/*
 * Structure to manage literals
 * literal table consists of literal and its pool number
 * And length and address
 */
typedef struct {
    uchar name[20];
    int leng; // length of literal
	int value; // pool number of literal
    int addr; // locctr of literal
} LT;

#define MAX_LITERALS 100 // Maximum number of literals
LT LTtab[MAX_LITERALS]; 
int LT_num;
int current_pool; // Track current pool number

#define MAX_EXTDEF 10 
#define MAX_EXTREF 10
symbol extDef[MAX_EXTDEF];
symbol extRef[MAX_EXTDEF];
int extDefCount;
int extRefCount;

FILE *object_program_file; // object program file
FILE *listing_file; // list file

static int starting_address;
static int sec;
int object_code[20];
int text_record_start;
int text_record_ctr;
int mod_record_count;
int mod_last; // Track last record count 
int is_lt; // Flag to check if it is LT
int BASEADDR; // Base register address
int is_first_write = 1; //Flag to check first write for intermediate file
int first_write = 1; //Flag to check first wirte for object / list

uchar sign; //sign for expressions
uchar sign2;
uchar text_record[70];
uchar mod_record[30][20];
uchar HEXTAB[]= {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
