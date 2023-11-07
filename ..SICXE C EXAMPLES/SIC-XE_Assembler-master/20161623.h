/*포함되는 파일*/
#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<dirent.h> 
#include<string.h>
#include<math.h>


/*정의되는 상수*/
#define MAX_INPUT_SIZE 200			// input의 최대 크기
#define MAX_MEMORY_SIZE 1048576		// 메모리의 최대 크기(16X65536)
#define MAX_KEY_LENGTH 7            // hash table의 key값의 최대 길이
#define MAX_FORMAT_LENGTH 7			// hash table의 format값의 최대 길이
#define HASH_TABLE_SIZE 20			// hash table 크기
#define MAX_OBJ_LEN 29

/*구조체 정의*/
typedef struct history_node {		// history 구초제
	char name[MAX_INPUT_SIZE];
	struct history_node* next;
}history_node;

typedef struct hash_table_node {	// hash table 구조체
	int opcode;
	char key[MAX_KEY_LENGTH];
	char format[MAX_FORMAT_LENGTH];
	struct hash_table_node* next;
}hash_table_node;

typedef struct symtab_node {		// symbol table 구조체
	int loc;
	char name[MAX_INPUT_SIZE];
	struct symtab_node* next;
}symtab_node;

typedef struct estab_node {			// load map 구조체
	int address;
	int length;
	char name[8];
	struct estab_node* next;
}estab_node;

typedef struct bp_node{				// break point 구조체
	int bp_loc;
	struct bp_node* next;
}bp_node;

char* input;						// 입력 한 줄을 저장할 문자열
char input_args[4][17];				// 입력된 명령어의 인자를 저장할 문자열
int input_args_len;					// 입력된 명령어의 인자 수를 저장할 변수
unsigned char* memory;				// memory를 저장할 변수 (sizeof unsighed char : 1byte)
int dump_address;					// 마지막으로 dump한 주소
int start_addr;						// 프로그램 시작 주소
int pc;								// 실행중인 프로그램의 주소
int is_running;						// 현재 프로그램이 실행중인지를 저장하는 변수
int prog_lth;						// load된 프로그램의 길이를 저장하는 변수
int regi[9];						// register값을 저장하는 변수

history_node* history_list;
history_node* last_history;
hash_table_node* hash_table[HASH_TABLE_SIZE];
symtab_node* tmp_symtab;
symtab_node* symtab;
estab_node* estab_start;
estab_node* estab_end;
bp_node* bp;
