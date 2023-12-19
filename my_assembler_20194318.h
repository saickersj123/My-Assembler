#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include <ctype.h>

/* 
 * my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다. 
 * 
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

typedef unsigned char uchar;

/* 
 * instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
 * 라인 별로 하나의 instruction을 저장한다.
 */
struct inst_unit
{
	uchar str[10];
	int ops; //	0:-		1:M		2:R		3:N		4:RR	5:RN
	int format; // 0:not SIC/XE code	1:format 1	2:format 2		3:format 3/4
	uchar op;
};
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
static int inst_index;
static int inst_count;

/*
 * 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
 fopen input.txt
 */
uchar *input_data[MAX_LINES];
static int line_num;

/* 
 * 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
 * operator는 renaming을 허용한다.
 */
struct token_unit
{
	uchar *label;
	uchar *operator;
	uchar operand[MAX_OPERAND][20];
	uchar comment[100];
	int addr;
	int ta;
};


typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;

/*
 * 심볼을 관리하는 구조체이다.
 * 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
 * 추후 과제에 사용 예정
 */
struct symbol_unit
{
	uchar symbol[10];
	int addr;
	int sec; // section
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
static int sym_index;

static int locctr; // loc값 
static int starting_address;
static int program_length;
static int sec;

// Define the structure for the program section
typedef struct {
    int sec;             // Section number
	//int locctr;          // Location counter
    int program_length;  // Program length
} csect;
#define MAX_CSECT 10
csect csect_table[MAX_CSECT];
//--------------
static int is_first_write = 1;
static int first_write = 1;
static uchar *input_file; // 입력 파일 input.txt
static uchar *output_file;// 출력 파일 output.txt

int init_my_assembler(void); // 어셈블러 초기화 파일 init_inst_file()과 init_input_file()도 포함
int init_inst_file(uchar *inst_file); //inst.data를 읽고 inst_table[]에 저장
int init_input_file(uchar *input_file); //intput.txt를 읽고 input_data[]에 저장
int token_parsing(uchar *str); //input_data[]에 저장된 명령어를 토큰화하여 token_table[]에 저장
int search_opcode(uchar *str); //input_data[]에 저장된 명령어의 opcode를 검색 inst_table[]을 참조
static int assem_pass1(void); //SIC/XE Pass1 make_opcode_output과 make_symtab_output을 포함
void make_opcode_output(uchar *file_name); 
void make_symtab_output(uchar *file_name); 
static int assem_pass2(void); // SIC/XE Pass2 make_objectcode_output을 포함
void make_objectcode_output(uchar *file_name, uchar *list_name); // 파일 output.txt에 입력된 명령어의 오브젝트 코드 생성

//Extra functions, variables
void write_intermediate_file(uchar *str, int locctr);
void add_to_symtab(const uchar *label, int loc, int is_equ, int secton);
int search_symtab(uchar *symbol, int section);
int tok_search_opcode(uchar *str);
int init_token_table(void);
int write_listing_line(int format);
void write_text_record(void);
int evaluate_expression(uchar *expr);
int search_literal(uchar *operand);
int search_literaladdr(uchar *operand);
int calculate_byte_length(uchar *operand);
int search_extRtab(uchar *symbol, int section);
int search_extR_index(uchar *symbol, int section);
int search_extDtab(uchar *symbol);
void handle_extdef(uchar *symbol);
void handle_extref(uchar *symbol, int section);
void handle_equ_directive(uchar *label, uchar *operand);
void handle_ltorg_directive(void);
int getREGnum(uchar *register_name);
int generate_object_code(int format);
int hexstr2dec(char H);
int write_literal(void);
int handle_pass2(void);
void write_text_record(void);
int write_listing_line(int format);

typedef struct {
    uchar name[20];
    int leng;
	int value;
    int addr;// -1 if not assigned an address yet
} LT;

// Maximum number of literals (adjust as needed)
#define MAX_LITERALS 100
LT LTtab[MAX_LITERALS];
static int LT_num;
int current_pool;

#define MAX_EXTDEF 10 
#define MAX_EXTREF 10

symbol extDef[MAX_EXTDEF];
symbol extRef[MAX_EXTDEF];
static int extDefCount;
static int extRefCount;

// Define a counter for object code records
FILE *object_code_file;
FILE *listing_file;

// Define an array to store object code
int object_code[20];
uchar text_record[70];
int text_record_start;
int text_record_ctr;

// Define a counter for modification records
int mod_record_count;
int mod_last;
uchar mod_record[30][20];
int is_lt;
int csect_start_address;
int csect_length;
int BASEADDR;
int FEI;
uchar HEXTAB[]= {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
uchar *texp[10];