/*
  제목: SIC/XE Assembler, Example of a SIC/XE program
  작성자: 김영대 (http://www.howto.pe.kr)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define MAXIDLEN 20 // 명칭(identifier의 표준 길이)

struct TAB { // Symbol, Literal Table 구조체
  char name[MAXIDLEN];
  int  value;
  int  sec; // 섹션번호
};

struct OPT { // Operation Table 구조체
  char name[MAXIDLEN];
  int  op;
  int  oplength;
  char ext; // 확장 명령어(SIC/XE) 이면 1
};

char REGTAB[]= {'A','X','L','B','S','T','F'};
char HEXTAB[]= {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

// SIC/XE 명령 테이블(Operation Table)
struct OPT OPTAB[] =
{
  // 어셈블러 명령어
  {"ADD",0x18,3,0},   {"ADDF",0x58,3,1},  {"ADDR",0x90,2,1},  {"AND",0x40,3,0},
  {"CLEAR",0xB4,2,1}, {"COMP",0x28,3,0},  {"COMPF",0x88,3,1}, {"COMPR",0xA0,2,1},
  {"DIV",0x24,3,0},   {"DIVF",0x64,3,1},  {"DIVR",0x9C,2,1},  {"FIX",0xC4,1,1},
  {"FLOAT",0xC0,1,1}, {"HIO",0xF4,1,1},   {"J",0x3C,3,0},     {"JEQ",0x30,3,0},
  {"JGT",0x34,3,0},   {"JLT",0x38,3,0},   {"JSUB",0x48,3,0},  {"LDA",0x00,3,0},
  {"LDB",0x68,3,1},   {"LDCH",0x50,3,0},  {"LDF",0x70,3,1},   {"LDL",0x08,3,0},
  {"LDS",0x6C,3,1},   {"LDT",0x74,3,1},   {"LDX",0x04,3,0},   {"LPS",0xD0,3,1},
  {"MUL",0x20,3,0},   {"MULF",0x60,3,1},  {"MULR",0x98,2,1},  {"NORM",0xC8,1,1},
  {"OR",0x44,3,0},    {"RD",0xD8,3,0},    {"RMO",0xAC,2,1},   {"RSUB",0x4C,3,0},
  {"SHIFTL",0xA4,2,1},{"SHIFTR",0xA8,2,1},{"SIO",0xF0,1,1},   {"SSK",0xEC,3,1},
  {"STA",0x0C,3,0},   {"STB",0x78,3,1},   {"STCH",0x54,3,0},  {"STF",0x80,3,1}, 
  {"STI",0xD4,3,1},   {"STL",0x14,3,0},   {"STS",0x7C,3,1},   {"STSW",0xE8,3,0},
  {"STT",0x84,3,1},   {"STX",0x10,3,0},   {"SUB",0x1C,3,0},   {"SUBF",0x5C,3,1},
  {"SUBR",0x94,2,1},  {"SVC",0xB0,2,1},   {"TD",0xE0,3,0},    {"TIO",0xF8,1,1}, 
  {"TIX",0x2C,3,0},   {"TIXR",0xB8,2,1},  {"WD",0xDC,3,0 },
  // 어셈블러 지시자
  {"BASE",0,0,0},  {"NOBASE",0,0,0}, {"BYTE",1,0,0}, {"END",2,0,0},   {"EQU",7,0,0},
  {"LTORG",8,0,0}, {"RESB",3,0,0},   {"RESW",4,0,0}, {"START",5,0,0}, {"WORD",6,0,0},
  {"USE",9,0,0},   {"CSECT",10,0,0}, {"EXTREF",11,0,0}, {"EXTDEF",12,0,0}
};

// 소스파일 핸들러
FILE *fSrc;
// 목적 코드가 첨가된 소스파일 핸들러
FILE *fLst;
// 목적 코드 파일 핸들러
FILE *fObj;
// 소스 한행의 label, operation code, operand 1/2
char label[MAXIDLEN], opcode[MAXIDLEN], op1[MAXIDLEN], op2[MAXIDLEN];
// 소스 한행을 읽어 저장
char buf[255];
// buf 문자열에서 하나의 토큰(token) 문자열을 tmp에 복사
char tmp[MAXIDLEN];
// 위치 계수기
int LOCCTR;
// 프로그램 시작 주소
int STARTADDR;
// 베이스 주소
int BASEADDR;
// Symbol Table의 기호 갯수
int SYMTABp;
// Literal Table의 기호 갯수
int LTTABp;
// Literal Table의 최근 읽은 위치
int LThandlep;
// Literal Table(pool) 번호
int LTctr, LTp;
// 소스 한행의 문자수
int length;
// 소스 한행의 처리 위치
int index1, index2, index3;
// 행수
int LineNum;
// 현재 제어섹션(control section) 번호
int secctr;
// object code
int obj[MAXIDLEN];
// 현재 처리중인 pass 번호
int PASS;
// 제어 섹션의 프로그램(섹션) 길이 계산
int Plength[20];
// 현재 처리되는 명령어의 주소
int L2;
// 16진수로 나타낸 텍스트 레코드(Text record)의 목적 코드
char TR[70];
// 바이트로 나타낸 텍스트 레코드의 길이
int TRctr;
// 텍스트 레코드에 포함될 목적 코드 시작주소
int TS;
// Operation Table의 opcode
int opc;
// 외부 참조의 기호 갯수
int EXTTABp;
// 수정 레코드(Modification record)의 갯수
int MTABp;
// 수정 레코드(Modification record)의 최근 읽은 위치
int MThandlep;
// 최초로 실행할 명령어의 주소(first execution instruction)
int FEI;
// 기호(symbol), 리터럴(literal), 외부참조 테이블
struct TAB SYMTAB[200], LTTAB[100], EXTTAB[100];
// 제어섹션명(control section name)
char SN[20][MAXIDLEN];
// 수정 레코드(Modification record) 테이블
char MTAB[30][MAXIDLEN];
// 함수의 prototype
int writeLST(int format);

/*
  Operation table인 TBL[]에서 obj[]를 찾는다
*/
int searchop(char obj[])
{
  int i;
  for(i = 0; i < (sizeof(OPTAB) / sizeof(struct OPT)); i++) { // opcode 갯수만큼
    if (obj[0] == '+') { // 확장 형식 명령어(4형식)
      if (strcmp(OPTAB[i].name, obj+1) == 0) {
        opc = OPTAB[i].op; // 연산코드(operation code)를 저장해둠
        return (OPTAB[i].oplength+1); // +1 은 4byte 길이를 나타냄
      }
    }
    if (strcmp(OPTAB[i].name, obj) == 0) {
      opc = OPTAB[i].op;
      return (OPTAB[i].oplength);
    }
  }
  return (-1);
}

/*
  1. Symbol Table 인 TBL[]에서 sc 섹션의 obj[]를 찾는다
  2. Literal Table(pool) 인 TBL[]에서 sc 풀의 obj[]를 찾는다
*/
int search(struct TAB TBL[], char obj[], int sc)
{
  int i;
  for(i = 0; i < 100; i++) {
    if ((strcmp(TBL[i].name, obj) == 0) && (TBL[i].sec == sc))
      return (TBL[i].value);
  }
  return (-1);
}

/*
  buf 문자열의 index1에서 시작하여 하나의 토큰(token) 문자열을 tmp에 복사한다
*/
int gettoken()
{
  index2 = 0;
  for(;(buf[index1] == ' ') || (buf[index1] == '\t'); index1++); //공백문자(white space) 제거
  if ((buf[index1] == ',') || (buf[index1] == '-') || (buf[index1] == '+')){
    tmp[index2] = buf[index1];
    index2++;
    index1++;
  }
  while ((buf[index1] != '\n') && (buf[index1] != '\r') && // 행의 끝
         (buf[index1] != ' ') && (buf[index1] != '\t') && (buf[index1] != ',') &&
         (buf[index1] != '+') && (buf[index1] != '-') && (index1 < length)) {
    tmp[index2] = buf[index1];
    index1++;
    index2++;
  }
  tmp[index2] = '\0';
}

/*
  소스의 한 행을 읽어 label, opcode, op1, op2 를 구한다
  return code:
    0: 정상
    1: syntax error
    2: eof
*/
int readoneline()
{
  int n;

  if (fgets(buf, 255, fSrc) < 0)
    return (2); // eof

  while (buf[0] == '.' || buf[0] == '\n') { // 주석(comment)과 공백라인(empty line)을 제거
    if (PASS == 2) {
      LineNum += 5;
      fprintf(fLst, "%4d          %s", LineNum, buf); // 주석과 공백은 Lst 파일에 Loc없이 그냥 출력
    }
    if (fgets(buf, 255, fSrc) < 0)
      return (2); // eof
  }
  index1 = 0;
  length = strlen(buf);

  gettoken();
  if (searchop(tmp) >= 0) { // 첫번째 토큰이 Operation table에 있다
    label[0] = '\0'; // 이때는 label이 없다
    strcpy(opcode, tmp); // 원래는 label의 자리이지만 Operation table에 있으므로 opcode로 간주됨
  } else {
    strcpy(label, tmp); // 첫번째 토큰을 label로 간주한다
    gettoken();
    strcpy(opcode, tmp); // opcode(operation code)
    if (searchop(tmp) == -1) { // Operation table에 없다
      gettoken();
      strcpy(op1, tmp);
      gettoken();
      strcpy(op2, tmp);
      index3 = index1;
      if (PASS == 1)
  		  printf("illegal operation code: %s\n", buf);
      return (1); // syntax error 처리
    }
  }

  n = index1;
  gettoken();
  // op1이 있고 op1이 opcode와 거리가 20 이상이면 op1을 주석으로 간주
  if ((tmp[0] != '\0') && ((index1 - n) > 20)) {
    op1[0] = '\0';
    op2[0] = '\0';
  } else {
    strcpy(op1, tmp); // 첫번째 operand
    gettoken();
    strcpy(op2, tmp); // 두번째 operand
  }
  index3 = index1; // index3은 op2 다음위치를 가리킴
  // printf("label->%s opcode->%s op1->%s op2->%s\n", label, opcode, op1, op2);
  return (0); // 정상처리
}

/*
  소스에서 첫행의 START문장을 찾아 시작주소와 기본 섹션명을 구한다
  return code:
    0: 정상
    1: syntax error 또는 eof
*/
int findstart()
{
  LOCCTR = 0; // 위치계수기(location counter)
  secctr = 0; // 현재 제어섹션(control section) 번호

  if (readoneline() == 0) {
    if (strcmp(opcode, "START") == 0) {
      sscanf(op1, "%d", &STARTADDR); // 프로그램 시작 주소
      strcpy(SN[secctr], label); // 기본 제어섹션명(START문에 나타난 label명)

		LOCCTR = STARTADDR; // 위치 계수기(LOCCTR)를 START문에 나타난 시작 주소로 초기화한다

		if (PASS == 2) {
        fprintf(fLst, " 행     Loc                     원시문                     목적코드\n");
        fprintf(fLst, "----    ----  -------------------------------------------- --------\n");
        writeLST(0); // Lst 파일의 출력 시작
        // Obj 파일의 출력 시작(Header)
        fprintf(fObj, "H%-6s%06X%06X\n", label, STARTADDR, Plength[secctr]);
      }
      // printf("section name->%s value->%d section->%d\n", SN[secctr], STARTADDR, secctr);

      if (readoneline() != 0) { // START문 다음행을 읽어 둔다
        return(1);
      }
    } else // START문에 시작주소가 없다
      printf("The start location is set to 0 as default.\n");
  } else {
    return(1);
  }
  return(0);
}

/*
  pass 2에서 사용하기 위해 모든 label에 배정된 주소 값들을 저장하여
  기호 테이블(Symbol Table)을 구성하고 EQU 어셈블러 지시자를 처리한다
*/
int handlelabel1()
{
  int value;
  if (label[0] != '\0') {
    // EQU 어셈블러 지시자는 기호를 정의하고(즉, SYMTAB에 기입) 그 기호에 지정된
    // 값을 배정한다. 기호의 값은 상수 또는 상수와 이전에 정의된 기호로 구성되는
    // 수식이 될 수 있다
    if (strcmp(opcode, "EQU") == 0) { // EQU 어셈블러 지시자 처리
      if (op1[0] != '\0') {
        strcpy(SYMTAB[SYMTABp].name, label);
        if (op1[0] == '*') { // location counter값을 기호의 값으로 사용
          SYMTAB[SYMTABp].value = LOCCTR;
        } else {
          // 기호의 값이 기호 테이블에 있다면 기호값을 가짐
          SYMTAB[SYMTABp].value = search(SYMTAB, op1, secctr);
          if (SYMTAB[SYMTABp].value == -1) // 기호 테이블에 없다면 기호값을 상수로 보고 처리
            SYMTAB[SYMTABp].value = atoi(op1);
        }
        SYMTAB[SYMTABp].sec = secctr;// 현재의 섹션 번호(기호가 들어있는 섹션 번호)

        // 아래 문장은 복수개의 기호(MAXLEN EQU BUFEND-BUFFER-10 같은 문장)을 처리하기 위해...
        if ((op2[0] == '-') || (op2[0] == '+')) { // 수식의 연산은 +, - 만 지원됨
          strcpy(tmp, op2); // 다음 토큰을 읽기전에 이미 읽었던 op2를 먼저 처리
          while ((tmp[0] == '-') || (tmp[0] == '+')) {
            if (tmp[0] == '-') {
              value = search(SYMTAB, tmp+1, secctr);
              if (value == -1) // 기호 테이블에 없다면 기호값을 상수로 보고 처리
                SYMTAB[SYMTABp].value -= atoi(tmp+1);
              else
                SYMTAB[SYMTABp].value -= value;
            }
            if (tmp[0] == '+') {
              value = search(SYMTAB, tmp+1, secctr);
              if (value == -1)
                SYMTAB[SYMTABp].value += atoi(tmp+1);
              else
                SYMTAB[SYMTABp].value += value;
            }
            gettoken(); // 다음 토큰을 tmp에 읽는다
          } // while
        }

        SYMTABp++;
        // printf("symbol name->%s value->%d section->%d\n",
        //        SYMTAB[SYMTABp-1].name, SYMTAB[SYMTABp-1].value, SYMTAB[SYMTABp-1].sec);
      }
    } else {
      if (search(SYMTAB, label, secctr) >= 0) {
        printf("duplicate symbol\n");
        printf("%s \n", buf);
      } else {
        strcpy(SYMTAB[SYMTABp].name, label);
        SYMTAB[SYMTABp].value = LOCCTR; // location counter값을 기호의 값으로 사용
        SYMTAB[SYMTABp].sec = secctr; // 현재의 섹션 번호(기호가 들어있는 섹션 번호)
        SYMTABp++; // Symbol Table의 기호 갯수
        // printf("symbol name->%s value->%d section->%d\n",
        //        SYMTAB[SYMTABp-1].name, SYMTAB[SYMTABp-1].value, SYMTAB[SYMTABp-1].sec);
      }
    }
  }
}

/*
  Literal Table(pool) 에 리터럴 등록(번지계산은 computeLT()에서 함)
*/
int handleLT1()
{
  if (op1[0] == '=') {
    if (search(LTTAB, op1, LTctr) == -1) { // Literal Table(pool) 에서 중복 검사
      strcpy(LTTAB[LTTABp].name, op1);
      // 현재의 Literal Table 번호(LTORG 문에 의해 서로 다른 테이블이 만들어짐)
      LTTAB[LTTABp].sec = LTctr;
      LTTABp++; // Literal Table의 리터럴 갯수
      // printf("literal name->%s pool->%d\n", LTTAB[LTTABp-1].name, LTTAB[LTTABp-1].sec);
    }
  }
}

/*
  16진수 문자 H의 10진수를 리턴한다
*/
int hexstr2dec(char H)
{
  int i;
  for (i = 0; i <= 15; i++)
    if (HEXTAB[i] == H)
      return (i);
  return (-1);
}

/*
  프로그램(제어섹션)의 끝이거나 LTORG 어셈블러 지시자를 만낮을때 호출되며
  Literal Table(pool) 의 리터럴에 번지계산 함(LTORG 밑에 생성되야 하므로)
*/
int computeLT()
{
  int n;
  for (; LThandlep < LTTABp; LThandlep++) {
    if (LTTAB[LThandlep].name[1] == 'C') { // 지정한 문자로 초기화되는 바이트 수
      n = 0;
      LTTAB[LThandlep].value = LOCCTR;
      while (LTTAB[LThandlep].name[n+3] != '\'')
        n++;
      LOCCTR += n;
      // printf("literal name->%s value->%d pool->%d size->%d\n",
      //        LTTAB[LThandlep].name, LTTAB[LThandlep].value, LTTAB[LThandlep].sec, n);
    } else if (LTTAB[LThandlep].name[1] == 'X') { // 지정한 16진수로 초기화되는 바이트 수
      n = 0;
      LTTAB[LThandlep].value = LOCCTR;
      while (LTTAB[LThandlep].name[n+3] != '\'')
        n++;
      LOCCTR += n / 2; // byte 수 로 환산하므로 2로 나눔
      // printf("literal name->%s value->%d pool->%d size->%d\n",
      //        LTTAB[LThandlep].name, LTTAB[LThandlep].value, LTTAB[LThandlep].sec, n/2);
    } else {
      LTTAB[LThandlep].value = LOCCTR;
      LOCCTR++;
    }
  }
}

/*
  1. 프로그램내의 모든 문에 주소를 배정한다
  2. 어셈블러 지시자들에 관련된 처리를 부분적으로 행한다(BYTE, RESW 등에 의하여 정의되는
     데이타 영역의 길이 결정과 같은 주소 배정에 영향을 주는 처리를 포함한다
*/
int handleopcode1()
{
  int n = 0;
  int v = searchop(opcode); // 명령어 길이를 구함(지시자의 길이는 0)

  if (v > 0)
    LOCCTR = LOCCTR + v;
  else if (v == 0) { // 어셈블러 지시자
    if (strcmp(opcode, "WORD") == 0) {
      LOCCTR = LOCCTR + 3;
      // printf("%s %s\n", opcode, op1);
    } else if (strcmp(opcode, "RESW") == 0) {
      sscanf(op1, "%d", &n); // n은 예약되는 워드(3 byte)의 갯수
      LOCCTR = LOCCTR + (n * 3);
      // printf("%s %s\n", opcode, op1);
    } else if (strcmp(opcode, "RESB") == 0) {
      sscanf(op1, "%d", &n); // n은 예약되는 byte의 갯수
      LOCCTR = LOCCTR + n;
      // printf("%s %s\n", opcode, op1);
    } else if (strcmp(opcode, "BYTE") == 0) {
      if (op1[0] == 'C') { // 지정한 문자로 초기화되는 바이트 수
        n = 0;
        while (op1[n+2] != '\'') {
          LOCCTR++;
          n++;
        }
        // printf("%s %s -> %d\n", opcode, op1, n);
      } else if (op1[0] == 'X') { // 지정한 16진수로 초기화되는 바이트 수
        n = 0;
        while (op1[n+2] != '\'')
          n++;
        LOCCTR += n / 2; // byte 수 로 환산하므로 2로 나눔
        // printf("%s %s -> %d\n", opcode, op1, n/2);
      } else
        LOCCTR++;
    } else if (strcmp(opcode, "LTORG") == 0) {
      computeLT(); // Literal Table(pool) 의 리터럴에 번지계산 함
      LTctr++; // 새로운 Literal Pool 이므로 Literal Table(pool) 번호를 증가한다
    } else if (strcmp(opcode, "CSECT") == 0) { // 새로운 제어 섹션의 발견
      Plength[secctr] = LOCCTR-STARTADDR; // 이전 제어 섹션의 프로그램(섹션) 길이 계산
      
      secctr++; // 제어섹션(control section) 번호 증가
      LOCCTR = 0; // 위치계수기(location counter) 초기화
      STARTADDR = 0; // 시작 주소 초기화

      strcpy(SN[secctr], label); // 제어섹션명(CSECT문에 나타난 label명)
      // 제어섹션명은 이미 기호 테이블에 들어가 있으므로(처리 순서상 그렇게 됨) 초기화
      SYMTAB[SYMTABp-1].value = 0;
      SYMTAB[SYMTABp-1].sec = secctr;
      // printf("section name->%s value->%d section->%d\n",
      //        SN[secctr], SYMTAB[SYMTABp-1].value, SYMTAB[SYMTABp-1].sec);
    } else if (strcmp(opcode, "EXTREF") == 0) { // 외부 참조(external reference)
      if (op1[0] != '\0') {
        strcpy(EXTTAB[EXTTABp].name, op1);
        EXTTAB[EXTTABp].value = 0; // 주소값은 아직 결정되지 않음
        EXTTAB[EXTTABp].sec = secctr; // 현재의 섹션 번호
        EXTTABp++; // 외부 참조의 기호 갯수
      }
      // printf("external reference->%s value->%d section->%d\n",
      //        EXTTAB[EXTTABp-1].name, EXTTAB[EXTTABp-1].value, EXTTAB[EXTTABp-1].sec);

      strcpy(tmp, op2); // 다음 토큰을 읽기전에 이미 읽었던 op2를 먼저 처리
      while (tmp[0] == ',') {
        strcpy(EXTTAB[EXTTABp].name, tmp+1);
        EXTTAB[EXTTABp].value = 0;
        EXTTAB[EXTTABp].sec = secctr;
        EXTTABp++;
        // printf("external reference->%s value->%d section->%d\n",
        //        EXTTAB[EXTTABp-1].name, EXTTAB[EXTTABp-1].value, EXTTAB[EXTTABp-1].sec);
        gettoken(); // 다음 토큰을 tmp에 읽는다
      }
    }
  }
}

/*
  레지스터 문자 REG의 번호를 리턴한다
*/
char getREGnum(char REG)
{
  int i;
  for (i = 0; i <= 6; i++)
    if (REGTAB[i] == REG)
      return (i);
  printf("Error! REGISTER NAME expected: %s\n", buf);
  return (-1);
}

/*
  연산코드(operation code)의 object 코드 생성
*/
int makeobjcode(int format)
{
  int dxx;
  int disp;
  int loc;
  int loc2;

  // opc는 연산코드(operation code)로 searchop()내에서 미리 저장해둠
  switch(format) {
    case 1: // 1형식 명령어(FIX, FLOAT, HIO, NORM, SIO, TIO)
      obj[0] = opc;
      obj[1] = '\0';
      break;

    case 2: // 2형식 명령어(operand 형식은 r1 또는 r1,r2 또는 n 또는 r1,n)
      obj[0] = opc;
      if (op1[0] == '\0')
        printf("Error! Operand expected: %s\n", buf);

      if (op1[0] < 'A') { // op1이 레지스터가 아닌 상수값(n)
        sscanf(op1, "%d", &dxx);
        obj[1] = dxx << 4; // n
      } else {
        obj[1] = getREGnum(op1[0]) << 4; // r1
      }
      if (op2[0] == ',') {
        if (op2[1] < 'A') { // op2가 레지스터가 아닌 상수값(n)
          sscanf(op2+1, "%d", &dxx);
          obj[1] = obj[1] | dxx; // n
        } else {
          obj[1] = obj[1] | getREGnum(op2[1]); // r2
        }
      }
      break;

    case 3: // 3형식 명령어
      switch (op1[0]) {
        case '#': // 즉시 주소 지정(Immediate addressing)
          obj[0] = opc + 1; // n=0,i=1 이므로 +1 함
          if (op1[1] >= 'A') { // PC 상대주소 + 즉시 주소 지정인 경우(예, #LENGTH)
            dxx = search(SYMTAB, op1+1, secctr);
            if (dxx == -1)
              printf("Error: Undefind symbol: %s\n", op1+1);
            else
              dxx -= LOCCTR; // op1과 다음 명령어를 가리키고 있는 LOCCTR 과의 차이
            obj[1] = (dxx >> 8) & 15; // TA 상위 1 니블
            obj[1] = obj[1] | 32; // p=1 (PC 상대주소)
            obj[2] = dxx & 255; // TA 하위 2 니블
          } else { // 상수값
             sscanf(op1+1, "%d", &dxx);
             obj[1] = (dxx >> 8) & 15; // TA 상위 1 니블
             obj[2] = dxx & 255; // TA 하위 2 니블
          }
          break;
        case '\0': // operand가 없다(RSUB)
           obj[0] = opc + 3; // n=1,i=1 이므로 +3 함
           obj[1] = 0;
           obj[2] = 0;
           break;
        default:
          if (op1[0] == '@') { // 간접 주소 지정(Indirect addressing)
            obj[0] = opc + 2; // n=1,i=0 이므로 +2 함
            loc = search(SYMTAB, op1+1, secctr);
            disp =loc - LOCCTR; // op1과 다음 명령어를 가리키고 있는 LOCCTR 과의 차이
          }
          else if (op1[0] == '=') { // 리터럴
            obj[0] = opc + 3; // n=1,i=1 이므로 +3 함
            loc = search(LTTAB, op1, LTctr); // Literal Table(pool) 에서 검색
            disp = loc - LOCCTR; // op1과 다음 명령어를 가리키고 있는 LOCCTR 과의 차이
          }
          else {
            obj[0] = opc + 3; // n=1,i=1 이므로 +3 함
            loc = search(SYMTAB, op1, secctr);
            disp = loc - LOCCTR; // op1과 다음 명령어를 가리키고 있는 LOCCTR 과의 차이
          }

          if (loc < 0) {
            printf("Error: Undefinded symbol: %s\n", op1);
            loc = 0;
          }

          if ((abs(disp) >= 4096) && (loc >= 0)) { // base 상대주소를 사용해야하는 경우
            disp = abs(BASEADDR - loc);
            obj[1] = (disp >> 8) & 15; // TA 상위 1 니블
            obj[1] = obj[1] | 64; // b=1 (base 상대주소)
            obj[2] = disp & 255; // TA 하위 2 니블
            // printf("opcode->%s base->%X loc->%X disp->%d\n", opcode, BASEADDR, loc, disp);
          }
			 else if ((disp < 4096) && (loc >= 0)) {
            obj[1] = (disp >> 8) & 15; // TA 상위 1 니블
            obj[1] = obj[1] | 32; // p=1 (PC 상대주소)
            obj[2] = disp & 255; // TA 하위 2 니블
          }
          else {
            int ddd;
            if (op1[0] == '@') // Indirect addressing
              sscanf(op1+1, "%d", &ddd);
            else
              sscanf(op1, "%d", &ddd);
            obj[1] = (ddd >> 8);
            obj[2] = ddd & 255;
          }

          if ((op2[1] == 'X') && (op2[0] == ',')) // 인덱스 주소 지정
            obj[1] = obj[1] + 128; // x=1
      }
      // printf("<3형식> opcode->%s obj->%02X%02X%02X\n", opcode, obj[0], obj[1], obj[2]);
      break;

    case 4: // 4형식
      switch (op1[0]) {
        case '#': // 즉시 주소 지정(Immediate addressing)
          obj[0] = opc + 1; // n=0,i=1 이므로 +1 함
          if (op1[1] >= 'A') { // PC 상대주소 + 즉시 주소 지정인 경우(예, #LENGTH)
            dxx = search(SYMTAB, op1+1, secctr);
            if (dxx== -1)
              printf("Error: Undefind symbol: %s\n", op1+1);
            else
              dxx -= LOCCTR; // op1과 다음 명령어를 가리키고 있는 LOCCTR 과의 차이
          } else  // 상수값
            sscanf(op1+1, "%d", &dxx);
          obj[1] = 16; // e=1
          obj[2] = (dxx >> 8) & 255; // TA 상위 1 바이트
          obj[3] = dxx & 255; // TA 하위 1 바이트
          break;
        default:
          if (op1[0] == '@') { // 간접 주소 지정(Indirect addressing)
            obj[0] = opc + 2; // n=1,i=0 이므로 +2 함
            loc2 = search(SYMTAB, op1+1, secctr);
          }
          else {
            obj[0] = opc + 3; // n=1,i=1 이므로 +3 함
            loc2 = search(SYMTAB, op1, secctr);
            if (loc2 < 0) { // 현재 섹션에서 찾았는데 없다
              loc2 = search(EXTTAB, op1, secctr); // 외부 참조인지 검사
              if (loc2 >= 0) {
                // 수정 레코드(Modification record)에 추가
                // L2-STARTADDR+1 -> 현 제어섹션의 처음부터 상대적으로 표기된 수정될 필드의 주소
                // 05 -> 하프 바이트로 나타낸 수정될 레코드의 길이(여기서는 05로 fix시킴)
                // + -> 수정 플래그(여기서는 +로 fix시킴)
                sprintf(MTAB[MTABp], "M%06X05+%-6s\n", L2-STARTADDR+1, op1);
                MTABp++;
              }
            }
            if (loc2 < 0) {
               printf("Error: Undefinded symbol: %s\n", op1);
               loc2=0;
            }
          }
          if (loc2 >= 0) {
            obj[1] = 16; // e=1
            obj[2] = (loc2 >> 8); // TA 상위 1 바이트
            obj[3] = loc2 & 255;  // TA 하위 1 바이트
          }
          else {
            int ddd;
            if (op1[0] == '@') // Indirect addressing
              sscanf(op1+1, "%d", &ddd);
            else
              sscanf(op1, "%d", &ddd);
            obj[1] = 16; // e=1
            obj[2] = (ddd >> 8); // TA 상위 1 바이트
            obj[3] = ddd & 255;  // TA 하위 1 바이트
          }
          if (op2[1] == 'X') // 인덱스 주소 지정
            obj[1] = obj[1] + 128; // x=1
      }
      // printf("<4형식> opcode->%s obj->%02X%02X%02X%02X\n", opcode, obj[0], obj[1], obj[2], obj[3]);
      break;
  }
}

/*
  Literal Table(pool) 출력
*/
int writeLT()
{
  int n, len=0;

  for (; (LTp < LTTABp) && (LTTAB[LTp].sec == LTctr); LTp++) {
    L2 = LOCCTR;
    len = 0;
    label[0] = '*'; // *
    label[1] = '\0';
    strcpy(opcode, LTTAB[LTp].name);
    op1[0] = '\0';
    if (LTTAB[LTp].name[1] == 'C') { // 지정한 문자로 초기화되는 바이트 수
      n = 0;
      while (LTTAB[LTp].name[n+3] != '\'') {
        obj[len] = LTTAB[LTp].name[n+3]; // character를 byte로 환산
        n++;
        len++;
      }
      obj[len] = '\0';
      LOCCTR += len;
    }
    else if (LTTAB[LTp].name[1] == 'X') { // 지정한 16진수로 초기화되는 바이트 수
      n = 0;
      while (LTTAB[LTp].name[n+3] != '\'') {
        // 16진수 두 글자를 1 byte로 환산
        obj[len] = hexstr2dec(LTTAB[LTp].name[n+3])*16 + hexstr2dec(LTTAB[LTp].name[n+4]);
        n += 2;
        len++;
      }
      obj[len] = '\0';
      LOCCTR += len;
    }
    else {
      sscanf(op1+1, "%d", obj);
      len++;
      LOCCTR++;
    }

    if (opcode[0] != '=')
      writeLST(0);
    else
      writeLST(len);

  } // for
  return(len);
}

/*
  텍스트 레코드(Text record)의 출력
*/
int writeTEXT()
{
  if (TRctr > 0) {
    // 텍스트 레코드(Text record)
    // TS -> 텍스트 레코드에 포함될 목적 코드 시작주소
    // TRctr-> 바이트로 나타낸 텍스트 레코드의 길이(TR에 16진수 문자로 들어가 있으므로 반으로 나눈것임)
    // TR -> 16진수로 나타낸 목적 코드
    fprintf(fObj, "T%06X%02X%s\n", TS, TRctr/2, TR);
    TRctr = 0;
  }
}

/*
  Pass 2 리스트, 목적코드 출력
*/
int handlepass2()
{
  int n = 0;
  int format = searchop(opcode); // 명령어 길이를 구함(지시자의 길이는 0)
  // * LOCCTR은 현재 처리되는 명령어의 다음 명령어를 가리키게 되므로 L2를 현재 주소로 사용하시오!
  L2 = LOCCTR;
  if (format > 0) { // 어셈블러 지시자가 아님
    LOCCTR = LOCCTR + format; // 위치계수기(location counter)는 다음 명령어를 가리키게됨
    makeobjcode(format);
    writeLST(format);
  }
  else if (format == 0) { // 어셈블러 지시자
    if (strcmp(opcode, "BASE") == 0) { // 베이스 레지스터의 값을 어셈블러에게 알려준다
      writeLST(format);
      BASEADDR = search(SYMTAB, op1, secctr); // 기호 테이블에서 검색
      if (BASEADDR < 0)
        BASEADDR = 0;
    }
    else if (strcmp(opcode, "NOBASE") == 0) { // 베이스 레지스터의 값을 해제한다
      writeLST(format);
      BASEADDR = 0;
    }
    else if (strcmp(opcode, "WORD") == 0) {
      LOCCTR = LOCCTR + 3;
      if (op1[0] > 'A') { // 기호 사용
        int W3, W4=0;
        W3 = search(SYMTAB, op1, secctr); // 기호 테이블에서 검색
        if (W3 < 0) {
          W3 = search(EXTTAB, op1, secctr); // 외부 참조인지 검사
          if (W3 < 0) {
            printf("Error: Undefinded symbol: %s\n", op1);
            obj[0] = obj[1] = obj[2] = 0;
            return(3);
          }
          // 수정 레코드(Modification record)에 추가
          // L2-STARTADDR+1 -> 현 제어섹션의 처음부터 상대적으로 표기된 수정될 필드의 주소
          // 05 -> 하프 바이트로 나타낸 수정될 레코드의 길이(여기서는 05로 fix시킴)
          // + -> 수정 플래그(여기서는 +로 fix시킴)
          sprintf(MTAB[MTABp], "M%06X06+%-6s\n", L2-STARTADDR, op1);
          MTABp++;
        }

        // 아래 문장은 복수개의 기호(MAXLEN WORD BUFEND-BUFFER 같은 문장)을 처리하기 위해...
        if ((op2[0] == '-') || (op2[0] == '+')) { // 수식의 연산은 +, - 만 지원됨
          strcpy(tmp, op2); // 다음 토큰을 읽기전에 이미 읽었던 op2를 먼저 처리
          while ((tmp[0] == '-') || (tmp[0] == '+')) {
            W4 = search(SYMTAB, tmp+1, secctr);
            if (W4 < 0) {
              W4 = search(EXTTAB, tmp+1, secctr);
              if (W4 < 0) {
                printf("Error: Undefinded symbol: %s\n", tmp+1);
                obj[0] = obj[1] = obj[2] = 0;
                return(3);
              }
              sprintf(MTAB[MTABp], "M%06X06%c%-6s\n", L2-STARTADDR, tmp[0], tmp+1);
              MTABp++;
            }
            if (op2[0]=='+')
              W3 = W3 + W4;
            else
              W3 = W3 - W4;

            gettoken(); // 다음 토큰을 tmp에 읽는다
          } // while
        }

        obj[0] = (W3 >> 16) & 255;
        obj[1] = (W3 >> 8) & 255;
        obj[2] = W3 & 255;
      }
      else {
        sscanf(op1, "%d", &n);
        obj[0] = 0;
        obj[1] = (n >> 8) & 255;
        obj[2] = n & 255;
      }
      writeLST(3); // 3형식이 아니라 단지 인쇄를 위해
      return (3);
    }
    else if (strcmp(opcode, "RESW") == 0) {
      writeLST(format);
      sscanf(op1, "%d", &n); // n은 예약되는 워드(3 byte)의 갯수
      LOCCTR = LOCCTR + (n * 3);
      writeTEXT(); // 이전 까지의 텍스트 레코드(Text record)의 출력
      // TS -> 텍스트 레코드에 포함될 목적 코드 시작주소
      // LOCCTR를 대입하는것은 현재 RESW가 할당받는 공간을 넘어서 주소가 시작되어야 하므로...
      TS = LOCCTR;
    }
    else if (strcmp(opcode, "RESB") == 0) {
      writeLST(format);
      sscanf(op1, "%d", &n); // n은 예약되는 byte의 갯수
      LOCCTR = LOCCTR + n;
      writeTEXT(); // 이전 까지의 텍스트 레코드(Text record)의 출력
      // TS -> 텍스트 레코드에 포함될 목적 코드 시작주소
      // LOCCTR를 대입하는것은 현재 RESW가 할당받는 공간을 넘어서 주소가 시작되어야 하므로...
      TS = LOCCTR;
    }
    else if (strcmp(opcode, "BYTE")==0)
    {
      if (op1[0] == 'C') { // 지정한 문자로 초기화되는 바이트 수
        n = 0;
        while (op1[n+2] != '\'') {
          obj[n] = op1[n+2]; // character를 byte로 환산
          n++;
        }
        obj[n] = '\0';
        LOCCTR += n;
        writeLST(n);
        return(n);
      }
      else if (op1[0] == 'X') { // 지정한 16진수로 초기화되는 바이트 수
        int len=0;
        n = 0;
        while (op1[n+2] != '\'') {
          obj[len] = hexstr2dec(op1[n+2])*16 + hexstr2dec(op1[n+3]); // 16진수 두 글자를 1 byte로 환산
          n += 2;
          len++;
        }
        obj[len] = '\0';
        LOCCTR += len;
        writeLST(len);
        return(len);
      }
      else {
        sscanf(op1, "%d", &n);
        obj[0] = n;
        LOCCTR++;
        writeLST(n);
        return(1);
      }
    }
    else if (strcmp(opcode, "LTORG") == 0) {
      writeLST(format);
      n = writeLT(); // Literal Table(pool) 출력
      LTctr++; // 새로운 Literal Pool 이므로 Literal Table(pool) 번호를 증가한다
      return(n);
    }
    else if (strcmp(opcode, "CSECT")==0)
    {
      LOCCTR = 0; // 위치계수기(location counter) 초기화
      STARTADDR = 0; // 시작 주소 초기화
      L2 = 0;
      TS = 0; // TS -> 텍스트 레코드에 포함될 목적 코드 시작주소

      fprintf(fLst, "\n");
      writeLST(format);

      secctr++; // 제어섹션(control section) 번호 증가
      writeTEXT(); // 이전 까지의 텍스트 레코드(Text record)의 출력
      while (MThandlep < MTABp) { // 수정 레코드(Modification record)를 출력한다
        fprintf(fObj, "%s", MTAB[MThandlep]);
        MThandlep++;
      }
      if (secctr == 1) // 기본 섹션(default section)
         fprintf(fObj, "E%06X\n\n", FEI); // 엔드 레코드(End record)
      else
         fprintf(fObj, "E\n\n");
      fprintf(fObj, "H%-6s%06X%06X\n", label, 0, Plength[secctr]);
    }
    else if (strcmp(opcode, "EQU") == 0) {
      L2 = search(SYMTAB, label, secctr);
      LineNum += 5;
      fprintf(fLst, "%4d    %04X  %-8s %-10s %s", LineNum, L2, label, opcode, op1); // 맨 앞에 Loc 출력
      strcpy(tmp, op2); // 다음 토큰을 읽기전에 이미 읽었던 op2를 먼저 처리
      while ((tmp[0] == '-') || (tmp[0] == '+')) {
        fprintf(fLst, "%s", tmp);
        gettoken(); // 다음 토큰을 tmp에 읽는다
      }
      fprintf(fLst, "\n");
    }
    else if (strcmp(opcode, "EXTDEF") == 0) // 외부 정의(external definition)
    {
      fprintf(fObj, "D"); // 정의 레코드(define record)
      if (op1[0] != '\0') {
        int a1 = search(SYMTAB, op1, secctr);
        if (a1 < 0)
          printf("Error in EXTDEF: No such symbol: %s\n", op1);
        fprintf(fObj, "%-6s%06X", op1, a1); // 현 제어 섹션에서 정의된 외부기호이름,상대주소
        LineNum += 5;
        fprintf(fLst, "%4d          %-8s %-10s %s", LineNum, label, opcode, op1);
      }

      strcpy(tmp, op2); // 다음 토큰을 읽기전에 이미 읽었던 op2를 먼저 처리
      while (tmp[0] == ',') {
        int a2 = search(SYMTAB, tmp+1, secctr);
        if (a2 < 0)
          printf("Error in EXTDEF: No such symbol: %s\n", tmp+1);
        fprintf(fObj,"%-6s%06X", tmp+1, a2);
        fprintf(fLst, "%s", tmp);
        gettoken(); // 다음 토큰을 tmp에 읽는다
      }
      fprintf(fObj, "\n");
      fprintf(fLst, "\n");
    }
    else if (strcmp(opcode, "EXTREF") == 0) // 외부 참조(external reference)
    {
      fprintf(fObj, "R"); // 참조 레코드(Refer record)
      if (op1[0] != '\0') {
        fprintf(fObj, "%-6s", op1);
        LineNum +=  5;
        fprintf(fLst, "%4d          %-8s %-10s %s", LineNum, label, opcode, op1);
      }

      strcpy(tmp, op2); // 다음 토큰을 읽기전에 이미 읽었던 op2를 먼저 처리
      while (tmp[0] == ',') {
        fprintf(fObj, "%-6s", tmp+1);
        fprintf(fLst, "%s", tmp);
        gettoken(); // 다음 토큰을 tmp에 읽는다
      }
      fprintf(fObj, "\n");
      fprintf(fLst, "\n");
    }
  }
  return (format);
}

int writeLST(int format)
{
  LineNum += 5; // 행번호

  switch (format) {
    case 0: // 에셈블러 지시자
      if (label[0] == '\0')
        fprintf(fLst, "%4d          %-8s %-10s %s\n", LineNum, label, opcode, op1);
      else
        fprintf(fLst, "%4d    %04X  %-8s %-10s %s\n", LineNum, L2, label, opcode, op1);
      break;

    case 1:
      if ((TRctr + 2) > 60) {
        writeTEXT();
        TS = L2;
      }
      sprintf(TR+TRctr, "%02X", obj[0]);
      TRctr += 2;
      fprintf(fLst, "%4d    %04X  %-8s %-10s %-25s%02X\n", LineNum, L2, label, opcode, op1, obj[0]);
      break;

    case 2: // 2형식
      if ((TRctr + 4) > 60) {
        writeTEXT(); // 이전 까지의 텍스트 레코드(Text record)의 출력
        // TS -> 텍스트 레코드에 포함될 목적 코드 시작주소
        // L2를 대입하는것은 현재 명령어가 다음 텍스트 레코드의 시작주소이므로...
        TS = L2;
      }
      sprintf(TR+TRctr, "%02X%02X", obj[0], obj[1]);
      TRctr += 4;

      fprintf(fLst,"%4d    %04X  %-8s %-10s", LineNum, L2, label, opcode);
      strcpy(tmp, op2); // 다음 토큰을 읽기전에 이미 읽었던 op2를 먼저 처리
      while ((tmp[0] == ',') || (tmp[0] == '-') || (tmp[0] == '+')) {
        strcat(op1, tmp);
        gettoken(); // 다음 토큰을 tmp에 읽는다
      }
      fprintf(fLst," %-25s%02X%02X\n", op1, obj[0], obj[1]);
      break;

    case 3: // 3형식
      if ((TRctr + 6) > 60) {
        writeTEXT(); // 이전 까지의 텍스트 레코드(Text record)의 출력
        // TS -> 텍스트 레코드에 포함될 목적 코드 시작주소
        // L2를 대입하는것은 현재 명령어가 다음 텍스트 레코드의 시작주소이므로...
        TS = L2;
      }
      sprintf(TR+TRctr, "%02X%02X%02X", obj[0], obj[1], obj[2]);
      TRctr += 6;
      fprintf(fLst,"%4d    %04X  %-8s %-10s", LineNum, L2, label, opcode);
      strcpy(tmp, op2); // 다음 토큰을 읽기전에 이미 읽었던 op2를 먼저 처리
      while ((tmp[0] == ',') || (tmp[0] == '-') || (tmp[0] == '+')) {
        strcat(op1, tmp);
        gettoken(); // 다음 토큰을 tmp에 읽는다
      }
      fprintf(fLst," %-25s%02X%02X%02X\n", op1, obj[0], obj[1], obj[2]);
      break;

    case 4: // 4형식
      if ((TRctr + 8) > 60) {
        writeTEXT(); // 이전 까지의 텍스트 레코드(Text record)의 출력
        // TS -> 텍스트 레코드에 포함될 목적 코드 시작주소
        // L2를 대입하는것은 현재 명령어가 다음 텍스트 레코드의 시작주소이므로...
        TS = L2;
      }
      sprintf(TR+TRctr, "%02X%02X%02X%02X", obj[0], obj[1], obj[2], obj[3]);
      TRctr += 8;
      fprintf(fLst,"%4d    %04X  %-8s %-10s", LineNum, L2, label, opcode);
      strcpy(tmp, op2); // 다음 토큰을 읽기전에 이미 읽었던 op2를 먼저 처리
      while ((tmp[0] == ',') || (tmp[0] == '-') || (tmp[0] == '+')) {
        strcat(op1, tmp);
        gettoken(); // 다음 토큰을 tmp에 읽는다
      }
      fprintf(fLst," %-25s%02X%02X%02X%02X\n", op1, obj[0], obj[1], obj[2], obj[3]);
      break;
  }
}

/*
  filename의 확장자를 ext로 바꾸어서 ofilename으로 리턴한다
*/
char * makefilename(char filename[], char ofilename[], char ext[])
{
  int i;
  char *dot;
  ofilename[0] = '\0';
  if (strchr(filename, '.') == NULL) { // 확장자 없음
    strcpy(ofilename, filename);
    strcat(ofilename, ext);
  } else {
    for (i = 0; filename[i]; i++)
      if (filename[i] == '.')
        break;
      else
        ofilename[i] = filename[i];
      ofilename[i] = '\0';
    strcat(ofilename, ext);
  }
  return ofilename;
}

int pass1(char filename[])
{
  int i;
  char ofilename[30];
  FILE *fSbl;

  PASS      = 1;
  SYMTABp   = 0;
  LTTABp    = 0;
  LThandlep = 0;
  LTctr     = 0;
  EXTTABp   = 0;
  if (findstart() != 0)
    return (1);

  while ((strcmp(opcode, "END") != 0)) {
    /*
      Pass 1 (기호 정의)
        1. 프로그램내의 모든 문에 주소를 배정한다
        2. Pass 2 에서 사용하기 위해 모든 label에 배정된 주소 값들을 저장한다
        3. 어셈블러 지시자들에 관련된 처리를 부분적으로 행한다(BYTE, RESW 등에 의하여 정의되는
           데이타 영역의 길이 결정과 같은 주소 배정에 영향을 주는 처리를 포함한다
    */
    handlelabel1();
    handleopcode1();
    handleLT1();
    // printf("label->[%s] opcode->[%s] op1->[%s] op2->[%s]\n", label, opcode, op1, op2);
    if (readoneline() != 0) // syntax error 또는 eof
      break;
  }

  if (op1[0] != '\0')
    FEI = search(SYMTAB, op1, 0); // 최초로 실행할 명령어의 주소
  else
    FEI = 0;

  computeLT(); // Literal Table(pool) 의 리터럴에 번지계산 함
  Plength[secctr] = LOCCTR-STARTADDR; // 마지막 제어 섹션의 프로그램(섹션) 길이 계산

  // Pass 1의 결과인 기호 테이블(Symbol Table), 리터럴 테이블(Literal Table) 출력
  fSbl = fopen(makefilename(filename,ofilename,".sbl"), "w");
  fprintf(fSbl, "1. Symbol Table for %s\n\n", filename);
  fprintf(fSbl, "    Symbol Name           Loc          Section Name\n");
  fprintf(fSbl, "--------------------      ----      --------------------\n");
  for (i = 0; i < SYMTABp; i++)
    fprintf(fSbl, "%20s      %04X      %20s\n", SYMTAB[i].name, SYMTAB[i].value, SN[SYMTAB[i].sec]);
  if (LTTABp > 0) {
    fprintf(fSbl, "\n\n2. Literal Table for %s\n\n", filename);
    fprintf(fSbl, "       Literal            Loc        Pool Number\n");
    fprintf(fSbl, "--------------------      ----      -------------\n");
    for (i = 0; i < LTTABp; i++)
      fprintf(fSbl, "%20s      %04X          %3d\n", LTTAB[i].name, LTTAB[i].value, LTTAB[i].sec);
  }
  fclose(fSbl);
  return (0);
}

int pass2(char filename[])
{
  int i;
  char ofilename[30];

  PASS      = 2;
  LTctr     = 0;

  // 목적 코드가 첨가된 소스파일 핸들러 fLst
  fLst = fopen(makefilename(filename,ofilename,".lst"), "w"); // Code listing with object code

  // 목적 코드 파일 핸들러 fObj
  fObj = fopen(makefilename(filename,ofilename,".obj"), "w"); // Object code

  if (findstart() != 0)
    return (1);

  while (1) {
    // 리스트, 목적코드 출력
	 handlepass2();
    if (readoneline() != 0) // syntax error 또는 eof
      break;

    if (strcmp(opcode, "END") == 0) {
      writeLST(0);
      writeTEXT(); // 이전 까지의 텍스트 레코드(Text record)의 출력

      TS = LOCCTR; // TS -> 텍스트 레코드에 포함될 목적 코드 시작주소
      i = writeLT(); // Literal Table(pool) 출력
      writeTEXT(); // Literal Table(pool) 에 대한 텍스트 레코드(Text record)의 출력

      while (MThandlep < MTABp) { // 수정 레코드(Modification record)를 출력한다
        fprintf(fObj, "%s", MTAB[MThandlep]);
        MThandlep++;
      }
      if (secctr == 0) // 기본 섹션(default section)
        fprintf(fObj, "E%06X\n\n", FEI); // 엔드 레코드(End record)
      else
        fprintf(fObj, "E\n\n");
      break;
    }
  }

  fclose(fLst);
  fclose(fObj);
  return (0);
}

void main(int argc, char *argv[])
{
  char filename[30] = "input_visible.txt";

  if (argc > 1)
    strcpy(filename, argv[1]); // 어셈블할 소스파일명
  else {
    printf("SIC/XE assembler: No input files\n");
    exit(1);
  }

  // 소스파일 핸들러 fSrc
  fSrc = fopen(filename, "r");
  while (fSrc == NULL) {
    printf("Can't find File: %s\n", filename);
    exit(1);
  }

  // Pass 1 실행
  if (pass1(filename) != 0)
    exit(1);

  rewind(fSrc); // 소스파일의 포인터를 맨 앞으로 보낸다

  // Pass 2 실행
  if (pass2(filename) != 0)
    exit(1);

  fclose(fSrc);
}
