#include <stdio.h>
#include <stdlib.h>	
#include <string.h>	// ���ڿ� ������ ����
#include <math.h>	// log �Լ��� ����ϱ� ���� (�ڸ��� ���ϱ�)
#include <malloc.h>	// �����Ҵ��� ����

// ��ɾ�, �ǿ����� �տ� �پ��ִ� ��ȣ���� �˾ƺ��� ������ define
#define PLUS 1
#define SHARP 2
#define AT 3

// ������ ������� ��Ȯ�ϰ� �ǹ��ϱ� ���ؼ� define
#define BUFFER_SIZE 256	// ������ ũ��
#define LABEL_LENGTH 32	// �ҽ��ڵ�� �󺧵��� �ִ� ����
#define SYMTAB_SIZE 20	// �ɺ����̺��� ���ڵ� �ִ� ����
#define IMR_SIZE 100	// �߰������� ���ڵ� �ִ� ����
#define RLD_SIZE 20		// Relocation DIctionary�� ���ڵ� �ִ� ����
#define EXT_SIZE 20		// EXTREF�� EXTDEF ���̺��� ���ڵ� �ִ� ����
#define ESD_SIZE 20		// ESD�� ���ڵ� �ִ� ����
#define CSECT_SIZE 10	// �ҽ����� ���� ������ �� �ִ� ���α׷��� ����

/***************************** DECLERATE VARIABLE ****************************/
typedef struct OperationCodeTable {	// OP ���̺��� ������ ���ڵ� ����ü
	char Mnemonic[LABEL_LENGTH];	// ��ɾ��� ���� (ex. LDB, LDA, etc...)
	char Format;	// ��ɾ��� ���� (��ɾ��� ����)	3/4 ������ ���ǻ� 3�������� ǥ���ϵ��� �����߽��ϴ�.
	unsigned short int  ManchineCode;	// �ش� ��ɾ��� ���� �ڵ�
}SIC_OPTAB;

typedef struct SymbolTable { // �ɺ����̺��� ������ ���ڵ� ����ü
	char Label[LABEL_LENGTH];	// ���̺��� �̸�
	int Address;	// ���̺��� ����Ű�� �ּ�
}SIC_SYMTAB;

// �ܺ� ���� ���̺��� ���� ���ڵ�
typedef struct EXTREF {
	char Label[LABEL_LENGTH];
} EXTREF;

// Ÿ ���α׷����� �ܺ� ������ ������ ���̺��� ���� ���ڵ�
typedef struct EXTDEF {
	char Label[LABEL_LENGTH];
	int Address;	// �ش� ���̺��� ��ġ
} EXTDEF;

// �������� ���̺��� �����ϴ� �������� ���ڵ�
typedef struct RegisterTable {
	char name[LABEL_LENGTH];	// �������� Mnemonic
	int id;	// ���������� ������ȣ
} SIC_XE_REGISTER;

// �������ڵ� �ۼ��� ���� ���ġ�� �ʿ��� �κ��� ��� Dictionary ���ڵ�
typedef struct RelocationDictionary {
	int Address;	// ���ġ�� �ʿ��� ��ġ
	int Nibbles;	// ���ġ�� �ʿ��� ����
} RLD;

// �ܺ� ���� ���̺��� �������� ��, ���ġ�� �ʿ��ϹǷ� �� �κ��� ��� Dictionary ���ڵ�
typedef struct ExternalSymbolDictionary {
	char Label[LABEL_LENGTH];	// �ܺ� ���� ���̺�
	char sign;		// ����, ����� ��Ÿ��
	int Address;	// ���ġ�� �ʿ��� ��ġ
	int Nibbles;	// ���ġ�� �ʿ��� ����
} ESD;

typedef struct IntermediateRecord {	// �߰����� ����
	unsigned short int LineIndex;	// �ҽ��ڵ��� ���� �����ϴ� ����
	unsigned short int Loc;	//  �ش� ��ɾ��� �޸𸮻� ��ġ
	unsigned long long int ObjectCode;	//  Pass 2�� ���� Assemble�� �����ڵ�
	char LabelField[LABEL_LENGTH];	// �ҽ��ڵ�� ǥ��Ǿ��ִ� ���̺�
	char OperatorField[LABEL_LENGTH];	// �ҽ��ڵ�� ǥ��Ǿ��ִ� Mnemonic
	char OperandField[LABEL_LENGTH];	// �ҽ��ڵ�� ǥ��Ǿ��ִ� �ǿ�����
}IntermediateRec;

// �Լ��� ����� �����ϱ� ���� �ӽ÷� ����ϴ� ����������
int Counter;	// Opcodeã�� �� �� ��ɾ��� ��ġ�� ����Ű�� ���� ����
int RegIdx;		// ���������� ��ġ�� ����Ŵ. Counter ������ ���� ���
int SymIdx;		// Label�� ��ġ�� ����Ŵ. Counter ������ ���� ���
int ExtRefIdx;	// ExtRefTAB���� ã�� �ܺ� ���� ���̺��� ��ġ�� ����Ŵ. Counter ������ ���� ���
int LOCCTR[IMR_SIZE];	// �� ��ɾ���� �޸𸮸� �������� Location Counter. �߰������� ������ �����ϴ�
int LocctrCounter = 0;	// LOCCTR�� Index ����
int Flag;
int Index;
int j;
int ManchineCode;
int SymtabCounter[CSECT_SIZE] = { 0, };	// �ɺ����̺��� ������ ���� ����Ű�� ���� ����
int start_address[CSECT_SIZE] = { 0, };	// ���α׷��� ���� �ּ�
int program_length[CSECT_SIZE] = { 0, };// ���α׷��� �� ����
int ArrayIndex[CSECT_SIZE] = { 0, };	// �߰������� ���� ����Ű�� ���� Index ����
int RLDCounter[CSECT_SIZE] = { 0, };	// ���ġ�� �ʿ��� �κ��� ������ ���� ����Ű�� ���� ���� (���� ���ڵ�)
int ExtRefCounter[CSECT_SIZE] = { 0, };	// ���� ���ڵ��� ����
int ExtDefCounter[CSECT_SIZE] = { 0, };	// ���� ���ڵ��� ����
int ESDCounter[CSECT_SIZE] = { 0, };	// ESD ���ڵ��� ������ ���� ����Ű�� ���� ����
int CSectCounter = 0;	// �ҽ��ڵ� ���� �����ϴ� ���α׷��� ������ ���� ���� ����
char End_operand[LABEL_LENGTH];	// END ������ �������� ���۷��带 �����Ѵ�. (���� ���ڵ带 �ۼ��ϱ� ����)

unsigned short int FoundOnSymtab_flag = 0;	// �ش� ���̺��� �ɺ����̺���� ã�Ҵٴ� ���� ��ȯ�ϱ� ����
unsigned short int FoundOnOptab_flag = 0;	// �ش� Opcode�� Mnemonic�� OP ���̺���� ã�Ҵٴ� ���� ��ȯ�ϱ� ����
unsigned short int FoundOnRegTab_flag = 0;	// �������� ���̺�� �ش� ���������� ������ �ִ��� Ȯ��
unsigned short int FoundOnExtRefTab_flag = 0;	// �ܺ� ���� ���̺� �߿� �ش� ���̺��� �ִ��� Ȯ��

// �� �������� ��� �ҽ��ڵ���� ǥ����� ����
char Buffer[BUFFER_SIZE];	// �� �ҽ��ڵ带 �б����� ���� ����
char Label[LABEL_LENGTH];	// ���̺��� �ӽ÷� �����ϱ� ���� ����
char Mnemonic[LABEL_LENGTH];	// Mnemnic�� �ӽ÷� �����ϱ� ���� ����
char Operand[LABEL_LENGTH];	// �ǿ����ڸ� �ӽ÷� �����ϱ� ���� ����

// ���� ���α׷��� 1���� �ʿ��ϹǷ� 2���� �迭�� ����
SIC_SYMTAB SYMTAB[CSECT_SIZE][SYMTAB_SIZE];	// �ɺ����̺� ����
EXTREF ExtRefTAB[CSECT_SIZE][EXT_SIZE];		// ���� ���ڵ带 ���� ���̺� ����
EXTDEF ExtDefTAB[CSECT_SIZE][EXT_SIZE];		// ���� ���ڵ带 ���� ���̺� ����
IntermediateRec* IMRArray[CSECT_SIZE][IMR_SIZE];	// �߰����� ����
RLD RLDArray[CSECT_SIZE][RLD_SIZE];	// ���ġ�� �ʿ��� �κ��� �����ϱ� ���� ����
ESD ESDArray[CSECT_SIZE][ESD_SIZE];

// �������� ���̺�
static SIC_XE_REGISTER REG_TAB[] =
{
	{ "A", 0 },
	{ "X", 1 },
	{ "L", 2 },
	{ "B", 3 },
	{ "S", 4 },
	{ "T", 5 },
	{ "F", 6 },
	{ "PC", 8 },
	{ "SW", 9 }
};

// OP ���̺�
static SIC_OPTAB OPTAB[] =
{
	/*********Instruction Set II***********/
	{ "ADDF", '3', 0x58 },
	{ "COMPF", '3', 0x88 },
	{ "DIVF", '3', 0x64 },
	{ "FIX", '1', 0xC4 },
	{ "FLOAT", '1', 0xC0 },
	{ "LDF", '3', 0x70 },
	{ "MULF", '3', 0x60 },
	{ "NORM", '1', 0xC8 },
	{ "STF", '3', 0x80 },
	{ "SUBF", '3', 0x5C },
	/*********Instruction Set I***********/
	{ "ADDR", '2', 0x90 },
	{ "CLEAR", '2', 0xB4 },
	{ "COMPR", '2', 0xA0 },
	{ "DIVR", '2', 0x9C },
	{ "HIO", '1', 0xF4 },
	{ "LDB", '3', 0x68 },
	{ "LDS", '3', 0x6C },
	{ "LDT" , '3', 0x74 },
	{ "LPS", '3', 0xD0 },
	{ "MULR", '2', 0x98 },
	{ "RMO", '2', 0xAC },
	{ "SHIFTL", '2', 0xA4 },
	{ "SHIFTR", '2', 0xA8 },
	{ "SIO", '1', 0xF0 },
	{ "SSK", '3', 0xEC },
	{ "STB", '3', 0x78 },
	{ "STS", '3', 0x7C },
	{ "STT", '3', 0x84 },
	{ "SUBR", '2', 0x94 },
	{ "SVC", '2', 0xB0 },
	{ "TIO", '1', 0xF8 },
	{ "TIXR", '2', 0xB8 },
	/**********SIC Instruction Set*********/
	{ "ADD",  '3',  0x18 },
	{ "AND",  '3',  0x40 },
	{ "COMP",  '3',  0x28 },
	{ "DIV",  '3',  0x24 },
	{ "J",  '3',  0x3C },
	{ "JEQ",  '3',  0x30 },
	{ "JGT",  '3',  0x34 },
	{ "JLT",  '3',  0x38 },
	{ "JSUB",  '3',  0x48 },
	{ "LDA",  '3',  0x00 },
	{ "LDCH",  '3',  0x50 },
	{ "LDL",  '3',  0x08 },
	{ "LDX",  '3',  0x04 },
	{ "MUL",  '3',  0x20 },
	{ "OR",  '3',  0x44 },
	{ "RD",  '3',  0xD8 },
	{ "RSUB",  '3',  0x4F },
	{ "STA",  '3',  0x0C },
	{ "STCH",  '3',  0x54 },
	{ "STL",  '3',  0x14 },
	{ "STSW",  '3',  0xE8 },
	{ "STX",  '3',  0x10 },
	{ "SUB",  '3',  0x1C },
	{ "TD",  '3',  0xE0 },
	{ "TIX",  '3',  0x2C },
	{ "WD",  '3',  0xDC },
};


/****************************** DFINATE FUNCTION *****************************/
char* ReadLabel() {	// ���̺� �б�
	j = 0;//zeroing
	while (Buffer[Index] != ' ' && Buffer[Index] != '\t' && Buffer[Index] != '\n')
		Label[j++] = Buffer[Index++];
	Label[j] = '\0';
	return(Label);
}

void SkipSpace() {	// ���� ��ŵ�ϱ� (Index�� �ڷ� �ű�)
	while (Buffer[Index] == ' ' || Buffer[Index] == '\t')
		Index++;
}

int ReadFlag(char *Mnemonic) {	// Mnemonic���� �÷��� ��Ʈ �б�
	Flag = 0;
	switch (Mnemonic[0]) {	// Mnemonic�� ù��° ���ڰ� Ư�������ϰ��
	case '+':
		Flag = PLUS;	// extended instruction
		break;
	case '#':
		Flag = SHARP;	// immediate addressing mode
		break;
	case '@':
		Flag = AT;	// indirect addressing mode
		break;
	default:
		Flag = 0;	// default (�ƹ��� ǥ�ð� ���� ���)
	}
	return Flag;
}

char* ReadOprator() {	// Mnemonic �б�
	j = 0;//zeroing
	while (Buffer[Index] != ' ' && Buffer[Index] != '\t' && Buffer[Index] != '\n')	// ������ ���ö����� Mnemonic �б�
		Mnemonic[j++] = Buffer[Index++];
	Mnemonic[j] = '\0';	// �ܼ� ���ڹ迭�� ���ڿ��� �ν��ϵ��� �� ���� �߰�
	return(Mnemonic);	// ���ڿ� ��ȯ
}

char* ReadOperand() {	// �ǿ����� �б�
	j = 0;//zeroing
	while (Buffer[Index] != ' ' && Buffer[Index] != '\t' && Buffer[Index] != '\n')	// ������ ���ö����� Operand �б�
		Operand[j++] = Buffer[Index++];
	Operand[j] = '\0';	// �ܼ� ���ڹ迭�� ���ڿ��� �ν��ϵ��� �� ���� �߰�
	return(Operand);	// ���ڿ� ��ȯ
}

void RecordSymtab(char* label) {	// �ɺ����̺�� �ش� ���̺��� ��ġ�� ���̺� �Է�
	if (ReadFlag(label)) { // Immediate or Indirect Addressing Mode ���� ó��
		label = label + 1;
	}
	strcpy(SYMTAB[CSectCounter][SymtabCounter[CSectCounter]].Label, label);	// Symbol ���̺�� Label �߰�
	SYMTAB[CSectCounter][SymtabCounter[CSectCounter]].Address = LOCCTR[LocctrCounter - 1];	// �ش� Label�� �޸� ��ġ�� ���
	SymtabCounter[CSectCounter]++;	// 1�� �߰��Ǿ����Ƿ� ī��Ʈ 1����
}

void RecordRLD(char* Mnemonic, int loc, int idx) {	// ���ġ�� �ʿ��� �κ� RLDArray�� �߰�
	RLDArray[idx][RLDCounter[idx]].Address = loc + 1;	// ��ɾ� ���� ��ġ���� OP code�� �÷��׺�Ʈ �κ��� ������ ���� ��ġ ����
	RLDArray[idx][RLDCounter[idx]].Nibbles = 3;	// ���ġ�� �ʿ��� �κ� ���� ���� (3������ ��� 3 �Ϻ�)
	if (ReadFlag(Mnemonic)) {	// 4������ ��� 1����Ʈ(2 �Ϻ�)��ŭ �ǿ����ڰ� �þ�Ƿ� �߰�
		RLDArray[idx][RLDCounter[idx]].Nibbles += 2;
	}
	RLDCounter[idx]++;	// RLDCounter�� ���� 1����
}

void RecordESD(char * Mnemonic, int loc, int byte, int idx) {
	// �������ڵ� �ۼ��� ���� ���
	ESDArray[idx][ESDCounter[idx]].Address = loc + 1;	// ��ɾ� ���� ��ġ���� OP code�� �÷��׺�Ʈ �κ��� ������ ���� ��ġ ����
	ESDArray[idx][ESDCounter[idx]].sign = '+';	// ����� ���
	if (Mnemonic[0] == '-') {
		ESDArray[idx][ESDCounter[idx]].sign = '-';	// ������ ���
		Mnemonic += 1;	// ���� ��ȣ ����
	} else if (Mnemonic[0] == '+') {
		Mnemonic += 1;	// ��� ��ȣ ����
	}
	ESDArray[idx][ESDCounter[idx]].Nibbles = 3;	// ���ġ�� �ʿ��� �κ� ���� ���� (3������ ��� 3 �Ϻ�)
	if (byte == 4) {
		ESDArray[idx][ESDCounter[idx]].Nibbles += 2;	// 4���� ��ɾ��� �ǿ����ڴ� 5 �Ϻ� �̹Ƿ� 2 �Ϻ� �߰�
	}
	strcpy(ESDArray[idx][ESDCounter[idx]].Label, Mnemonic);	// ESD�� �ش� �����ȣ�� ���
	ESDCounter[idx]++;	// RLDCounter�� ���� 1����
}

void RecordEXTREF(char * Mnemonic) {
	strcpy(ExtRefTAB[CSectCounter][ExtRefCounter[CSectCounter]].Label, Mnemonic);	// �ܺ� ���� ���̺���� ���
	ExtRefCounter[CSectCounter]++;	// ESDCounter�� ���� 1����
}

void RecordEXTDEF(char * Mnemonic) {
	strcpy(ExtDefTAB[CSectCounter][ExtDefCounter[CSectCounter]].Label, Mnemonic);	// �ܺ� ���� ���̺���� ���
	ExtDefCounter[CSectCounter]++;	// RLDCounter�� ���� 1����
}

int RecordEXTDEFLoc() {
	// �ܺ� ���� ���̺��� �ּҸ� SYMTAB���� ã�Ƽ� ���
	int i;
	for (i = 0; i < ExtDefCounter[CSectCounter]; i++) {
		if (SearchSymtab(ExtDefTAB[CSectCounter][i].Label, CSectCounter)) {
			ExtDefTAB[CSectCounter][i].Address = SYMTAB[CSectCounter][SymIdx].Address;
		} else {
			return 1;	// SYMTAB�� �����Ƿ� ���� ó��
		}
	}
	return 0;	// ��� �ܺ� ���� ���̺���� ã�Ƽ� �����
}

int SearchSymtab(char* label, int idx) {	// �ɺ����̺���� ���̺� ã��
	FoundOnSymtab_flag = 0;
	if (ReadFlag(label)) { // Immediate Addressing Mode�̰ų� Indirect Addressing Mode�� ��� ����ó��
		label = label + 1;
	}

	for (int k = 0; k <= SymtabCounter[idx]; k++) {	// �ݺ��� ���� ã��
		if (!strcmp(SYMTAB[idx][k].Label, label)) {	// label�� �ɺ����̺�� ���� ���
			FoundOnSymtab_flag = 1;	// ã�Ҵٴ� �ǹ̸� ǥ���ϱ����� �÷���
			SymIdx = k;	// SymIdx�� �ɺ����̺���� ��� ��ġ�� �ִ��� ����Ų��.
			return (FoundOnSymtab_flag);
		}
	}
	return (FoundOnSymtab_flag);	// ������ 0 ��ȯ
}

int SearchOptab(char * Mnemonic) {	// �ɺ����̺���� OP code ã��
	int size = sizeof(OPTAB) / sizeof(SIC_OPTAB);
	FoundOnOptab_flag = 0;
	if (ReadFlag(Mnemonic)) { // Extended Instruction�� ��� ����ó��
		Mnemonic = Mnemonic + 1;	// ���ڿ������͸� 1������Ŵ. �Ǿ��ڸ� ����
	}
	for (int i = 0; i<size; i++) {
		if (!strcmp(Mnemonic, OPTAB[i].Mnemonic)) {	// OP ���̺�� �ش� Mnemonic�� ���� ���
			Counter = i;	// ���� ��� Counter�� �ش� OP code�� �ִ� OPTAB �� Index ��ȯ�ϱ�
			FoundOnOptab_flag = 1;	// ã�Ҵٴ� �ǹ̸� ǥ���ϱ����� �÷���
			break;
		}
	}
	return (FoundOnOptab_flag);	// ������ 0 ��ȯ
}

int SearchRegTab(char * Mnemonic) {	// �̸� ���ǵ� �������� ���̺���� �ش� �������͸� �д´�
	int size = sizeof(REG_TAB) / sizeof(SIC_XE_REGISTER);
	FoundOnRegTab_flag = 0;
	for (int i = 0; i < size; i++) {
		if (!strcmp(Mnemonic, REG_TAB[i].name)) {	// �������� ���̺�� �ش� ���������� ��ȣ�� ���� ���
			RegIdx = i;	// �������� ���̺���� ��� ��ġ�� �ִ��� ����Ŵ
			FoundOnRegTab_flag = 1;	// ã�Ҵٴ� �ǹ̸� ǥ���ϱ� ���� �÷���
			break;
		}
	}
	return (FoundOnRegTab_flag);	// ������ 0 ��ȯ
}

int SearchExtRefTAB(char * Mnemonic, int idx) {
	// ���ԵǾ��ִ� ExtRefTAB�� �ش� ���̺��� �ִ��� Ȯ��
	if (ReadFlag(Mnemonic)) {	// �� �տ� �÷��װ� �پ��ִٸ�
		Mnemonic += 1;	// �÷��� ����
	}
	FoundOnExtRefTab_flag = 0;
	for (int i = 0; i < ExtRefCounter[idx]; i++) {
		if (!strcmp(Mnemonic, ExtRefTAB[idx][i].Label)) {	// �ܺ� ���� ���̺��� ���� ���
			ExtRefIdx = i;	// �ܺ� ���� ���̺��� ��ġ�� ��ȯ
			FoundOnExtRefTab_flag = 1;	// ã���� ǥ���ϱ� ���� �÷���
			break;
		}
	}
	return (FoundOnExtRefTab_flag);
}

int isNum(char * str) {	// ���ڿ��� �̷�� �ִ� ��� ���ҵ��� ���ڷ� �̷�����ִ��� Ȯ��
	if (ReadFlag(str)) {	// ���ڿ� �� �տ� �÷��׺�Ʈ�� ���� ��� �̸� �����ϱ� ����.
		str += 1;
	}
	int i, len = strlen(str);
	for (i = 0; i < len; ++i) {	// ���ڿ� ���̸�ŭ �ݺ��ؼ� ��� �������� �Ǵ�
		if ('0' > str[i] || '9' < str[i]) {	// ���ڰ� �ƴ� ��� 0�� ��ȯ
			if (str[i] == '-') continue;	// ������ ��Ÿ���� '-'�� ������ ���� ����
			return 0;
		}
	}
	return 1;	// ��� �����̹Ƿ� 1�� ��ȯ
}

int isFloatNum(char * str) {	// Floating Number���� Ȯ���ϱ� ���� �Լ�
	if (ReadFlag(str)) {	// ���ڿ� �� �տ� �÷��׺�Ʈ�� ���� ��� �̸� �����ϱ� ����.
		str += 1;
	}
	int i, len = strlen(str), f = 0;	// �Ҽ����� ������ ���� ��� Float Num�� �ƴϱ� ������ �б�ó���ϱ� ���� ���� f
	for (i = 0; i < len; ++i) {	// ���ڿ� ���̸�ŭ �ݺ��ؼ� ��� �������� �Ǵ�
		if ('0' > str[i] || '9' < str[i]) {	// ���ڰ� �ƴ� ��� 0�� ��ȯ
			if (str[i] == '.' && f == 0) {
				f = 1;
				continue;
			}
			if (str[i] == '-') continue;	// �Ҽ������� ��Ÿ���� '.'�� ������ ��Ÿ���� '-'�� ������ ���� ����
			return 0;
		}
	}
	return (f != 0) ? 1 : 0;	// ��� �����̹Ƿ� 1�� ��ȯ
}

unsigned long ConvertNumber(int diff, int nibble) {
	// ��Ʈ�� ���� ������ ����ϱ� ���� �Լ� (12 bit�̹Ƿ� ���� ����� �Ѵ�.)
	if (diff >= 0) { // ����̹Ƿ� �״�� ��ȯ
		return diff;
	}
	// �����ϰ�� �ڷ����� ũ�Ⱑ 12��Ʈ��� �����ϰ� 2's completion�� ����
	// ǥ���ϴ� nibble�� ���� �ڿ� �߶󳻴� ��Ʈ ���� �ٸ����Ѵ�.
	if (nibble == 5) {
		diff ^= 0xFFF00000;	// 20��Ʈ ������ bit�� �����ϱ� ���ؼ� �� 32��Ʈ�� int���� �� 12��Ʈ�� 0���� �����
	} else {
		diff ^= 0xFFFFF000;	// 12��Ʈ ������ bit�� �����ϱ� ���ؼ� �� 32��Ʈ�� int���� �� 20��Ʈ�� 0���� �����
	}
	return diff;
}

int StrToDec(char* c) {	// 10������ ǥ���ϴ� String�� ���������� ��ȯ�ؼ� ��ȯ
	if (ReadFlag(c)) {	// �÷��׺�Ʈ�� �����ϱ�����.
		c += 1;
	}
	int dec_num = 0;
	char temp[10];
	strcpy(temp, c);	// temp�� ���ڿ� ����

	int len = strlen(c);
	for (int k = len - 1, l = 1; k >= 0; k--)	// �� ���ڿ��� �Ųٷ� �о dec_num�� ���
	{
		if (temp[0] == '-') { // ������ ��Ÿ���� '-'�� �����ϱ� ����
			continue;
		}
		dec_num = dec_num + (int)(temp[k] - '0')*l;
		l = l * 10;	// �ڸ����� ����ϱ����� ������ ���� ���ڵ��� ���° �ڸ����� ��Ÿ��
	}
	return (temp[0] == '-') ? (-dec_num) : (dec_num);	// ������ ��� ������ ��ȯ
}

int StrToHex(char* c)	// 16������ ǥ���ϴ� String�� ���������� ��ȯ�ؼ� ��ȯ
{
	int hex_num = 0;
	char temp[10];
	strcpy(temp, c);	// temp�� ���ڿ� ����

	int len = strlen(temp);
	for (int k = len - 1, l = 1; k >= 0; k--)	// �� ���ڿ��� �Ųٷ� �о� 16������ ���������� ��ȯ
	{
		if (temp[k] >= '0' && temp[k] <= '9')
			hex_num = hex_num + (int)(temp[k] - '0')*l;
		else if (temp[k] >= 'A' && temp[k] <= 'F')	// �빮�� �ϰ��
			hex_num = hex_num + (int)(temp[k] - 'A' + 10)*l;
		else if (temp[k] >= 'a' && temp[k] >= 'f')	// �ҹ��� �ϰ��
			hex_num = hex_num + (int)(temp[k] - 'a' + 10)*l;
		else;
		l = l * 16; // �ڸ����� ����ϱ����� ������ ���� ���ڵ��� ���° �ڸ����� ��Ÿ��
	}
	return (hex_num);
}

double StrToFloat(char* c) {
	double float_num = 0;
	int len = strlen(c);
	for (int i = len - 1; i >= 0; i--) {
		float_num /= 10.0;
		float_num += (c[i] - '0')/10.0;
	}
	return float_num;
}

unsigned long long ConvertFloatNum(char * operand) {
	// Floating Number�� �ٲٱ� ���� �Լ�
	int dec_size = 0, b = 0, k = 0;
	// dec_size : �����ΰ� �����ϴ� ��Ʈ��
	// b : �Ҽ��ΰ� �����ϴ� ��Ʈ��
	// k : ���� �����ΰ� 0�̶�� �Ҽ��� �������������� ���� 
	int i = 0, j = 0;	// �ε��� ����
	unsigned long long int s = 0, dec = 0, f = 0, e = 0x400;
	// 48��Ʈ�� unsigned long long int�� ǥ���� �� �ֱ� ������ ����
	// s : ����, ����� ǥ���ϱ� ���� ���� (���� : 1, ��� : 0)
	// dec : �����θ� ǥ���ϴ� �κ�
	// f : �Ҽ��θ� ǥ���ϴ� �κ�
	// e : �����θ� ǥ���ϴ� �κ�
	double frac = 0;	// ������ ���� fraction�� ǥ���ϴ� �κ�
	char temp[1000];	// ������, �Ҽ��θ� �߶󳻱� ���� �ӽ� ����

	if (ReadFlag(operand)) {	// ���ڿ� �� �տ� �÷��׺�Ʈ�� ���� ��� �̸� �����ϱ� ����.
		operand += 1;
	}
	if (operand[0] == '-') {	// ������ �� �б�ó��
		s = 1;	// �������� ǥ��
		operand += 1;	// '-' ����
	}

	// ������ ����ϱ� ���� �ڸ��� ���� ������, �Ҽ��� ������ int���� ����
	do {
		if (operand[i] == '.') {	// ������ �߶� ����
			temp[j] = '\0';
			dec = StrToDec(temp);	// ������ ���
			if (dec > 0) {	// 0�� ��� �б�ó�� (���Ѵ�)
				dec_size = log2(dec) + 1;
			}
			j = 0;
		}
		else if (operand[i] == '\0') {
			// �Ҽ��� ���
			temp[j] = '\0';
			frac = StrToFloat(temp);	// �Ҽ��� double������ ����
			if ((dec + frac) == 0) {
				// �����ο� �Ҽ��� ��� 0�� ��� 0�� ��ȯ
				return 0;
			}
			while (frac == 0 || 36 - dec_size > b) {
				// ���̻� ���� �Ҽ��ΰ� ���� ��쳪 ǥ�������� ��Ʈ ���� �Ѿ ��� �ݺ��� Ż��
				frac *= 2; // �Ҽ��� 2 ����
				f <<= 1;	// ǥ���� �Ҽ��ο� 1��Ʈ�� �ڸ��� ����
				if (f != 0 || dec_size != 0) {
					// f�� � ��Ʈ�� �� �ְų� �������� ���̰� 0�� �ƴҶ� b ����
					// ��, 0.00357 ���� ��� 3�� ��Ÿ�� ������ ǥ���� ��Ʈ�� �����ϱ� ����
					b++;	// ǥ���� ��Ʈ �� 1����
				}

				if ((int)frac >= 1) {	// ���� �Ҽ��� 1.xxx������ ���
					frac -= 1;	// 0.xxx���·� ����
					f += 1;	// f�� �� ������ ��Ʈ�� 1����
				} else if (f == 0) {	// f�� ������ 0�� ��� �����ΰ� 0�϶� ����ǥ���� ���� k�� 1���� 
					k += 1;
				}
			}
		}
		else {
			temp[j++] = operand[i];
		}
	} while (operand[i++] != '\0');

	e += (dec_size > 0) ? (dec_size - 1) : (-k);	// ���� �κ� ǥ��

	if (dec_size > 36) {	// �����ΰ� 36��Ʈ�� �Ѿ ��� �Ѵ� ��Ʈ �߶󳻱�
		dec >>= (dec_size - 36);
		dec_size = 36;
	}
	else {	// �����ΰ� 36��Ʈ�� �ȵ� ��� �Ҽ����� �Ǿ����� ����
		dec <<= (36 - dec_size);
	}

	if (b >= (36 - dec_size)) {	// �����ΰ� ������ �κ��� ������ ��Ʈ�� �Ҽ��� �� ���� ���� ��� ��Ʈ �߶󳻱�
		f >>= (b - (36 - dec_size));
	}
	else {	// �����ΰ� ������ �κ��� ������ ��Ʈ�� �Ҽ��� ����� ���� ��� ��Ʈ ����
		f <<= ((36 - dec_size) - b);
	}
	return (s << 47) + (e << 36) + dec + f;	// ǥ���� �ε��Ҽ��� ��ȯ
}

int ComputeLen(char* c) {	// �ƽ�Ű �ڵ峪 16������ ���̸� ���
	unsigned int b;	// ���̸� �����ϱ����� ���� (byte ����)
	char len[32];

	strcpy(len, c);
	if (len[0] == 'C' || len[0] == 'c' && len[1] == '\'') {	// C'�� ������ ���
		for (b = 2; b <= strlen(len); b++) {
			// ���� �б�
			if (len[b] == '\'') {
				b -= 2;	// ������ '�� ������ ��� �� ���̸� �Ǿ� C'�� �α��ڸ� ������ ���̸� ������ �ݺ��� Ż��
				break;
			}
		}
	}
	if (len[0] == 'X' || len[0] == 'x' && len[1] == '\'')	// X'�� ������ ���
		b = 1;	// ������ 1����Ʈ
	return (b);
}

void CreateSymbolTable() {	// �ɺ����̺� ���� ����
	int loop, csect_loop;
	FILE *fptr_sym;
	fptr_sym = fopen("symtab.list", "w");	// �ɺ����̺� ������ ���� ���·� ��

	if (fptr_sym == NULL)
	{
		printf("ERROR: Unable to open the symtab.list.\n");	// �ɺ����̺� ������ �� �� ���� ��� ����ó��
		exit(1);
	}
	
	// ������ ���α׷� ���� ���
	for (csect_loop = 0; csect_loop <= CSectCounter; csect_loop++) {
		// �� Column ���� ������ ���
		// �ܼ�â�� ���Ͽ� ��� ���
		printf("%-10s\t%-4s\n", "LABEL", "LOC");
		fprintf(fptr_sym, "%-10s\t%-4s\n", "LABEL", "LOC");
		for (loop = 0; loop < SymtabCounter[csect_loop]; loop++) {
			// �ɺ����̺��� ���ڵ���� ���� ���
			// �ܼ�â�� ���Ͽ� ��� ���
			printf("%-10s\t%04X\n", SYMTAB[csect_loop][loop].Label, SYMTAB[csect_loop][loop].Address);
			fprintf(fptr_sym, "%-10s\t%04X\n", SYMTAB[csect_loop][loop].Label, SYMTAB[csect_loop][loop].Address);
		}
		printf("\n");
		fprintf(fptr_sym, "\n");
	}
	fclose(fptr_sym);	// ���� ����� �������Ƿ� close
}

void CreateProgramList() {	// ����Ʈ ���� ����
	int loop, csect_loop;
	int len;	// ���ڳ� 16������ ��� ���� ����� ���� ����
	FILE *fptr_list;

	fptr_list = fopen("sic.list", "w");

	if (fptr_list == NULL)
	{
		printf("ERROR: Unable to open the sic.list.\n");	// ����Ʈ ������ �� �� ���� ��� ����ó��
		exit(1);
	}

	for (csect_loop = 0; csect_loop <= CSectCounter; csect_loop++) {
		// ����Ʈ ���� ���� ���
		fprintf(fptr_list, "%-4s\t%-10s%-10s%-10s\t%s\n", "LOC", "LABEL", "OPERATOR", "OPERAND", "OBJECT CODE");	// �� Column ���� ����
		for (loop = 0; loop < ArrayIndex[csect_loop]; loop++)
		{
			len = 0;
			if (strlen(IMRArray[csect_loop][loop]->OperandField) <= 0
				&& !strcmp(IMRArray[csect_loop][loop]->OperatorField, "END")) {
				// END ������ �������� �ǿ����ڰ� ���� ��� �ǳ� �ٱ�
				continue;
			}
			fprintf(fptr_list, "%04X\t%-10s%-10s%-10s\t",
				IMRArray[csect_loop][loop]->Loc,
				IMRArray[csect_loop][loop]->LabelField,
				IMRArray[csect_loop][loop]->OperatorField,
				IMRArray[csect_loop][loop]->OperandField);	// ��� �ڵ���� ����Ǵ� �κ�

			if (!strcmp(IMRArray[csect_loop][loop]->OperatorField, "START")
				|| !strcmp(IMRArray[csect_loop][loop]->OperatorField, "RESW")
				|| !strcmp(IMRArray[csect_loop][loop]->OperatorField, "RESB")
				|| !strcmp(IMRArray[csect_loop][loop]->OperatorField, "END")
				|| !strcmp(IMRArray[csect_loop][loop]->OperatorField, "BASE")
				|| !strcmp(IMRArray[csect_loop][loop]->OperatorField, "EXTREF")
				|| !strcmp(IMRArray[csect_loop][loop]->OperatorField, "EXTDEF"))
				// Object code ����� �ʿ���� �κе� object code ����
				fprintf(fptr_list, "\n");
			else if (SearchOptab(IMRArray[csect_loop][loop]->OperatorField)) {
				// operator�� OPTAB�� ������ ���
				if (OPTAB[Counter].Format == '3') {	// �ش� ��ɾ 3/4���� ��ɾ��� ���
					if (ReadFlag(IMRArray[csect_loop][loop]->OperatorField)) {	// ��ɾ '+'�� ���� ��� (4���� ��ɾ����� �Ǵ��ϱ� ����)
						fprintf(fptr_list, "%08X\n", IMRArray[csect_loop][loop]->ObjectCode);	// 4������ �� 4����Ʈ ���
					}
					else {
						fprintf(fptr_list, "%06X\n", IMRArray[csect_loop][loop]->ObjectCode);	// 3������ �� 3����Ʈ ���
					}
				}
				else if (OPTAB[Counter].Format == '2') {	// �ش� ��ɾ 2���� ��ɾ��� ���
					fprintf(fptr_list, "%04X\n", IMRArray[csect_loop][loop]->ObjectCode);	// 2����Ʈ ���
				}
				else if (OPTAB[Counter].Format == '1') {	// �ش� ��ɾ 1���� ��ɾ��� ���
					fprintf(fptr_list, "%02X\n", IMRArray[csect_loop][loop]->ObjectCode);	// 1����Ʈ ���
				}
			}
			else {
				if (isFloatNum(IMRArray[csect_loop][loop]->OperandField)) {
					// �ε��Ҽ����̹Ƿ� 6����Ʈ�� ǥ��
					fprintf(fptr_list, "%012llX\n", IMRArray[csect_loop][loop]->ObjectCode);
				}
				else {
					// C'XX' Ȥ�� X'XX' �϶� ����ó��
					len = ComputeLen(IMRArray[csect_loop][loop]->OperandField);	// C, ', ' Ȥ�� X, ', '�� ������ ���ҵ��� �����Ʈ���� ����ϱ� ����
					if (len == 1) {	// 1����Ʈ�� ���
						fprintf(fptr_list, "%02X\n", IMRArray[csect_loop][loop]->ObjectCode);	// 1����Ʈ ���
					}
					else if (len == 2) {	// 2����Ʈ�� ���
						fprintf(fptr_list, "%04X\n", IMRArray[csect_loop][loop]->ObjectCode);	// 2����Ʈ ���
					}
					else {
						// �׿��� ���
						fprintf(fptr_list, "%06X\n", IMRArray[csect_loop][loop]->ObjectCode);	// �׿��� ��� object code ����
					}
				}
			}
		}
		fprintf(fptr_list, "\n");
	}
	fclose(fptr_list);	// ����Ʈ ���� ����� ����Ǿ ���� ����
}

void CreateObjectCode() {	// �������� ����
	// �������� ������ ����� �ӽú��� ����
	int first_address;
	int last_address;
	int temp_address;
	unsigned long long int temp_objectcode[30];	// 6����Ʈ �����ڵ带 �������
	int first_index;
	int last_index;
	int x, xx;
	int loop, csect_loop, len = 0;

	char temp_operator[12][10];
	char temp_operand[12][10];

	FILE *fptr_obj;
	fptr_obj = fopen("sic.obj", "w");	// sic.obj ������ �������·� ����
	if (fptr_obj == NULL)
	{
		printf("ERROR: Unable to open the sic.obj.\n");	// ���������� �� �� ���� ��� ����ó��
		exit(1);
	}

	printf("Creating Object Code...\n\n");

	for (csect_loop = 0; csect_loop <= CSectCounter; csect_loop++) {
		loop = 0;	// �ݺ��� ���� �ε��� ����
		if (!strcmp(IMRArray[csect_loop][loop]->OperatorField, "START") || !strcmp(IMRArray[csect_loop][loop]->OperatorField, "CSECT"))
			// �߰������� ù��° ���Ұ� CSECT Ȥ�� START�� ��
		{
			// ��� ���ڵ� �ۼ� (���α׷� �̸�, �����ּ�, ���α׷� ����)
			// �ܼ�â�� ���� �Ѵ� ���
			printf("H%-6s%06X%06X\n", IMRArray[csect_loop][loop]->LabelField, start_address[csect_loop], program_length[csect_loop]);
			fprintf(fptr_obj, "H^%-6s^%06X^%06X\n", IMRArray[csect_loop][loop]->LabelField, start_address[csect_loop], program_length[csect_loop]);
			loop++;
		}

		if (ExtDefCounter[csect_loop] > 0) {
			// ����� ���� ���ڵ尡 ���� ��� ���� ���ڵ� ���
			printf("D");
			fprintf(fptr_obj, "D");
			for (x = 0; x < ExtDefCounter[csect_loop]; x++) {
				// ���� ���ڵ� �ۼ� (���̺� �̸�, �ּ�)
				// �ܼ�â�� ���� �Ѵ� ���
				printf("%-6s%06X", ExtDefTAB[csect_loop][x].Label, ExtDefTAB[csect_loop][x].Address);
				fprintf(fptr_obj, "^%-6s^%06X", ExtDefTAB[csect_loop][x].Label, ExtDefTAB[csect_loop][x].Address);
			}
			printf("\n");
			fprintf(fptr_obj, "\n");
		}

		if (ExtRefCounter[csect_loop] > 0) {
			// ����� ���� ���ڵ尡 ���� ��� ���� ���ڵ� ���
			printf("R");
			fprintf(fptr_obj, "R");
			for (x = 0; x < ExtRefCounter[csect_loop]; x++) {
				// ���� ���ڵ� �ۼ� (�ܺ� ���� ���̺� �̸�)
				// �ܼ�â�� ���� �Ѵ� ���
				printf("%-6s", ExtRefTAB[csect_loop][x].Label);
				fprintf(fptr_obj, "^%-6s", ExtRefTAB[csect_loop][x].Label);
			}
			printf("\n");
			fprintf(fptr_obj, "\n");
		}

		while (1)	// ���ѷ��� ����
		{
			first_address = IMRArray[csect_loop][loop]->Loc;	// ������ �����ּҸ� ����
			last_address = IMRArray[csect_loop][loop]->Loc + 29;	// 1D���� ����Ʈ�� ����� �� �����Ƿ� �ִ� 29 ����Ʈ ����ϴ� �Ѱ� ����
			first_index = loop;	// �ݺ����� �������� ù��° �ε������� �ʱ�ȭ

			// ����� �� �ִ� ���� ���
			for (x = 0, temp_address = first_address; temp_address <= last_address; loop++) {
				// END ������ �����ڰ� �����ų� ���ٿ� ����� �� �ִ� ���� �Ѱ迡 �������� �� for�� ����
				// x : �� �پȿ� ����� �� �ִ� �����ڵ� �ִ� ����

				if (!strcmp(IMRArray[csect_loop][loop]->OperatorField, "END")) {// END ������ �����ڸ� ������ for�� Ż��
					
					break;
				} else if (strcmp(IMRArray[csect_loop][loop]->OperatorField, "RESB")
					&& strcmp(IMRArray[csect_loop][loop]->OperatorField, "RESW")
					&& strcmp(IMRArray[csect_loop][loop]->OperatorField, "BASE")
					&& strcmp(IMRArray[csect_loop][loop]->OperatorField, "NOBASE")
					&& strcmp(IMRArray[csect_loop][loop]->OperatorField, "EXTREF")
					&& strcmp(IMRArray[csect_loop][loop]->OperatorField, "EXTDEF")) {
					// �����ڵ尡 ���� ������ �����ڸ� ������ ���������� ����ϱ� ���� ����
					// temp_objectcode : �����ڵ带 ����
					// temp_operator : Operator Mnemonic ����
					// temp_operand : Operand Mnemonic ����
					temp_objectcode[x] = IMRArray[csect_loop][loop]->ObjectCode;
					strcpy(temp_operator[x], IMRArray[csect_loop][loop]->OperatorField);
					strcpy(temp_operand[x], IMRArray[csect_loop][loop]->OperandField);
					last_index = loop + 1;	// ���ٿ� ǥ���� �� �ִ� �����ڵ��� ���̸� ����ϱ� ���� ����
					x++; // ��ɾ� ���� 1 ����
				}
				// ������ ��ɾ��� �������� �Ѱ������� �˻��ϱ� ���� ����
				temp_address = IMRArray[csect_loop][loop + 1]->Loc;
				// ������ ��ɾ ��µǾ ����� �� �ִ� ���� �Ѱ踦 ���� �ʴ� �� �˻� (������ �ٸ� �� �ֱ� ����)
				if (SearchOptab(IMRArray[csect_loop][loop + 1]->OperatorField)) {
					if (ReadFlag(IMRArray[csect_loop][loop + 1]->OperatorField)) {	// 4���� ��ɾ��� ���
						temp_address += 1;	// 1����Ʈ �߰�
					}
					temp_address += OPTAB[Counter].Format - '0';	// ���� ��ɾ� ���ĸ�ŭ �߰�
				}
				else {
					if (!strcmp(IMRArray[csect_loop][loop + 1]->OperatorField, "WORD")
						|| !strcmp(IMRArray[csect_loop][loop + 1]->OperatorField, "BYTE")) {
						if (isFloatNum(IMRArray[csect_loop][loop + 1]->OperandField)) {
							// �ε��Ҽ����� ��� 6����Ʈ �߰�
							temp_address += 6;
						}
						else if (!strcmp(IMRArray[csect_loop][loop + 1]->OperatorField, "BYTE")) {
							// C'XX' Ȥ�� X'XX' �� ��� ��µǾ ������ �� �˻��ϱ� ����
							temp_address += ComputeLen(IMRArray[csect_loop][loop + 1]->OperandField);
						}
						else if (!strcmp(IMRArray[csect_loop][loop + 1]->OperatorField, "WORD")) {
							temp_address += 3;
						}
					}
				}
			}

			// �ؽ�Ʈ ���ڵ�� ������ �����ּҿ� �����ڵ��� ���� ����ؼ� ���
			// �ܼ�â�� ���Ͽ� ��� ���
			if ((IMRArray[csect_loop][last_index]->Loc - IMRArray[csect_loop][first_index]->Loc) == 0) {
				if (!strcmp(IMRArray[csect_loop][loop]->OperatorField, "END"))	// END ������ �����ڸ� ������ ��� while�� Ż��
					break;
				else
					continue;
			}
			printf("T%06X%02X", first_address, (IMRArray[csect_loop][last_index]->Loc - IMRArray[csect_loop][first_index]->Loc));
			fprintf(fptr_obj, "T^%06X^%02X", first_address, (IMRArray[csect_loop][last_index]->Loc - IMRArray[csect_loop][first_index]->Loc));

			for (xx = 0; xx < x; xx++) {
				// ���ٿ� �� �� �ִ� �ִ��� �����ڵ带 ����ϱ� ���� �ݺ���
				// �ܼ�â�� ���Ͽ� ��� ���
				if ((strcmp(temp_operator[xx], "BYTE") == 0)) {
					if (temp_operand[xx][0] == 'X' || temp_operand[xx][0] == 'x') {
						// 16������ ǥ���� 1����Ʈ�� ���� ��� 1����Ʈ�� ���Ŀ� ���� ���
						printf("%02X", temp_objectcode[xx]);
						fprintf(fptr_obj, "^%02X", temp_objectcode[xx]);
					}
					else if (isFloatNum(temp_operand[xx])) {
						// �ε��Ҽ����� ��� 6����Ʈ�� ���Ŀ� ���� ���
						printf("%012llX", temp_objectcode[xx]);
						fprintf(fptr_obj, "^%012llX", temp_objectcode[xx]);
					}
					else {
						printf("%06X", temp_objectcode[xx]);
						fprintf(fptr_obj, "^%06X", temp_objectcode[xx]);
					}
				}
				else {
					// ��ɾ��� ���Ŀ� ���� �޶����� ���̿� ���缭 ���
					if (SearchOptab(temp_operator[xx])) {
						// operator�� OPTAB�� ������ ���
						if (OPTAB[Counter].Format == '3') {	// �ش� ��ɾ 3/4���� ��ɾ��� ���
							if (ReadFlag(temp_operator[xx])) {	// ��ɾ '+'�� ���� ��� (4���� ��ɾ����� �Ǵ��ϱ� ����)
								// 4������ �� 4����Ʈ�� ���
								printf("%08X", temp_objectcode[xx]);
								fprintf(fptr_obj, "^%08X", temp_objectcode[xx]);
							}
							else {
								// 3������ �� 3����Ʈ�� ���
								printf("%06X", temp_objectcode[xx]);
								fprintf(fptr_obj, "^%06X", temp_objectcode[xx]);
							}
						}
						else if (OPTAB[Counter].Format == '2') {
							// 2���� ��ɾ��� ��� 2����Ʈ�� ���
							printf("%04X", temp_objectcode[xx]);
							fprintf(fptr_obj, "^%04X", temp_objectcode[xx]);
						}
						else if (OPTAB[Counter].Format == '1') {
							// 1���� ��ɾ��� ��� 1����Ʈ�� ���
							printf("%02X", temp_objectcode[xx]);
							fprintf(fptr_obj, "^%02X", temp_objectcode[xx]);
						}
					}
					else {
						if (isFloatNum(temp_operand[xx])) {
							// �ε��Ҽ����� ��� 6����Ʈ�� ���Ŀ� ���� ���
							printf("%012llX", temp_objectcode[xx]);
							fprintf(fptr_obj, "^%012llX", temp_objectcode[xx]);
						}
						else {
							// WORD�� ���
							printf("%06X", temp_objectcode[xx]);
							fprintf(fptr_obj, "^%06X", temp_objectcode[xx]);
						}
					}
				}
			}

			// ���� ����� ���� �� ����
			// �ܼ�â�� ���Ͽ� ��� ���
			printf("\n");
			fprintf(fptr_obj, "\n");

			if (!strcmp(IMRArray[csect_loop][loop]->OperatorField, "END"))	// END ������ �����ڸ� ������ ��� while�� Ż��
				break;
		}

		// �������ڵ� ��ºκ�
		// RLD �κ�
		for (loop = 0; loop < RLDCounter[csect_loop]; loop++) {
			// RLD�� ��� ���ڵ���� �������ڵ�� ���
			// ���ġ�� �ʿ����� �δ����� �˸�������
			printf("M%06X%02X\n", RLDArray[csect_loop][loop].Address, RLDArray[csect_loop][loop].Nibbles);
			fprintf(fptr_obj, "M^%06X^%02X\n", RLDArray[csect_loop][loop].Address, RLDArray[csect_loop][loop].Nibbles);
		}
		// ESD �κ�
		for (loop = 0; loop < ESDCounter[csect_loop]; loop++) {
			// ESD�� ��� ���ڵ���� �������ڵ�� ���
			// ��ŷ�� ���ġ�� �ʿ����� �δ����� �˸��� ����
			printf("M%06X%02X%c%s\n", ESDArray[csect_loop][loop].Address, ESDArray[csect_loop][loop].Nibbles, ESDArray[csect_loop][loop].sign, ESDArray[csect_loop][loop].Label);
			fprintf(fptr_obj, "M^%06X^%02X^%c%s\n", ESDArray[csect_loop][loop].Address, ESDArray[csect_loop][loop].Nibbles, ESDArray[csect_loop][loop].sign, ESDArray[csect_loop][loop].Label);
		}

		// ���� ���ڵ带 ���� ���α׷��� �����ּҸ� ���
		// �ܼ�â�� ���Ͽ� ��� ���
		printf("E");
		fprintf(fptr_obj, "E");
		if (SearchSymtab(End_operand, csect_loop)) {
			printf("%06X\n\n", SYMTAB[csect_loop][SymIdx].Address);
			fprintf(fptr_obj, "^%06X\n\n", SYMTAB[csect_loop][SymIdx].Address);
		} else {
			printf("\n\n");
			fprintf(fptr_obj, "\n\n");
		}
	}
	fclose(fptr_obj);	// obj ���� ���� ����
}

/******************************* MAIN FUNCTION *******************************/
void main(void)
{
	FILE* fptr;

	char filename[10];
	char label[32];
	char opcode[32];
	char operand[32];
	char tempLabel[LABEL_LENGTH];	// �ܺ� ����, �ܺ� ���� ���̺���� ��� ���� �ӽ� ����

	int loc = 0;
	int line = 0;
	int loop, csect_loop;
	int is_empty_line;
	int is_comment;
	int loader_flag = 0;
	int start_line = 0;
	int tempLabelIdx = 0;
	int i = 0;
	// Intro Part
	printf(" ******************************************************************************\n");
	printf(" * Program: SIC ASSEMBYER                                                     *\n");
	printf(" *                                                                            *\n");
	printf(" * Procedure:                                                                 *\n");
	printf(" *   - Enter file name of source code.                                        *\n");
	printf(" *   - Do pass 1 process.                                                     *\n");
	printf(" *   - Do pass 2 process.                                                     *\n");
	printf(" *   - Create \"program list\" data on sic.list.(Use Notepad to read this file) *\n");
	printf(" *   - Create \"object code\" data on sic.obj.(Use Notepad to read this file)   *\n");
	printf(" *   - Also output object code to standard output device.                     *\n");
	printf(" ******************************************************************************\n");


	printf("\nEnter the file name you want to assembly (sic.asm):");
	//scanf("%s", filename);
	fptr = fopen("fig2_1.asm", "r");
	if (fptr == NULL)	// �ҽ��ڵ� ���� �б� �������� ��� ����ó��
	{
		printf("ERROR: Unable to open the %s file.\n", filename);
		exit(1);
	}

	/********************************** PASS 1 ***********************************/
	printf("Pass 1 Processing...\n");
	while (fgets(Buffer, 256, fptr) != NULL)	// �ҽ��ڵ� ���Ͽ��� �ڵ� �б�
	{
		is_empty_line = strlen(Buffer);

		Index = 0;
		j = 0;
		strcpy(label, ReadLabel());
		if (Label[0] == '.')	// �ش� �ҽ��ڵ尡 �ּ����� �ƴ��� Ȯ��
			is_comment = 1;
		else
			is_comment = 0;

		if (is_empty_line>1 && is_comment != 1)
		{
			// �ε��� ������ �ʱ�ȭ
			Index = 0;
			j = 0;

			IMRArray[CSectCounter][ArrayIndex[CSectCounter]] = (IntermediateRec*)malloc(sizeof(IntermediateRec));/* [A] */	// �߰����� �����Ҵ�
			IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->LineIndex = ArrayIndex;	// �ҽ��ڵ� ���� �� ����
			strcpy(label, ReadLabel());	// ���̺��� �о� Label�� ����
			strcpy(IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->LabelField, label);	// ���̺��� �߰����Ͽ� ����
			SkipSpace();	// ���� ����

			if (line == start_line)	// ���α׷��� ���� ������ ù���� �ƴ� ��� (ù��° ���� �ּ��� ���) ����ó��
			{
				strcpy(opcode, ReadOprator());	// Mnemonic �б�
				strcpy(IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->OperatorField, opcode); /* [A] */	// ���� Mnemonic�� �߰����Ͽ� ����
				if (!strcmp(opcode, "START")) {	// �����ּ� �ʱ�ȭ
					SkipSpace();
					strcpy(operand, ReadOperand());
					strcpy(IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->OperandField, operand);/* [A] */
					LOCCTR[LocctrCounter] = StrToHex(operand);
					start_address[CSectCounter] = LOCCTR[LocctrCounter];
				} else {	// ���� �ּҰ� ��õǾ����� ���� ��� 0���� �ʱ�ȭ
					LOCCTR[LocctrCounter] = 0;
					start_address[CSectCounter] = LOCCTR[LocctrCounter];
				}
			} else {
				strcpy(opcode, ReadOprator());	// OP Code �б�
				strcpy(IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->OperatorField, opcode);	// �߰����Ͽ� OP code ����
				SkipSpace();	// OP code�� �ǿ����� ������ ���� ����
				strcpy(operand, ReadOperand());	// �ǿ����� �κ� �б�
				strcpy(IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->OperandField, operand);	// �߰����Ͽ� �ǿ����� ����

				if (strcmp(opcode, "END"))	// OP code�� END ������ �����ڰ� �ƴ� ���
				{
					if (label[0] != '\0')	// ���̺��� ���� ���
					{
						if (SearchSymtab(label, CSectCounter))	// ���� �̸��� ���̺��� �ִ��� ã��
						{
							// ���� ���� �̸��� ���̺��� ���� ��� Alert�ϰ� ���α׷� ����
							fclose(fptr);
							printf("ERROR: Duplicate Symbol\n");
							FoundOnSymtab_flag = 0;
							exit(1);
						}
						if (strcmp(opcode, "CSECT")) {	// CSECT�� �ƴ� ��쿡�� �ɺ����̺�� �߰�
							RecordSymtab(label);	// �����̸��� �����Ƿ� �ɺ����̺�� �߰�
						}
					}

					if (SearchOptab(opcode)) {	// OP Code�� OPTAB�� ���� ��� ��ɾ� ���ĸ�ŭ �޸� Ȯ��
						LOCCTR[LocctrCounter] = loc + (int)(OPTAB[Counter].Format - '0');
						if (ReadFlag(opcode)) {
							// 4���� ��ɾ��� ���
							LOCCTR[LocctrCounter] += 1;	// ������ 1����Ʈ �� �߰�
						}
					}
					else if (!strcmp(opcode, "WORD")) {	// 3����Ʈ(1 WORD) Ȯ��
						if (isFloatNum(operand)) {
							// �ε��Ҽ����� 6����Ʈ ����ϹǷ�
							LOCCTR[LocctrCounter] = loc + 6;
						} else {
							LOCCTR[LocctrCounter] = loc + 3;
						}
					} else if (!strcmp(opcode, "RESW"))	// �ǿ����� ������ WORD ��ŭ �޸� Ȯ��
						LOCCTR[LocctrCounter] = loc + 3 * StrToDec(operand);
					else if (!strcmp(opcode, "RESB"))	// �ǿ����� ������ ����Ʈ��ŭ �޸� Ȯ��
						LOCCTR[LocctrCounter] = loc + StrToDec(operand);
					else if (!strcmp(opcode, "BYTE")) {	// 1����Ʈ Ȯ��
						if (isFloatNum(operand)) {
							// �ε��Ҽ����� 6����Ʈ ����ϹǷ�
							LOCCTR[LocctrCounter] = loc + 6;
						} else {
							LOCCTR[LocctrCounter] = loc + ComputeLen(operand);
						}
					} else if (!strcmp(opcode, "BASE")
						|| !strcmp(opcode, "NOBASE")) {
						// ���޸� ó���� �ʿ��� Assembler Directive�� �ƴ� ��� Loc�� ����
						LOCCTR[LocctrCounter] = loc;
					}
					else if (!strcmp(opcode, "EXTDEF")
						|| !strcmp(opcode, "EXTREF")) {
						// �ܺ� ����, ���� ���̺��� ��� , ������ ���̺�� �߰�
						i = 0; tempLabelIdx = 0;
						while (1) {
							if (operand[i] == ',' || operand[i] == '\0') {
								// , Ȥ�� \0�� ���� ��� ���
								tempLabel[tempLabelIdx] = '\0';
								// ���� �ٸ� ���̺�� ���
								if (!strcmp(opcode, "EXTDEF")) {
									RecordEXTDEF(tempLabel);
								}
								else if (!strcmp(opcode, "EXTREF")) {
									RecordEXTREF(tempLabel);
								}
								tempLabelIdx = 0;
								if (operand[i] == '\0') break;	// \0�� ������ ��� break;
							}
							else {
								tempLabel[tempLabelIdx++] = operand[i];
							}
							i++;
						}
						LOCCTR[LocctrCounter] = loc;
					}
					else if (!strcmp(opcode, "CSECT")) {
						// ������ �ۼ��� �ɺ����̺��� ������� �ܺ� ���� ���̺��� ��ġ�� ���
						if (RecordEXTDEFLoc()) {
							printf("ERROR: Isn't exist External Define Label\n");	// EXTDEF �߿� �ɺ����̺���� ã�� �� ���� ���̺��� ���� ��� ����ó��
							fclose(fptr);
							exit(1);
						}
						
						// ���� END ������ ������ �߰�
						IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->LabelField[0] = '\0';
						IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->Loc = LOCCTR[LocctrCounter - 1];
						strcpy(IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->OperatorField, "END");
						strcpy(IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->OperandField, "");
						program_length[CSectCounter] = LOCCTR[LocctrCounter - 1] - LOCCTR[0];
						ArrayIndex[CSectCounter]++;
						
						// ���ο� ���α׷��� �б� ���� ���� �ʱ�ȭ
						CSectCounter++;
						line = 1;
						loc = 0;
						LocctrCounter = 0;
						LOCCTR[LocctrCounter] = 0;
						FoundOnOptab_flag = 0;	// flag ���� �ʱ�ȭ
						
						// ���ο� �߰����� ���� �� �ʱ�ȭ
						IMRArray[CSectCounter][ArrayIndex[CSectCounter]] = (IntermediateRec *)malloc(sizeof(IntermediateRec));
						strcpy(IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->LabelField, label);
						strcpy(IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->OperandField, "");
						strcpy(IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->OperatorField, opcode);
						IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->LineIndex = 0;
						IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->Loc = 0;
						RecordSymtab(label);	// �����̸��� �����Ƿ� �ɺ����̺�� �߰�
						start_address[CSectCounter] = LOCCTR[LocctrCounter];
						LocctrCounter++;
						ArrayIndex[CSectCounter]++;
						continue;
					}
					else { // ���ǵ��� ���� OP code�̹Ƿ� ����� ���α׷� ����
						fclose(fptr);
						printf("ERROR: Invalid Operation Code[%s]\n", opcode);
						exit(1);
					}
				}
				else {
					// END Assembler Directive�� ������ ��� �ǿ����� ����
					strcpy(End_operand, operand);
				}
			}
			loc = LOCCTR[LocctrCounter];	// loc�� �ٽ� �����ϰ� ���� ������ �غ�
			IMRArray[CSectCounter][ArrayIndex[CSectCounter]]->Loc = LOCCTR[LocctrCounter - 1];	// �߰����Ͽ� �ش� �ڵ��� �޸� ���� ���
			LocctrCounter++;	// LOCCTR�� �����ϴ� �ε��� ���� �� ����
			ArrayIndex[CSectCounter]++;	// ���� �ڵ带 �б� ���� �߰������� �ε��� ���� �� ����
		}
		
		if (is_comment == 1) {	// ù ���� �ּ��� ��� ������ ���� �ʴ� ���� ����
			start_line += 1;
		}

		FoundOnOptab_flag = 0;	// flag ���� �ʱ�ȭ
		line += 1;	// �ҽ� �� 1 ����
	}
	program_length[CSectCounter] = LOCCTR[LocctrCounter - 2] - LOCCTR[0];
	// END �����ڸ� ������ ��� END ������ �ٷ� ���� �ҽ��ڵ��� �޸� ��ġ�� �����ּҸ� ���� �� ���α׷� ���� ���

	if (RecordEXTDEFLoc()) {
		printf("ERROR: Isn't exist External Define Label\n");	// EXTDEF �߿� �ɺ����̺���� ã�� �� ���� ���̺��� ���� ��� ����ó��
		fclose(fptr);
		exit(1);
	}

	// Pass 1���� ������ �ɺ����̺� ���
	CreateSymbolTable();

	/********************************** PASS 2 ***********************************/
	printf("Pass 2 Processing...\n");

	unsigned long inst_fmt;		// ���� ���� �ڵ�
	unsigned long inst_fmt_opcode;	// �����ڵ��� op code �κ��� ��Ÿ���� ����
	unsigned long inst_fmt_sign;	// Immediate, Indirect Addressing Mode�� ��Ÿ���� �÷��׺�Ʈ ���� 
	unsigned long inst_fmt_relative;	// relative addressing mode�� ��Ÿ���� �÷��׺�Ʈ ����
	unsigned long inst_fmt_index;	// index Addressing Mode�� ��Ÿ���� ����
	unsigned long inst_fmt_extended;	// �÷��׺�Ʈ e�� ��Ÿ���� �÷��׺�Ʈ ����
	unsigned long inst_fmt_address;	// �ǿ����� �κ��� ��Ÿ���� ���� (�ʿ信 ���ؼ� �������� ����� �� ���� �ִ�. ex. Immediate Addressing Mode)
	int inst_fmt_byte;		// ������ ��ɾ����� ��Ÿ���� ���� (����Ʈ ��)
	int regCharIdx;	// �ε��� ������
	char regName[3];	// �������� �̸��� ���ϱ����� ��Ƴ��� �ӽú���
	
	int diff = 0;	// �ּҿ� �� ������ �����ϴ� ���� (������ ���� �� �����Ƿ� unsigned �� �ƴϴ�)
	int base_register = -1;	// BASE ������ �����ڰ� ������ ���� ��� base relative addressing mode�� ������� ���ϰ� �ϵ��� �ϱ� ���� �⺻�� -1

	for (csect_loop = 0; csect_loop <= CSectCounter; csect_loop++) {
		base_register = -1;
		diff = 0;
		for (loop = 1; loop < ArrayIndex[csect_loop]; loop++) {	// �߰������� ���������� ����
			// �� ������ �ʱ�ȭ
			inst_fmt_opcode = 0;
			inst_fmt_sign = 0;
			inst_fmt_relative = 0;
			inst_fmt_index = 0;
			inst_fmt_extended = 0;
			inst_fmt_address = 0;
			inst_fmt_byte = 0;
			regName[0] = '\0';
			tempLabel[0] = '\0';

			strcpy(opcode, IMRArray[csect_loop][loop]->OperatorField);	// op code �κ� ����

			if (SearchOptab(opcode)) {	// opcode ã��
				if (!strcmp(OPTAB[Counter].Mnemonic, "RSUB")) {
					// RSUB�� 3���� ��ɾ������� ����� �����Ƿ�
					IMRArray[csect_loop][loop]->ObjectCode = (OPTAB[Counter].ManchineCode << 16);
					continue;
				}
				inst_fmt_opcode = OPTAB[Counter].ManchineCode;	// opcode�� �����ڵ� ����
				inst_fmt_byte = OPTAB[Counter].Format - '0';	// �ش� ��ɾ �� ����Ʈ�� ����ϴ� �� ����
				if (inst_fmt_byte == 3 && ReadFlag(opcode)) {	// ���� 4���� ��ɾ��� ��� �б�ó��
					inst_fmt_byte = 4;	// 4���� ��ɾ�
					inst_fmt_extended = 0x00100000;	// �÷��� ��Ʈ e�� 1��.
				}
				inst_fmt_opcode <<= (8 * (inst_fmt_byte - 1));	// �� ��ɾ� ���Ŀ� �°� �������� Shift
				IMRArray[csect_loop][loop]->ObjectCode = inst_fmt_opcode;
				strcpy(operand, IMRArray[csect_loop][loop]->OperandField);

				if (ReadFlag(operand)) {
					if (inst_fmt_byte <= 2) {
						fclose(fptr);
						printf("ERROR: Invalid Addressing Mode\n");
						exit(1);
					}
					if (Flag == SHARP) {	// Immediate Addressing Mode
						inst_fmt_sign = 0x010000;
					}
					else if (Flag == AT) {	// Indirect Addressing Mode
						inst_fmt_sign = 0x020000;
					}
					inst_fmt_sign <<= 8 * (inst_fmt_byte - 3);	// ����Ʈ �� ��ŭ �������� Shift
				}
				else if (inst_fmt_byte >= 3) {	// 3/4���� ��ɾ� Simple Addressing Mode
					inst_fmt_sign = 0x030000;
					inst_fmt_sign <<= 8 * (inst_fmt_byte - 3);	// ����Ʈ �� ��ŭ �������� Shift
				}

				if (inst_fmt_byte >= 3) {
					// 3/4���� ��ɾ� �ϰ��
					if (operand[strlen(operand) - 2] == ',' && operand[strlen(operand) - 1] == 'X') {	// index addressing Mode
						inst_fmt_index = 0x008000;	// index addressing mode �÷��� ��Ʈ x�� 1
						inst_fmt_index <<= 8 * (inst_fmt_byte - 3);	// 4���� ��ɾ��� ��� 8��Ʈ�� �������� �̵�
						operand[strlen(operand) - 2] = '\0';	// ,X �κ� ����
					}

					if (SearchSymtab(operand, csect_loop)) {
						// �ɺ����̺���� �ش� �ǿ����ڸ� ã�� �� ���� ���
						if (inst_fmt_byte == 4) {	// extended instruction�� �ּ� ����
							inst_fmt_address = SYMTAB[csect_loop][SymIdx].Address;
							RecordRLD(IMRArray[csect_loop][loop]->OperatorField, IMRArray[csect_loop][loop]->Loc, csect_loop);	// ���ġ�� �ʿ��ϹǷ� RLD�� �߰�
						}
						else {	// relative Addressing mode
							// PC�� �� ����
							// ��ɾ� ���� �������� PC�� �� ��� (���� ��ɾ��� �޸� ��ġ)
							diff = SYMTAB[csect_loop][SymIdx].Address - IMRArray[csect_loop][loop]->Loc - inst_fmt_byte;
							if (diff >= -2048 && diff < 2048) {
								// pc relative�� ���
								// ������ 12��Ʈ�� ǥ���ؾ��ϹǷ� 12��Ʈ�� ������ ��ȯ���ֱ� ���� �Լ� ȣ��
								// ������ �ƴ϶�� �Լ� ���� ������ �״�� ��ȯ�ǹǷ� ������ �ƴϾ �Լ��� ȣ��
								inst_fmt_address = 0x002000;
								inst_fmt_address += ConvertNumber(diff, 3);
							}
							else {	// PC relative addressing mode�� �������� ��� Base relative addressing mode �õ�
								// base relative addressing mode�� ����Ͽ� ���� �ٽ� ���
								diff = SYMTAB[csect_loop][SymIdx].Address - base_register;
								if (base_register != -1 && diff >= 0 && diff < 4096) {	// Base relative addressing mode�� �����Ұ��
									// base relative addressing mode�� �����
									inst_fmt_address = 0x004000;
									inst_fmt_address += diff;
								}
								else {
									// pc Ȥ�� base relative addressing mode�� ����� �Ұ��� ��� ����ó��
									fclose(fptr);
									printf("ERROR: CANNOT present relative addressing mode[line : %d]\n", IMRArray[csect_loop][loop]->LineIndex);
									exit(1);
								}
							}
						}
					}
					else {
						// �ɺ����̺���� �ǿ����ڸ� ã�� �� ���� ��
						ReadFlag(operand);	// �ǿ����ڿ� �پ��ִ� �÷��װ� #���� �˻��ϱ����� 
						if (Flag == SHARP && isNum(operand)) {	// �ǿ����ڰ� ���ڷ� �̷�����ְ�, immediate addressing mode���� �˻�
							inst_fmt_address = ConvertNumber(StrToDec(operand), (inst_fmt_byte == 4) ? 5 : 3);	// �ǿ����ڰ� ����(������)�� �̷���� �����Ƿ� �� ���� �ּҿ� ����
						}
						else if (SearchExtRefTAB(operand, csect_loop)) {
							// �ܺ� ���� ���̺��� ���
							inst_fmt_address = 0;
							RecordESD(operand, IMRArray[csect_loop][loop]->Loc, inst_fmt_byte, csect_loop);
						}
						else {
							// �ܺ����� ���̺�� �ƴϰ� �ɺ����̺���� �ǿ����ڸ� ã�� �� ���� ���ڷ� �̷���������� �ʱ� ������
							fclose(fptr);
							printf("ERROR: Label isn't exist [%s]\n", operand);
							exit(1);
						}
					}
				}
				else if (inst_fmt_byte == 2) {	// 2���� ��ɾ��� ���
					i = 0; regCharIdx = 0;	// �ε��� ������ �ʱ�ȭ
					do {	// �ǿ����ڸ� �о� �������͵鿡 �´� �����ڵ� �ۼ�
						if (operand[i] == ',' || operand[i] == '\0') {	// �ռ� ���� �������͸� ���� �غ� �Ǿ��� ���
							regName[regCharIdx] = '\0';	// �ܼ� ���ڹ迭�� ���ڿ��� ����
							if (operand[i] == ',') {	// ������ ��ϵ� ���������� ���̵� �����Ұ�� 4��Ʈ�� �������� �а� ���
								inst_fmt_address <<= 4;
							}

							if (SearchRegTab(regName)) {	// �̸� ���ǵ� �������� ���̺���� ����
								inst_fmt_address += REG_TAB[RegIdx].id;	// �������� ���̺�� �ش� �������Ͱ� ���� ��� �� ���̵� �����ڵ忡 �߰� 
							}
							else {
								if (!strcmp(OPTAB[Counter].Mnemonic, "SVC")
									|| !strcmp(OPTAB[Counter].Mnemonic, "SHIFTL")
									|| !strcmp(OPTAB[Counter].Mnemonic, "SHIFTR")) {
									// �ǿ����ڷ� �������͸� ��������ʰ� ���ڸ� ����ϴ� ���
									if (isNum(regName)) {	// �ǿ����ڰ� ���ڶ��
										inst_fmt_address += StrToDec(regName);	// �߰�
									}
								}
								else { // RegTab�� ���� ������ ������ ó���ϰ� ���α׷� ����
									fclose(fptr);
									printf("ERROR: Invalid Register\n");
									exit(1);
								}
							}
							regCharIdx = 0;	// �ε��� ���� �ʱ�ȭ
						}
						else if (operand[i] != ' ') {	// ������ ��� ��ŵ�ϵ���
							regName[regCharIdx++] = operand[i];	// �������� �̸� ����
						}
					} while (operand[i++] != '\0');

					if (!strcmp(OPTAB[Counter].Mnemonic, "CLEAR")
						|| !strcmp(OPTAB[Counter].Mnemonic, "TIXR")
						|| !strcmp(OPTAB[Counter].Mnemonic, "SVC")) {
						// �ǿ����� ������ �ٸ� Ư�� ��ɾ�鿡 ���� ����ó��
						// �ǿ����ڰ� 1�� �ϰ�� ����� 4��Ʈ �������� �̵�
						inst_fmt_address <<= 4;
					}
				}

				// Object Code ����
				inst_fmt = inst_fmt_opcode + inst_fmt_sign + inst_fmt_index + inst_fmt_relative + inst_fmt_extended + inst_fmt_address;
				IMRArray[csect_loop][loop]->ObjectCode = inst_fmt;
			}
			else if (!strcmp(opcode, "WORD")) {
				// 1 WORD�� ũ�⿡ 10���� ����
				strcpy(operand, IMRArray[csect_loop][loop]->OperandField);
				if (isFloatNum(operand)) {
					// �ε��Ҽ����̹Ƿ� �ε��Ҽ��� ���
					IMRArray[csect_loop][loop]->ObjectCode = ConvertFloatNum(operand);
				}
				else {
					IMRArray[csect_loop][loop]->ObjectCode = StrToDec(operand);
				}
			}
			else if (!strcmp(opcode, "BYTE")) {
				// 1 Byte�� ���� ������ Ȥ�� �Ѱ� ���� (ASCII code Ȥ�� 16����)
				strcpy(operand, IMRArray[csect_loop][loop]->OperandField);
				IMRArray[csect_loop][loop]->ObjectCode = 0;

				if (isFloatNum(operand)) {
					// �ε��Ҽ����̹Ƿ� �ε��Ҽ��� ���
					IMRArray[csect_loop][loop]->ObjectCode = ConvertFloatNum(operand);
				}
				else {
					// ���� ASCII code�� ��� �� ����� objectcode�� ����
					if (operand[0] == 'C' || operand[0] == 'c' && operand[1] == '\'') {
						for (int x = 2; x <= (int)(strlen(operand) - 2); x++) {
							IMRArray[csect_loop][loop]->ObjectCode += (int)operand[x];
							IMRArray[csect_loop][loop]->ObjectCode <<= 8;
						}
					}

					// ���� 16������ ��� �� ����� objectcode�� ����
					if (operand[0] == 'X' || operand[0] == 'x' && operand[1] == '\'') {
						char *operand_ptr;
						operand_ptr = &operand[2];
						*(operand_ptr + 2) = '\0';
						for (int x = 2; x <= (int)(strlen(operand) - 2); x++) {
							IMRArray[csect_loop][loop]->ObjectCode += StrToHex(operand_ptr);
							IMRArray[csect_loop][loop]->ObjectCode <<= 8;
						}
					}

					IMRArray[csect_loop][loop]->ObjectCode >>= 8;	// �� �ݺ��� �Ǹ������� 1����Ʈ �о��� ���� �ٽ� ����ġ�� ����
				}
			}
			else if (!strcmp(opcode, "BASE")) {
				// BASE ������ �������� ��� �ش� ��ġ�� base �������Ϳ� �ְ� base relative addressing mode�� ���������� ����
				strcpy(operand, IMRArray[csect_loop][loop]->OperandField);
				IMRArray[csect_loop][loop]->ObjectCode = 0;
				if (SearchSymtab(operand, csect_loop)) {
					base_register = SYMTAB[csect_loop][SymIdx].Address;
				}
				else {
					// �ǿ����ڰ� SymTab�� ���� ������ ������ ó���ϰ� ���α׷� ���� 
					fclose(fptr);
					printf("ERROR: No Label in SYMTAB[%s]\n", operand);
					exit(1);
				}
			}
			else if (!strcmp(opcode, "NOBASE")) {
				IMRArray[csect_loop][loop]->ObjectCode = 0;
				// base register ����
				base_register = -1;
			}
		}
	}

	// ����Ʈ ���ϰ� ���� ���� ����
	CreateProgramList();
	CreateObjectCode();

	// �޸� �����Ҵ� ����
	for (csect_loop = 0; csect_loop <= CSectCounter; csect_loop++) {
		for (loop = 0; loop < ArrayIndex[csect_loop]; loop++)
			free(IMRArray[csect_loop][loop]);
	}
	printf("Compeleted Assembly\n");
	fclose(fptr); // �ҽ��ڵ� ���� �б� ����

	//exit(0);	// exit�� ���ؼ� ���α׷��� ������������ ����Ǵ� ���� ����
}
