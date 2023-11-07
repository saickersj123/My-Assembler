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
	uchar op;
	int format; // 0:not SIC/XE code	1:format 1	2:format 2		3:format 3/4
	int ops; //	0:-		1:M		2:R		3:N		4:RR	5:RN
};
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

/*
 * 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
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
	// 다음과제에 사용될 변수
	// uchar nixbpe;
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
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];

static int locctr;
//--------------

static uchar *input_file;
static uchar *output_file;
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
