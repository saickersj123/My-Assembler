#include "C:\Users\saick\Documents\SICXE Assembler 20194318\SICXE C EXAMPLES\SIC-XE_Assembler-master\20161623.h"

/*------------------------------------------------------------------------------------*/
/*함수 : get_hash_code()*/
/*목적 : 파라미터로 받은 문자열의 hash code를 계산한다*/
/*리턴값 : hash code */
/*------------------------------------------------------------------------------------*/
int get_hash_code(char* key) {
	int hash;
	int len = strlen(key);
	if (len < 1)
		return 0;
	hash = 5 * (int)key[0] + (int)key[len - 1];
	hash = hash % HASH_TABLE_SIZE;
	return hash;
}

/*------------------------------------------------------------------------------------*/
/*함수 : add_hash_table()*/
/*목적 : hash table에 opcode struct 하나를 추가한다*/
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void add_hash_table(int opcode, char* key, char* format) {
	int hash = get_hash_code(key);
	hash_table_node* new = (hash_table_node*)malloc(sizeof(hash_table_node));
	strcpy(new->format, format);
	strcpy(new->key, key);
	new->opcode = opcode;
	new->next = hash_table[hash];
	hash_table[hash] = new;
}

/*------------------------------------------------------------------------------------*/
/*함수 : make_hash_table()*/
/*목적 : opcode.txt를 읽고, hash_table을 만든다.*/
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
int make_hash_table() {
	FILE* fp;
	int tmp_opcode;
	char tmp_key[MAX_KEY_LENGTH];
	char tmp_format[MAX_FORMAT_LENGTH];

	fp = fopen("opcode.txt","r");												// opcode.txt open 
	if (fp == NULL) {
		printf("Error: failed to open 'opcode.txt'\n");
		return 0;
	}
	while (0 < fscanf(fp, "%X %s %s", &tmp_opcode, tmp_key, tmp_format)) {		// opcode.txt에서 opcode 정보를 저장하고 hash_table에 저장.
		add_hash_table(tmp_opcode, tmp_key, tmp_format);
	}
	return 1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : init_input_variables()*/
/*목적 : 매 명령어 사이클마다 실행되어, 새로운 명령어를 저장할 input과 input_args를 초기화한다.*/
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void init_input_variables() {
	int i;
	input = (char*)malloc(sizeof(char) * MAX_INPUT_SIZE);
	for (i = 0; i < 4; i++) {
		memset(input_args[i], '\0', 17);
	}
	memset(input, '\0', MAX_INPUT_SIZE);
	tmp_symtab = NULL;
}


/*------------------------------------------------------------------------------------*/
/*함수 : scan_til_line_end()*/
/*목적 : 개행문자를 입력받을때까지 문자를 입력받는다.
		get_input()에서 개행문자를 입력받기 전에 이미 invalid input으로 판별한 경우 호출한다.*/
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void scan_til_line_end() {
	char c;
	while (1) {
		c = getchar();
		if (c == '\n')
			break;
	}
}

/*------------------------------------------------------------------------------------*/
/*함수 : get_input()*/
/*목적 : 명령어 한 줄을 입력 받고 공백을 정제하여 input과 input_args에 저장한다*/
/*리턴값 : 입력에 실패한 경우(invalid한 input을 받은 경우) 0, 성공한 경우에는 1*/
/*------------------------------------------------------------------------------------*/
int get_input() {
	int i;
	int arg_num = 0;
	int char_num = 0;
	printf("sicsim> ");

	for (i = 0; i < MAX_INPUT_SIZE; i++) {		// 명령어를 입력받는 반복문. 첫 번째 공백까지 입력받고 input 과 input_args에 저장한다.
		input[i] = getchar();
		if (input[i] == ' ' || input[i] == '\t') {
			if (i == 0) {						// 입력 앞에 들어오는 공백은 무시한다
				i--;
				continue;
			}
			else {								// 공백이 들어오는 경우 input을 저장하고 반복문을 종료한다.
				strcpy(input_args[0], input);
				input_args[0][i] = '\0';
				break;
			}
		}
		if (input[i] == '\n') {								// 공백 없이 입력이 끝난 경우 input을 저장하고 함수를 종료한다.
			input[i] = '\0';
			input_args_len = 0;
			strcpy(input_args[0], input);
			return 1;
		}
	}
	if (strncmp(input, "loader", 6) == 0){				// loader명령어의 경우 입력 형식이 다르기 때문에 예외로 처리
		arg_num = 1;
		for(i = i + 1; i < MAX_INPUT_SIZE; i++){
			input[i] = getchar();
			if (input[i] == ' ' || input[i] == 't'){
				if (strlen(input_args[arg_num]) != 0){
					input_args[arg_num][char_num]='\0';
					arg_num++;
					char_num = 0;
					continue;
				}
				else{
					i--;
					continue;
				}
			}
			else if(input[i] == '\0' || input[i] == '\n'){
				input[i] = '\0';
				if(strlen(input_args[arg_num]) == 0)
					arg_num--;
				input_args_len = arg_num;
				return 1;
			}
			else{
				input_args[arg_num][char_num] = input[i];
				char_num++;
			}
		}
		printf("Error: %s Invalid Input\n", input);
		return 0;
	}

	for (; i < MAX_INPUT_SIZE; i++) {						// 인자들을 입력받는 반복문. 명령어 뒤에 오는 인자들을 ','로 구분하여 input과 input_args에 저장한다.
		input[i] = getchar();
		if (input[i] == ' ' || input[i] == '\t') {								// 공백 처리
			while (i < MAX_INPUT_SIZE) {
				input[i] = getchar();
				if (input[i] == ' ' || input[i] == '\t')
					continue;
				else if (input[i] == ',') {
					break;
				}
				else if (input[i] == '\n')
					break;
				else if (arg_num == 0)
					break;
				else{
					printf("Error: Invalid Input\n");
					scan_til_line_end();
					return 0;
				}
			}
		}

		if (input[i] == ',') {								// ','으로 인자 구분
			if (arg_num == 0) {
				printf("Error: Invalid Input\n");
				scan_til_line_end();
				return 0;
			}
			input_args[arg_num][char_num] = '\0';
			arg_num++;
			char_num = 0;
			i++;
			while (i < MAX_INPUT_SIZE - 1) {
				input[i] = getchar();
				if (input[i] == ' ' || input[i] == '\t')
					continue;
				else if (input[i] == ',') {
					printf("Error: Invalid Input\n");
					scan_til_line_end();
					return 0;
				}
				else if (input[i] == '\0') {
					printf("Error: Invalid Input\n");
					return 0;
				}
				else
					break;
			}
			input[i + 1] = input[i];
			input[i] = ' ';
			i++;
		}

		if (input[i] == '\n') {								// 개행 문자 처리
			input_args_len = arg_num;
			input[i] = '\0';
			if (arg_num>0)
				input_args[arg_num][char_num] = '\0';
			return 1;
		}
		
		if (arg_num == 0) {
			input[i+1] = input[i];
			input[i] = ' ';
			i++;
			arg_num++;
		}
		if (char_num > 15)
			break;
		input_args[arg_num][char_num] = input[i];
		char_num++;
	}

	scan_til_line_end();

	printf("Error: Invalid Input\n");
	return 0;
}


/*------------------------------------------------------------------------------------*/
/*함수 : hexadecimal_to_decimal()*/
/*목적 : 16진수를 2진수로 변환한다. get_address()와 get_value()에서 호출한다.*/
/*리턴값 : input_args[i] 문자열의 2진수 변환 값*/
/*------------------------------------------------------------------------------------*/
int hexadecimal_to_decimal(int i, int arg_len){
	int k;
	int mul_num = 1;
	char checking_char;
	int sum = 0;
	for (k = 0; k < arg_len; k++, mul_num *= 16) {
		checking_char = input_args[i][arg_len - k - 1];
		if ('0' <= checking_char && checking_char <= '9') {
			sum += (checking_char - '0') * mul_num;
		}
		else if ('A' <= checking_char && checking_char <= 'F') {
			sum += (checking_char - 'A' + 10) * mul_num;
		}
		else if ('a' <= checking_char && checking_char <= 'f') {
			sum += (checking_char - 'a' + 10) * mul_num;
		}
		else {
			return -1;
		}
	}
	return sum;
}


/*------------------------------------------------------------------------------------*/
/*함수 : get_address()*/
/*목적 : input_args[i]문자열 값이 가능한 address값인지 판단한다*/
/*리턴값 : input_args[i]가 valid한 address일 경우 address값, invalid한 address일 경우 -1*/
/*------------------------------------------------------------------------------------*/
int get_address(int i) {
	int arg_len = strlen(input_args[i]);
	int after;
	if (arg_len > 5)
		return -1;
	after = hexadecimal_to_decimal(i, arg_len);
	if (after < MAX_MEMORY_SIZE)
		return after;
	return -1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : get_value()*/
/*목적 : input_args[i]값이 가능한 1byte value값인지 판단한다*/
/*리턴값 : input_args[i]가 valid한 value값일 경우 value값, invalid한 value값일 경우 -1*/
/*------------------------------------------------------------------------------------*/
int get_value(int i) {
	int arg_len = strlen(input_args[i]);
	int after;
	if (arg_len > 2)
		return -1;
	after = hexadecimal_to_decimal(i, arg_len);
	if ((0 <= after && after <= 0xFF))
		return after;
	return -1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : help_action()*/
/*목적 : h[elp] 명령어에 대한 동작을 수행한다.
		모든 명령어 집합을 출력한다. */
/*리턴값 : 1*/
/*------------------------------------------------------------------------------------*/
int help_action() {
	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp][start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\nassemble filename\ntype filename\nsymbol\n");
	return 1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : dir_action()*/
/*목적 : d[ir] 명령어에 대한 동작을 수행한다
		현재 directory의 모든 파일명을 출력한다*/
/*리턴값 : 동작이 정상적으로 수행된 경우 1, 동작이 수행되지 못한 경우 0*/
/*------------------------------------------------------------------------------------*/
int dir_action() {
	DIR *c_dir = opendir(".");
	struct dirent *file = NULL;						// dirent.h가 제공하는 file 정보 구조체. 파일 명을 알 수 있다.
	struct stat buf;								// sys/stat.h가 제공하는 file 정보 구조체. 파일 종류를 알 수 있다.
	if(c_dir == NULL){
		printf("Error: Directory open failed\n");
		return 0;
	}
	while(1){
		file = readdir(c_dir);						// readdir()을 이용하여 지정 폴더 내 파일 정보를 하나씩 가져온다.
		if (file == NULL)							// readdir()이 실패하는 경우는 더 이상 남은 파일이 없는 경우이므로 action을 종료한다
			break;
		stat(file->d_name, &buf);
		if(S_ISDIR(buf.st_mode))					// DIR일 경우 filename/ 출력
			printf("\t%s/", file->d_name);
		else if(S_IEXEC & buf.st_mode)				// EXE를 비롯한 실행 파일일 경우 filename* 출력
			printf("\t%s*", file->d_name);
		else										// 기타 파일일 경우 filename 출력
			printf("\t%s", file->d_name);
	}
	printf("\n");
	closedir(c_dir);
	return 1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : free_all()*/
/*목적 : 프로그램 종료 전 할당한 메모리를 해제한다.*/
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void free_all() {
	int i;
	history_node* tmp_history, *tmp_history2;
	hash_table_node* tmp_hash_table, *tmp_hash_table2;
	free(memory);										// free memory 
	tmp_history = history_list;
	while (tmp_history!=NULL) {							// free history_list 
		tmp_history2 = tmp_history->next;
		free(tmp_history);
		tmp_history = tmp_history2;
	}
	for (i = 0; i < HASH_TABLE_SIZE; i++) {				// free hash_table 
		tmp_hash_table = hash_table[i];
		while (tmp_hash_table != NULL) {
			tmp_hash_table2 = tmp_hash_table->next;
			free(tmp_hash_table);
			tmp_hash_table = tmp_hash_table2;
		}
	}
}

/*------------------------------------------------------------------------------------*/
/*함수 : quit_action()*/
/*목적 : q[uit] 명령어에 대한 동작을 수행한다
		메모리를 해제하고 프로그램을 종료한다*/
/*리턴값 : 1*/
/*------------------------------------------------------------------------------------*/
int quit_action() {
	free_all();
	return 1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : history_action()*/
/*목적 : h[istory] 명령어에 대한 동작을 수행한다
		프로그램이 처음 실행된 시점부터 현재까지 수행한 모든 명령어를 출력한다 */
/*리턴값 : 1*/
/*------------------------------------------------------------------------------------*/
int history_action() {
	int i = 1;
	history_node* current_history;
	current_history = history_list;
	while (current_history != NULL) {
		printf("\t%d\t%s\n", i++, current_history->name);
		current_history = current_history->next;
	}
	printf("\t%d\t%s\n", i, input);
	return 1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : dump_action()*/
/*목적 : du[mp] 명령어에 대한 동작을 수행한다
		지정된 메모리의 값을 출력한다*/
/*리턴값 : 동작이 정상적으로 수행된 경우 1, 동작이 수행되지 못한 경우 0*/
/*------------------------------------------------------------------------------------*/
int dump_action() {
	int j;
	int start, end;
	int last_line, c_line;
	if (input_args_len == 0) {								// 인자 없이 dump만 입력된 경우 start와 end 설정
		start = dump_address;
		end = start + 10 * 16 - 1;
		if (end >= MAX_MEMORY_SIZE) {
			end = MAX_MEMORY_SIZE - 1;
		}
	}
	else if (input_args_len == 1) {							// dump start 가 입력된 경우 start와 end 설정
		start = get_address(1);
		end = start + 10 * 16 - 1;
		if (end >= MAX_MEMORY_SIZE) {
			end = MAX_MEMORY_SIZE - 1;
		}
	}
	else if (input_args_len == 2) {							// dump start end 가 입력된 경우 start와 end 설정
		start = get_address(1);
		end = get_address(2);
		if (end == -1) {
			printf("Error: Invalid address\n");
			return 0;
		}
		if (start > end) {
			printf("Error: start is bigger than end\n");
			return 0;
		}
	}
	else {
		printf("Error: Invalid input\n");
		return 0;
	}
	if (start == -1 || end == -1) {
		printf("Error: Invalid address\n");
		return 0;
	}
	if (start >= MAX_MEMORY_SIZE) {
		printf("Error: Memory out of range\n");
		return 0;
	}
	if (end >= MAX_MEMORY_SIZE) {
		end = MAX_MEMORY_SIZE - 1;
	}

	last_line = (end / 16) + 1;																	
	for (c_line = start / 16; c_line < last_line; c_line++) {							// start address부터 end address까지의 메모리 출력
		printf("%04X0 ", c_line);
		for (j = 0; j < 16; j++) {																	
			if (start <= (c_line * 16 + j) && (c_line * 16 + j) <= end)
				printf("%02X ", memory[c_line * 16 + j]);
			else
				printf("   ");
		}
		printf("; ");
		for (j = 0; j < 16; j++) {
			if (start <= (c_line * 16 + j) && (c_line * 16 + j) <= end) {
				if (0x20 <= memory[c_line * 16 + j] && memory[c_line * 16 + j] <= 0x7E)
					printf("%c", memory[c_line * 16 + j]);
				else
					printf(".");
			}
			else
				printf(".");
		}
		printf("\n");
	}
	dump_address = end + 1;									// 마지막으로 출력된 메모리 주소 저장
	if(dump_address >= MAX_MEMORY_SIZE)
		dump_address = 0;
	return 1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : edit_action()*/
/*목적 : ed[it] 명령어에 대한 동작을 수행한다
		지정된 메모리의 값을 수정한다*/
/*리턴값 : 동작이 정상적으로 수행된 경우 1, 동작이 수행되지 못한 경우 0*/
/*------------------------------------------------------------------------------------*/
int edit_action() {
	int address, value;
	if (input_args_len != 2) {
		printf("Error: 'edit' needs two elements\n");
		return 0;
	}
	address = get_address(1);
	value = get_value(2);
	if (address == -1) {
		printf("Error: Invalid address\n");
		return 0;
	}
	if (value == -1) {
		printf("Error: Invalid value\n");
		return 0;
	}
	*(memory + address) = value;
	return 1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : fill_action()*/
/*목적 : f[ill] 명령어에 대한 동작을 수행한다
		지정된 두 주소값 사이의 주소들의 값을 수정한다*/
/*리턴값 : 동작이 정상적으로 수행된 경우 1, 동작이 수행되지 못한 경우 0*/
/*------------------------------------------------------------------------------------*/
int fill_action() {
	/* write code */
	int i;
	int start, end, value;
	if (input_args_len != 3) {
		printf("Error: 'fill' needs three elements\n");
		return 0;
	}
	start = get_address(1);
	end = get_address(2);
	value = get_value(3);
	if (start == -1) {
		printf("Error: Invalid address\n");
		return 0;
	}
	if (end == -1) {
		printf("Error: Invalid address\n");
		return 0;
	}
	if (value == -1) {
		printf("Error: Invalid value\n");
		return 0;
	}
	for (i = start; i <= end; i++) {
		memory[i] = value;
	}
	return 1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : reset_action()*/
/*목적 : reset 명령어에 대한 동작을 수행한다
		모든 메모리의 값을 0으로 초기화한다*/
/*리턴값 : 1*/
/*------------------------------------------------------------------------------------*/
int reset_action() {
	free(memory);
	memory = (unsigned char*)calloc(MAX_MEMORY_SIZE, sizeof(unsigned char));
	return 1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : find_opcode()*/
/*목적 : hash_table에서 파라미터로 받은 문자열에 해당하는 opcode를 찾는다*/
/*리턴값 : 찾는 데 성공할 경우 opcode값, 실패할 경우 -1*/
/*------------------------------------------------------------------------------------*/
int find_opcode(char* key) {
	int hash = get_hash_code(key);
	hash_table_node* tmp;
	tmp = hash_table[hash];
	while (tmp != NULL) {
		if (strcmp(tmp->key, key) == 0)
			return tmp->opcode;
		tmp = tmp->next;
	}
	return -1;
}

/*------------------------------------------------------------------------------------*/
/*함수 : opcode_action()*/
/*목적 : opcode 명령어에 대한 동작을 수행한다
		key값으로 들어온 문자열에 대한 opcode를 찾아 출력한다*/
/*리턴값 : 동작이 정상적으로 수행된 경우 1, 동작이 수행되지 못한 경우 0*/
/*------------------------------------------------------------------------------------*/
int opcode_action() {
	int opcode;
	if (input_args_len != 1) {
		printf("Error: 'opcode' insturction needs one element\n");
		return 0;
	}
	opcode = find_opcode(input_args[1]);
	if (opcode == -1) {
		printf("Error: Can't find opcode\n");
		return 0;
	}
	printf("opcode is %x\n", opcode);
	return 1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : opcodelist_action()*/
/*목적 : opcodelist 명령어에 대한 동작을 수행한다
		hash table을 출력한다*/
/*리턴값 : 1*/
/*------------------------------------------------------------------------------------*/
int opcodelist_action() {
	hash_table_node* tmp = NULL;
	for (int i = 0; i < HASH_TABLE_SIZE; i++) {
		printf("%d :", i);
		tmp = hash_table[i];
		if (tmp != NULL) {
			while (tmp->next != NULL) {
				printf("[%s,%02X] -> ", tmp->key, tmp->opcode);
				tmp = tmp->next;
			}
			printf("[%s,%02X]", tmp->key, tmp->opcode);
		}
		printf("\n");
	}
	return 1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : type_action()*/
/*목적 : type 명령어에 대한 동작을 수행한다. 
		인자로 들어온 파일명에 해당하는 파일 내용을 출력한다.*/
/*리턴값 : 동작이 정상적으로 수행된 경우 1, 동작이 수행되지 못한 경우 0 1*/
/*------------------------------------------------------------------------------------*/
int type_action(){
	FILE *fp;
	char c;
	if (input_args_len != 1){
		printf("Error: 'type' needs one element\n");
		return 0;
	}
	fp = fopen(input_args[1], "r");
	if (fp == NULL){
		printf("Error: %s does not exist\n", input_args[1]);
		return 0;
	}
	while(0 < fscanf(fp, "%c", &c)){
		printf("%c", c);
	}
	printf("\n");
	fclose(fp);
	return 1;
}

/*------------------------------------------------------------------------------------*/
/*함수 : separate_asm_code()*/
/*목적 : asm 파일에서 읽어들인 코드 한 줄을 다시 개별적인 요소로 나누어 저장한다.*/
/*리턴값 : 저장한 element 갯수*/
/*------------------------------------------------------------------------------------*/
int separate_asm_code(char*code){
	int i;
	int code_len;
	int element_nu = 0;
	int word_nu = 0;
	code_len = strlen(code);
	
	for (i = 0; i < 4; i++){
		memset(input_args[i], '\0', 17);
	}

	if (code[0] == '.')
		return 0;

	for (i = 0; i < code_len; i++){
		if(code[i] == ' ' || code[i] == '\t' || code[i] == ','){
			while(1){
				i++;
				if(i >= code_len || code[i] == '\n' || code[i] == '\0'){
					return element_nu;
				}
				if (code[i] != ' ' && code[i] != '\t')
					break;
			}
			element_nu++;
			word_nu = 0;
		}
		
		if(code[i] == '\n' || code[i] == '\0'){
			return element_nu;
		}
		if (word_nu > MAX_INPUT_SIZE || element_nu > 3){
			return 5;
		}
		input_args[element_nu][word_nu] = code[i];
		word_nu++;
	}
	return element_nu;
}

/*------------------------------------------------------------------------------------*/
/*함수 : add_symtab()*/
/*목적 : symbol table에 새로운 symbol을 저장한다*/
/*리턴값 : 저장에 성공할 경우 1, 실패할 경우 0*/
/*------------------------------------------------------------------------------------*/
int add_symtab(char*symbol, int loc){
	int strcmp_result;
	symtab_node* tmp;
	symtab_node* new;
    new = (symtab_node*)malloc(sizeof(symtab_node));
	strcpy(new->name, symbol);
	new->loc = loc;
	new->next = NULL;
	if (tmp_symtab == NULL){
		tmp_symtab = new;
		return 1;
	}
	if (strcmp(tmp_symtab->name, symbol) > 0){
		new->next = tmp_symtab;
		tmp_symtab = new;
		return 1;
	}

	if( strcmp(tmp_symtab->name, symbol)==0){
		return 0;
	}

	tmp = tmp_symtab;
	while(tmp->next!=NULL){
		strcmp_result = strcmp(tmp->next->name, symbol);
		if (strcmp_result == 0){
			return 0;
		}
		if (strcmp_result > 0){
			new->next = tmp->next;
			tmp->next = new;
			return 1;
		}
		tmp = tmp->next;
	}
	tmp->next = new;
	return 1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : find_symtab()*/
/*목적 : 특정 문자열을 symbol table에서 검색한다.*/
/*리턴값 : 검색에 성공하면 해당 symbol의 location, 검색에 실패하면 -1*/
/*------------------------------------------------------------------------------------*/
int find_symtab(char* symbol){
	symtab_node* tmp;
	tmp = tmp_symtab;
	while(tmp != NULL){
		if (strcmp(symbol, tmp->name) == 0)
			return tmp->loc;
		tmp = tmp->next;
	}
	return -1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : find_opcode_format()*/
/*목적 : opcode hash table에서 target opcode의 format을 구한다.*/
/*리턴값 : 검색에 성공하면 해당 opcode의 format값, 검색에 실패하면 -1*/
/*------------------------------------------------------------------------------------*/
int find_opcode_format(char* key){
	int hash = get_hash_code(key);
	hash_table_node* tmp;
	tmp = hash_table[hash];
	while(tmp!=NULL){
		if(strcmp(tmp->key , key)==0){
			return tmp->format[0] - '0';
		}
		tmp = tmp->next;
	}
	return  -1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : get_instruction_size()*/
/*목적 : hash hable을 이용하여 현재 instruction의 크기를 구한다.*/
/*리턴값 : instruction 크기*/
/*------------------------------------------------------------------------------------*/
int get_instruction_size(int line, char* file_name){
	int format;

	if (input_args[1][0] == '+')
		return 4;
	else if (strcmp(input_args[1], "WORD") == 0){
		return 3;
	}
	else if (strcmp(input_args[1], "RESW") == 0)
		return 3 * atoi(input_args[2]);
	else if (strcmp(input_args[1], "RESB") == 0)
		return atoi(input_args[2]);
	else if (strcmp(input_args[1], "BYTE") == 0){
		if (input_args[2][0] == 'X'){
			if (input_args[2][strlen(input_args[2])-1] == '\\')
				return ((strlen(input_args[2]) - 1) / 2);
			return ((strlen(input_args[2]) - 2) / 2);
		}
		else if (input_args[2][0] == 'C')
			return strlen(input_args[2]) - 3;
		else{
			printf("%s.asm line %d: Invalid Constant Error\n", file_name, line);
			return -1;
		}
	}
	else {
		format = find_opcode_format(input_args[1]);
		if (format >= 0)
			return format;
	}
	
	printf("%s.asm line %d: Invalid Opcode Error\n", file_name, line);
	return -1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : assemble_error_func()*/
/*목적 : assemble 과정 중 에러가 발생할 경우 호출된다.
		에러 메시지를 출력하고, assemble 과정 중 사용하는 메모리와 임시 파일을 삭제한다.*/
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void assemble_error_func(FILE*asm_fp, FILE*obj_fp, FILE*lst_fp, FILE*tmp_fp, FILE* tmp_obj_fp, int line_nu, char*error_msg, char*filename){
	char tmp_filename[50];

	printf("line %d: %s Error\n", line_nu, error_msg);
	if (asm_fp != NULL) {
		fclose(asm_fp);
	}
	if (obj_fp != NULL) {
		fclose(obj_fp);
	}
	if (tmp_fp != NULL) {
		fclose(tmp_fp);
	}
	if (tmp_fp != NULL) {
		fclose(tmp_fp);
	}
	if (tmp_obj_fp != NULL) {
		fclose(tmp_obj_fp);
	}
	strcpy(tmp_filename, filename);
	strcat(tmp_filename, ".lst");
	remove(tmp_filename);
	strcpy(tmp_filename, filename);
	strcat(tmp_filename, ".obj");
	remove(tmp_filename);
	remove("intermediate_file_for_aseembler.txt");
	remove("intermediate_obj_file_for_aseembler.txt");
}


/*------------------------------------------------------------------------------------*/
/*함수 : get_register_num()*/
/*목적 : 레지스터 이름을 번호로 바꾸는 함수*/
/*리턴값 : 레지스터 번호*/
/*------------------------------------------------------------------------------------*/
int get_register_num(char*c) {
	int len = strlen(c);
	if (len == 0)
		return 0;
	if (c[len - 1] == ',') {
		c[len - 1] = '\0';
		len--;
	}
	if (len == 1) {
		switch (c[0]){
		case 'A': return 0;
		case 'X': return 1;
		case 'L': return 2;
		case 'B': return 3;
		case 'S': return 4;
		case 'T': return 5;
		case 'F': return 6;
		}
	}
	if (len > 1) {
		if (c[0] == 'P' && c[1] == 'C')
			return 8;
		if (c[0] == 'S' && c[1] == 'W')
			return 9;
	}
	return -1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : assemble_action()*/
/*목적 : assemble 명령어에 대한 동작을 수행한다.
		assembly 파일을 열고, assemble하여 obj파일과 lst파일과 symbol table을 생성한다.*/
/*리턴값 :  동작이 정상적으로 수행된 경우 1, 동작이 수행되지 못한 경우 0*/
/*------------------------------------------------------------------------------------*/
int assemble_action(){
	FILE *asm_fp = NULL;
	FILE *obj_fp = NULL;
	FILE *lst_fp = NULL;
	FILE *tmp_fp = NULL;
	FILE* tmp_obj_fp = NULL;
	int file_name_len;
	char filename[40];
	char lst_filename[35];
	char obj_filename[35];
	char tmp_asm_code[MAX_INPUT_SIZE];
	int locctr;
	int c_loc;
	int pc;
	int line_nu = 0;
	int start_address = 0;
	int element_nu;
	int loc_size;
	int str_len;
	int instruction;
	int i;
	char c;
	int format;
	int disp;
	int opcode;
	int r1, r2;
	int xbpe;
	int base;
	int line_count = 0;
	int next_line_nu, c_line_end;
	symtab_node* tmp_sym_node;
	int modification[1000];
	int modification_size = 0;
	int is_resbw = 0;
	int shifting;
	int mod;

	// input 처리, 파일 열기
	if (input_args_len != 1){
		printf("Error: 'assemble' needs one element\n");
		return 0;
	}
	file_name_len = strlen(input_args[1]);
	if (file_name_len < 5){
		printf("Error: file must be asm file\n");
		return 0;
	}
	if (strcmp(input_args[1]+(file_name_len-4), ".asm") != 0){
		printf("Error: file must be asm file\n");
		return 0;
	}
	asm_fp = fopen(input_args[1], "r");
	if (asm_fp == NULL) {
		printf("Error: file open failed\n");
		return 0;
	}
	memset(filename, '\0', 35);
	strncpy(filename, input_args[1], file_name_len - 4);

	tmp_fp = fopen("intermediate_file_for_aseembler.txt", "w");
	tmp_obj_fp = fopen("intermediate_obj_file_for_aseembler.txt", "w");

	// pass 1 시작
	while(1){
		// asm 파일에서 line 입력받고 element 단위로 분리
		memset(tmp_asm_code, '\0', MAX_INPUT_SIZE);				
		if (fgets(tmp_asm_code, MAX_INPUT_SIZE, asm_fp) == NULL)
			break;
		element_nu = separate_asm_code(tmp_asm_code);
		line_nu++;
		if (element_nu == 0){
			fprintf(tmp_fp, "%d -1\n", line_nu * 5);
			fputs(tmp_asm_code, tmp_fp);
			continue;
		}
		else if (element_nu > 3){
			assemble_error_func(asm_fp, obj_fp, lst_fp, tmp_fp, tmp_obj_fp, line_nu, "lnvalid expression", filename);
			return -1;
		}
		// start 처리 & start_address, locctr 저장
		if (line_nu == 1){
			if (strcmp(input_args[1], "START") == 0){
				start_address = hexadecimal_to_decimal(2, strlen(input_args[2]));
				locctr = start_address;
				if (start_address < 0){
					assemble_error_func(asm_fp, obj_fp, lst_fp, tmp_fp, tmp_obj_fp, line_nu, "lnvalid value", filename);
					return -1;
				}
				fprintf(tmp_fp, "%d %04X\n", line_nu *5, start_address);
				fputs(tmp_asm_code, tmp_fp);
				continue;
			}
			start_address = 0;
			locctr = start_address;
		}
		if (strlen(input_args[0]) != 0){	// label field에 입력이 있는 경우
			i = add_symtab(input_args[0], locctr);
			if (i == 0){
				assemble_error_func(asm_fp, obj_fp, lst_fp, tmp_fp, tmp_obj_fp, line_nu, "duplicate symbol", filename);
				return -1;
			}
		}
		// 특수 명령어에 대한 intermediate file 작성
		if (strcmp(input_args[1], "END") == 0){
			fprintf(tmp_fp, "%d -1\n", line_nu * 5);
			fputs(tmp_asm_code, tmp_fp);
			fprintf(tmp_obj_fp, "%X %d\n", line_count, (line_nu) * 5);
			break;
		}
		if (strcmp(input_args[1], "BASE") == 0){
			fprintf(tmp_fp, "%d -1\n", line_nu * 5);
			fputs(tmp_asm_code, tmp_fp);
			continue;
		}
		// opcode 찾아서 locctr 업뎃, 파일에 적기
		loc_size = get_instruction_size(line_nu, filename);
		if (loc_size < 0)
			return -1;
		if (strcmp(input_args[1], "RESB") * strcmp(input_args[1], "RESW") == 0) {
			is_resbw += 1;
		}
		else {
			if (loc_size != 0) {

				line_count += loc_size;
				if (is_resbw) {
					fprintf(tmp_obj_fp, "%X %d ", (line_count - loc_size), (line_nu) * 5);
					is_resbw = 0;
					line_count = loc_size;
				}
				if (line_count > MAX_OBJ_LEN) {
					fprintf(tmp_obj_fp, "%X %d ", (line_count - loc_size), (line_nu) * 5);
					line_count = loc_size;
					is_resbw = 0;
				}
			}
		}
		fprintf(tmp_fp, "%d %04X\n", line_nu * 5, locctr);
		fputs(tmp_asm_code, tmp_fp);
		locctr += loc_size;
	}
	fclose(tmp_obj_fp);
	fclose(tmp_fp);
	tmp_fp = NULL;
	tmp_obj_fp = NULL;
	
	// pass2 시작
	tmp_fp = fopen("intermediate_file_for_aseembler.txt", "r");
	tmp_obj_fp = fopen("intermediate_obj_file_for_aseembler.txt", "r");
	strcpy(lst_filename, filename);
	strcat(lst_filename, ".lst");
	strcpy(obj_filename, filename);
	strcat(obj_filename, ".obj");
	obj_fp = fopen(obj_filename, "w");
	lst_fp = fopen(lst_filename, "w");
	fscanf(tmp_obj_fp, "%X",&c_line_end);
	while(1){
		memset(tmp_asm_code, '\0', MAX_INPUT_SIZE);
		if (fscanf(tmp_fp, "%d %X%c", &line_nu, &c_loc, &c) == EOF)
			break;
		if (fgets(tmp_asm_code, MAX_INPUT_SIZE, tmp_fp) == NULL)
			break;

		element_nu = separate_asm_code(tmp_asm_code);
		tmp_asm_code[strlen(tmp_asm_code) - 1] = '\0';
		if (strcmp(input_args[1], "END") == 0) {
			fprintf(obj_fp, "\n");
			fprintf(lst_fp, "%d\t\t    \t%-30s\n", line_nu, tmp_asm_code);
			break;
		}
		// 특수 opcode에 대한 obj, lst파일 작성
		if (line_nu == 5 && strcmp(input_args[1], "START") == 0){
			fprintf(obj_fp, "H%-6s%06X%06X\n", input_args[0], c_loc, locctr - start_address);
			fprintf(obj_fp, "T%06X%02X", start_address, c_line_end);
			fprintf(lst_fp, "%d\t\t%04X\t%-30s\n", line_nu, c_loc, tmp_asm_code);
			fscanf(tmp_obj_fp, "%d %X", &next_line_nu, &c_line_end);
			continue;
		}
		if (line_nu == next_line_nu) {
			fprintf(obj_fp, "\nT%06X%02X", c_loc, c_line_end);
			fscanf(tmp_obj_fp, "%d %X", &next_line_nu, &c_line_end);
		}
		if (element_nu == 0){
			tmp_asm_code[0] = ' ';
			fprintf(lst_fp, "%d\t\t    \t%s\n", line_nu, tmp_asm_code);
			continue;
		}
		if (c_loc < 0) {
			if (strcmp(input_args[1], "BASE") == 0) {
				base = find_symtab(input_args[2]);
			}
			fprintf(lst_fp, "%d\t\t    \t%s\n", line_nu, tmp_asm_code);
			continue;
		}
		if (strcmp(input_args[1], "BYTE") == 0){
			fprintf(lst_fp, "%d\t\t%04X\t%-30s\t\t", line_nu, c_loc, tmp_asm_code);
			if (input_args[2][0] == 'C'){
				str_len = strlen(input_args[2]);
				instruction = 0;
				for (i = 2; i < str_len-1; i++){
					instruction *= 16 * 16;
					instruction += input_args[2][i];
				}
				fprintf(obj_fp, "%X", instruction);
				fprintf(lst_fp, "%X", instruction);
			}
			else{
				str_len = strlen(input_args[2]);
				instruction = 0;
				for (i = 2; i < str_len - 1; i++){
					instruction *= 16;
					c = input_args[2][i];
					if (c >= '0' && c <= '9'){
						fprintf(obj_fp, "%c", c);
						fprintf(lst_fp, "%c", c);
					}
					else if (c >= 'A' && c <= 'F'){
						fprintf(obj_fp, "%c", c);
						fprintf(lst_fp, "%c", c);
					}
					else{
						assemble_error_func(asm_fp, obj_fp, lst_fp, tmp_fp, tmp_obj_fp, line_nu, "Invalid Value", filename);
					}
				}
			}
			fprintf(lst_fp, "\n");

			continue;
		}
		if (strcmp(input_args[1], "WORD") == 0){
			instruction = strtol(input_args[2], NULL, 16);
			fprintf(obj_fp, "%06X", instruction);
			fprintf(lst_fp, "%d\t\t%04X\t%-30s\t%X\n", line_nu, c_loc, tmp_asm_code, instruction);
			continue;
		}
		if (strcmp(input_args[1], "RESW") == 0 || strcmp(input_args[1], "RESB") == 0){
			fprintf(lst_fp, "%d\t\t%04X\t%-30s\n", line_nu, c_loc, tmp_asm_code);
			continue;
		}
		else {
			if (input_args[1][0] == '+')	// format 확인
				format = 4;
			else
				format = find_opcode_format(input_args[1]);
			if (format == 1) {	// format 1 명령어일 경우 obj, lst파일 작성
				opcode = find_opcode(input_args[1]);
				fprintf(lst_fp, "%d\t\t%04X\t%-30s\t\t%02X\n", line_nu, c_loc, tmp_asm_code, opcode);
				fprintf(obj_fp, "%02X", opcode);
			}
			else if (format == 2) {		// format 2 명령어일 경우 obj, lst파일 작성
				opcode = find_opcode(input_args[1]);
				r1 = get_register_num(input_args[2]);
				r2 = get_register_num(input_args[3]);
				if (r1 < 0 || r2 < 0) {
					assemble_error_func(asm_fp, obj_fp, lst_fp, tmp_fp, tmp_obj_fp, line_nu, "Invalid Value", filename);
					return -1;
				}
				fprintf(lst_fp, "%d\t\t%04X\t%-30s\t\t%02X%1d%1d\n", line_nu, c_loc, tmp_asm_code, opcode, r1, r2);
				fprintf(obj_fp, "%02X%1d%1d", opcode, r1, r2);
			}
			if (format == 3) {	// format 3 명령어일 경우 obj, lst파일 작성
				xbpe = 0;
				if (strlen(input_args[2]) == 0) {
					if (strcmp(input_args[1], "RSUB") == 0) {
						fprintf(lst_fp, "%d\t\t%04X\t%-30s\t\t4F%04X\n", line_nu, c_loc, tmp_asm_code, start_address);
						fprintf(obj_fp, "4F%04X", start_address);
						continue;
					}
					else {
						assemble_error_func(asm_fp, obj_fp, lst_fp, tmp_fp, tmp_obj_fp, line_nu, "Invalid Value", filename);
						return -1;
					}
				}
				opcode = find_opcode(input_args[1]);
				pc = c_loc + 3;

				if (input_args[2][0] == '@') {
					opcode += 2;
					strcpy(input_args[2], strtok(input_args[2], "@"));
					disp = find_symtab(input_args[2]) - pc;
				}
				else if (input_args[2][0] == '#') {
					opcode += 1;
					strcpy(input_args[2], strtok(input_args[2], "#"));
					disp = find_symtab(input_args[2]);
					if (disp < 0) {
						disp = strtol(input_args[2], NULL, 10);
						disp += pc;
					}
					disp = disp - pc;
				}
				else {
					opcode += 3;
					disp = find_symtab(input_args[2]) - pc;
				}

				if (input_args[3][0] == 'X')
					xbpe += 8;

				if (disp < 2048 && disp >= -2048) {
					xbpe += 2;
					shifting = (1 << 12) - 1;
					disp = (disp & shifting);
					fprintf(lst_fp, "%d\t\t%04X\t%-30s\t\t%02X%1X%03X\n", line_nu, c_loc, tmp_asm_code, opcode, xbpe, disp);
					fprintf(obj_fp, "%02X2%03X", opcode, disp);
				}
				else {
					xbpe += 4;
					disp = find_symtab(input_args[2]) - base;
					shifting = (1 << 12) - 1;
					disp = (disp & shifting);
					fprintf(lst_fp, "%d\t\t%04X\t%-30s\t\t%02X%1X%03X\n", line_nu, c_loc, tmp_asm_code, opcode, xbpe, disp);
					fprintf(obj_fp, "%02X2%03X", opcode, disp);
				}
			}
			if (format == 4) {	// format 4 명령어일 경우 obj, lst파일 작성
				mod = 1;
				xbpe = 1;
				strcpy(input_args[1], strtok(input_args[1], "+"));
				if (strlen(input_args[2]) == 0) {
					if (strcmp(input_args[1], "RSUB") == 0) {
						fprintf(lst_fp, "%d\t\t%04X\t%-30s\t\t4F%04X\n", line_nu, c_loc, tmp_asm_code, start_address);
						fprintf(obj_fp, "4F%04X", start_address);
						continue;
					}
					else {
						assemble_error_func(asm_fp, obj_fp, lst_fp, tmp_fp, tmp_obj_fp, line_nu, "Invalid Value", filename);
						return -1;
					}
				}
				opcode = find_opcode(input_args[1]);
				if (input_args[2][0] == '@') {
					opcode += 2;
					strcpy(input_args[2], strtok(input_args[2], "@"));
					disp = find_symtab(input_args[2]);
				}
				else if (input_args[2][0] == '#') {
					opcode += 1;
					strcpy(input_args[2], strtok(input_args[2], "#"));
					disp = find_symtab(input_args[2]);
					if (disp < 0) {
						disp = strtol(input_args[2], NULL, 10);
					}
					mod = 0;
				}
				else {
					opcode += 3;
					disp = find_symtab(input_args[2]);
				}
				if (input_args[3][0] == 'X')
					xbpe += 8;
				if (mod) {
					modification[modification_size] = c_loc + 1;
					modification_size++;
				}
				fprintf(lst_fp, "%d\t\t%04X\t%-30s\t\t%02X%1X%05X\n", line_nu, c_loc, tmp_asm_code, opcode, xbpe, disp);
				fprintf(obj_fp, "%02X2%05X", opcode, disp);
			}
		}
	}
	for (int i = 0; i < modification_size; i++)	// modification 작성
		fprintf(obj_fp, "M%06X05\n", modification[i]);
	fprintf(obj_fp, "E%06X", start_address);

	while (symtab != NULL) {	// 이전 symbol table 할당 해제
		tmp_sym_node = symtab->next;
		free(symtab);
		symtab = tmp_sym_node;
	}
	symtab = tmp_symtab;
	fclose(obj_fp);
	fclose(lst_fp);
	fclose(tmp_obj_fp);
	fclose(tmp_fp);
	remove("intermediate_file_for_aseembler.txt");
	remove("intermediate_obj_file_for_aseembler.txt");
	return 1;
}


/*------------------------------------------------------------------------------------*/
/*함수 : symbol_action()*/
/*목적 : symbol에 대한 동작을 수행한다. Symbol table 전체를 출력한다.*/
/*리턴값 :  동작이 정상적으로 수행된 경우 1, 동작이 수행되지 못한 경우 0*/
/*------------------------------------------------------------------------------------*/
int symbol_action() {
	symtab_node* tmp = symtab;
	if (tmp == NULL) {
		printf("no saved symbol table\n");
	}
	while (tmp != NULL) {
		printf("\t%s\t%04X\n", tmp->name, tmp->loc);
		tmp = tmp->next;
	}
	return 1;
}

/*------------------------------------------------------------------------------------*/
/*함수 : progaddr_action()*/
/*목적 : progaddr에 대한 동작을 수행한다. element로 들어온 address를 프로그램 시작 address로 셋팅한다*/
/*리턴값 : 동작이 정상적으로 수행된 경우 1, 동작이 수행되지 못한 경우 0*/
/*------------------------------------------------------------------------------------*/
int progaddr_action(){
	int new_addr;
	if (input_args_len != 1){
		printf("Error: 'progaddr' needs one elements\n");
		return 0;
	}
	new_addr = get_address(1);
	if (new_addr < 0){
		printf("Error: invalid address\n");
		return 0;
	}
	start_addr = new_addr;
	return 1;
}

/*------------------------------------------------------------------------------------*/
/*함수 : find_estab()*/
/*목적 : estab에서 특정 문자열을 찾아 해당 주소값을 반환한다. */
/*리턴값 : 문자열이 estab에 존재하는 경우 해당 주소값, 존재하지 않는 경우 -1*/
/*------------------------------------------------------------------------------------*/
int find_estab(char* c){
	estab_node* tmp = estab_start;
	while(tmp!=NULL){
		if (strcmp(tmp->name, c) == 0)
			return tmp->address;
		tmp = tmp->next;
	}
	return -1;
}

/*------------------------------------------------------------------------------------*/
/*함수 : loader_action()*/
/*목적 : loader에 대한 동작을 수행한다. element로 들어온 obj file을 열고 2pass linking loading을 수행한다.*/
/*리턴값 : 동작이 정상적으로 수행된 경우 1, 동작이 수행되지 못한 경우 0*/
/*------------------------------------------------------------------------------------*/
int loader_action(){
	int csaddr = start_addr;
	int cslth;
	FILE*fp;
	int n;
	char tmp_obj_code[MAX_INPUT_SIZE];
	int element_nu;
	int word_nu;
	int refer_list[5];
	int refer_nu;
	char refer_name[7];
	int tmp, tmp2, a, b, c;
	estab_node* tmp_estab;

	if (input_args_len == 0){
		printf("Error: 'loader' needs at least one obj file\n");
		return 0;
	}
	else if(input_args_len > 3){
		printf("Error: Too many obj files\n");
		return 0;
	}
	if (estab_start != NULL){		// estab 초기화
		while(estab_start != NULL){
			tmp_estab = estab_start->next;
			free(estab_start);
			estab_start = tmp_estab;
		}
		estab_end = NULL;
	}

	// pass 1
	for(n = 0; n < input_args_len; n++){		// 파일 개수만큼 loop
		fp = fopen(input_args[n+1], "r");
		if (fp == NULL){
			printf("Error: File does not exist\n");
			return 0;
		}
		while(1){
			memset(tmp_obj_code, '\0', MAX_INPUT_SIZE);
			if(fgets(tmp_obj_code, MAX_INPUT_SIZE, fp) ==NULL)			// 한 줄 scan
				break;
			if(tmp_obj_code[0] == 'H'){									// header record인 경우, estab 추가
				tmp_estab = (estab_node*)malloc(sizeof(estab_node));
				strcpy(input_args[0], tmp_obj_code + 13);
				cslth = hexadecimal_to_decimal(0, 6);
				if (cslth < 0){
					printf("Error: %s %c\n", input_args[n+1], tmp_obj_code[0]);
					free(tmp_estab);
					return 0;
				}
				tmp_estab->length = cslth;
				tmp_estab->address = csaddr;
				strncpy(tmp_estab->name, tmp_obj_code + 1, 6);
				tmp_estab->name[6] = '\0';
				tmp_estab->next = NULL;
				if(estab_start == NULL){
					estab_start = tmp_estab;
					estab_end = tmp_estab;
				}
				else{
					estab_end->next = tmp_estab;
					estab_end = tmp_estab;
				}
			}
			if (tmp_obj_code[0] == 'D'){								// define record인 경우, estab 추가
				word_nu = 1;
				while(1){
					if (tmp_obj_code[word_nu] == '\0' || tmp_obj_code[word_nu] == '\n')
						break;
					tmp_estab = (estab_node*)malloc(sizeof(estab_node));
					strncpy(tmp_estab->name, tmp_obj_code + word_nu, 6);
					tmp_estab->name[6] = '\0';
					word_nu += 6;
					tmp_estab->length = -1;
					strncpy(input_args[0], tmp_obj_code+word_nu, 6);
					input_args[0][6] = '\0';
					tmp_estab->address = hexadecimal_to_decimal(0, 6);
					if (tmp_estab->address < 0){
						printf("Error: %s %c\n", input_args[n+1], tmp_obj_code[0]);
						free(tmp_estab);
						return 0;
					}
					word_nu += 6;
					tmp_estab->address += csaddr;
					estab_end->next = tmp_estab;
					estab_end = tmp_estab;
				}
			}
			if (tmp_obj_code[0] == 'E'){				// end record인 경우, csaddr 계산 후 loop 종료
				csaddr += cslth;
				break;
			}
		}
		fclose(fp);
	}

	// print estable
	printf("control symbol address length\n");
	printf("section name\n");
	printf("-----------------------------------\n");
	tmp_estab = estab_start;
	while(tmp_estab != NULL){
			if(tmp_estab->length > 0)
			printf("%s\t\t%04X\t%04X\n",tmp_estab->name, tmp_estab->address, tmp_estab->length);
		else
			printf("\t%s\t%04X\n", tmp_estab->name, tmp_estab->address);
		tmp_estab = tmp_estab->next;
	}
	printf("-----------------------------------\n");
	prog_lth = csaddr - start_addr;
	printf("\t\ttotal\tlength\t%04X\n", prog_lth);
	csaddr = start_addr;

	// pass2
	for(n = 0; n < input_args_len; n++){						// obj file 갯수만큼 loop
		fp = fopen(input_args[n+1], "r");
		if (fp == NULL){
			printf("Error: File open error\n");
			return 0;
		}
		while(1){
			memset(tmp_obj_code, '\0', MAX_INPUT_SIZE);				// 한 줄 scan
			if(fgets(tmp_obj_code, MAX_INPUT_SIZE, fp) == NULL)
				break;
			if(tmp_obj_code[0] == 'H'){								// header record인 경우 cs length 저장
				strcpy(input_args[0], tmp_obj_code + 13);
				input_args[0][6] = '\0';
				cslth = hexadecimal_to_decimal(0, 6);
			}
			else if (tmp_obj_code[0] == 'R'){						// reference record인 경우 referenc data 저장
				word_nu = 1;
				while(1){
					if(tmp_obj_code[word_nu] == '\0' || tmp_obj_code[word_nu] == '\n')
						break;
					input_args[0][0] = tmp_obj_code[word_nu++];
					input_args[0][1] = tmp_obj_code[word_nu++];
					input_args[0][2] = '\0';
					refer_nu = hexadecimal_to_decimal(0,2);
					if(refer_nu < 0){
						printf("Error: invalid value\n");
						return 0;
					}
					strncpy(refer_name, tmp_obj_code + word_nu, 6);
					refer_name[6] = '\0';
					for(tmp = 0; tmp < 6; tmp++){
						if(refer_name[tmp] == '\n' || refer_name[tmp] == '\0')
							refer_name[tmp] = ' ';
					}
					refer_list[refer_nu] = find_estab(refer_name);
					word_nu += 6;
					if (refer_list[refer_nu] < 0){
						printf("Error: Can't find %s at estab\n", refer_name);
						return 0;
					}
				}
				refer_list[1] = csaddr;
		 	}
			else if(tmp_obj_code[0] == 'T'){					// text record인 경우 memory에 프로그램 load
				strncpy(input_args[0], tmp_obj_code + 1, 6);
				input_args[0][6] = '\0';
				tmp = hexadecimal_to_decimal(0, 6);
				strncpy(input_args[0], tmp_obj_code + 7, 2);
				input_args[0][2] = '\0';
				element_nu = hexadecimal_to_decimal(0, 2);
				for (word_nu = 0; word_nu < element_nu; word_nu++){
					strncpy(input_args[0], tmp_obj_code + 9 + (word_nu * 2), 2);
					memory[tmp + word_nu + csaddr] = (unsigned int)hexadecimal_to_decimal(0,2);
				}
			}
			else if(tmp_obj_code[0] == 'M'){					// modification record인 경우 load된 memory modify
				strncpy(input_args[0], tmp_obj_code + 1, 6);
				input_args[0][6] = '\0';
				tmp = hexadecimal_to_decimal(0, 6);
				strncpy(input_args[0], tmp_obj_code + 7, 2);
				input_args[0][2] = '\0';
				word_nu = hexadecimal_to_decimal(0, 2);
				strncpy(input_args[0], tmp_obj_code + 10, 2);
				input_args[0][2] = '\0';
				refer_nu = hexadecimal_to_decimal(0, 2);
				a = (int)memory[csaddr + tmp];
				b = (int)memory[csaddr + tmp + 1];
				c = (int)memory[csaddr + tmp + 2];
				sprintf(input_args[0], "%02X%02X%02X", a,b,c);
				tmp2 = (int)strtol(input_args[0], NULL, 16);
				if(word_nu == 5)
					tmp2 = tmp2 & 0XFFFFF;
				if (tmp_obj_code[9] == '+')
					tmp2 += refer_list[refer_nu];
				else
					tmp2 -= refer_list[refer_nu];
				memory[csaddr+tmp+2] = (unsigned int)(tmp2 & 0xFF );
				memory[csaddr+tmp+1] = (unsigned int)((tmp2 & 0xFF00)>>8);
				if(word_nu == 5)
					memory[csaddr+tmp] = ((tmp&0xF0000) >> 16) + (a&0xF0);
				else
					memory[csaddr+tmp] = (tmp2 & 0xFF0000)>>16;
			}
			else if(tmp_obj_code[0] == 'E'){			// end record인 경우 csaddr 설정 후 loop 종료
				csaddr += cslth;
				break;
			}
		}
		fclose(fp);
	}
	return 1;
}

/*------------------------------------------------------------------------------------*/
/*함수 : bp_action()*/
/*목적 : bp에 대한 동작을 수행한다. element로 address값이 입력된 경우 해당 address를 bp로 셋팅한다. 
		clear가 들어온 경우 기존 bp를 삭제한다. element 없이 입력된 경우 셋팅된 모든 bp를 출력한다.*/
/*리턴값 : 동작이 정상적으로 수행된 경우 1, 동작이 수행되지 못한 경우 0*/
/*------------------------------------------------------------------------------------*/
int bp_action(){
	bp_node* tmp;
	tmp = bp;
	if (input_args_len == 0){
		printf("\t\tbreakpoint\n\t\t----------\n");
		while(tmp!=NULL){
			printf("\t\t%X\n", tmp->bp_loc);
			tmp = tmp->next;
		}
	}
	else if (input_args_len == 1){
		if(strcmp(input_args[1], "clear") == 0){
			while(tmp != NULL){
				bp = tmp->next;
				free(tmp);
				tmp = bp;
			}
			printf("\t\t[ok] clear all breakpoints\n");
		}
		else{
			tmp = (bp_node*)malloc(sizeof(bp_node));
			tmp->bp_loc = get_address(1);
			if(tmp->bp_loc<0){
				printf("Error: Invalide address\n");
				return 0;
			}
			tmp->next = bp;
			bp = tmp;
			printf("\t\t[ok] create breakpoint %s\n", input_args[1]);
		}
	}
	else{
		printf("Error: 'bp' needs less than two elements\n");
		return 0;
	}
	return 1;
}

/*------------------------------------------------------------------------------------*/
/*함수 : set_cc()*/
/*목적 : 두 정수의 대소 관계를 비교하여 cc를 셋팅한다. */
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void set_cc(int r1, int r2){
	if(r1 > r2)
		regi[7] = '>';
	else if(r1 == r2)
		regi[7] = '=';
	else
		regi[7] = '<';
}

/*------------------------------------------------------------------------------------*/
/*함수 : store()*/
/*목적 : run 동작 중 store 계열 동작을 수행하는 함수이다. */
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void store(int r, int a){
	int tmp = regi[r];
	memory[a+2] = (unsigned int)(0xFF & regi[r]);
	tmp /= 0x100;
	memory[a+1] = (unsigned int)(0xFF & tmp);
	tmp /= 0x100;
	memory[a] = (unsigned int)tmp;
}

/*------------------------------------------------------------------------------------*/
/*함수 : load()*/
/*목적 : run 동작 중 load 계열 동작을 수행하는 함수이다. */
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void load(int r, int a, int n, int i){
	if( n ==0 && i==1)
		regi[r] = a;
	else{
		regi[r] = memory[a] * 0x100;
		regi[r] += memory[a+1] * 0x100;
		regi[r] += memory[a+2];
	}
}

/*------------------------------------------------------------------------------------*/
/*함수 : find_bp()*/
/*목적 : 현재 pc가 bp인지 판단한다 */
/*리턴값 : bp일 경우 1, 아닐 경우 0 */
/*------------------------------------------------------------------------------------*/
int find_bp(int loc){
	bp_node *tmp = bp;
	while(tmp !=NULL){
		if (loc == tmp->bp_loc)
			return 1;
		tmp = tmp->next;
	}
	return 0;
}

/*------------------------------------------------------------------------------------*/
/*함수 : run_action()*/
/*목적 : run에 대한 동작을 수행한다. load된 메모리를 통해 프로그램을 실행한다. */
/*리턴값 : 동작이 정상적으로 수행된 경우 1, 동작이 수행되지 못한 경우 0*/
/*------------------------------------------------------------------------------------*/
int run_action(){ 
	int opcode;
	int r1, r2;
	int n,i,x,b,p,e;
	int disp;
	int tmp;

	if( is_running == 0){					// 프로그램 실행 전 register 초기화
		pc = start_addr;
		for (i=0;i<8;i++){
			if (i == 2)
				continue;
			regi[i] = 0;
		}
		regi[6] = pc;
		regi[2] = prog_lth;
		is_running = 1;
	}
	if(prog_lth == 0){
		printf("Error: No Program Loaded\n");
		is_running = 0;
		return 0;
	}

	while(1){
		tmp = (int)memory[pc++];
		opcode = tmp & 0xFC;
		if(opcode == 0xB4 || opcode == 0xA0 || opcode == 0xB8){ 	// claer, compr, tixr은 2형식
			r1 = (int)memory[pc++];
			r2 = r1 & 0x0F;
			r1 = (r1 & 0xF0) / 16;
			if (opcode == 0xB4)							//clear
				regi[r1] = 0;
			else if( opcode == 0xA0)					//compr
				set_cc(regi[r1], regi[r2]);
			else{										//tixr
				regi[1]++;
				set_cc(regi[1], regi[r1]);
			}
		}
		else{
			n = (tmp & 0x02)/2;							// nixbpe 계산
			i = (tmp & 0x01);
			tmp = memory[pc++];
			x = (tmp & 0x80) >> 7;
			b = (tmp & 0x40) >> 6;
			p = (tmp & 0x20) >> 5;
			e = (tmp & 0x10) >> 4;
			
			disp = (tmp & 0x0F)*16*16;					// 3형식 display 계산
			disp += memory[pc++];
			if(e == 1){									// 4형식 display 계산
				disp *= 16*16;
				disp+=memory[pc++];
			}
			if (p == 1 && b == 0){						// pc relative addressing
				if( e == 0 && (disp & 0x800) > 0)		// 음수일 경우
					disp -= 0x1000;
				if (e == 1 && (disp & 0x8000) > 0)
					disp -= 0x10000;
				disp += pc;
			}
			else if(p == 0 && b == 1)					// base relative addressing
				disp += regi[3];

			if(n == 1 && i == 0){
				tmp = disp;
				disp = (int)memory[tmp++] * 0x100;
				disp += (int)memory[tmp++] * 0x100;
				disp += (int)memory[tmp];
			}

			if (x == 1)					// indexed
				disp += regi[1];
			
			if (opcode == 0x14)			// stl
				store(2, disp);
			else if(opcode == 0x68)		// ldb
				load(3, disp, n, i);
			else if(opcode == 0x48){	// jsub
				regi[2] = pc;
				pc = disp;
			}
			else if(opcode == 0x00)		// lda
				load(0, disp, n, i);
			else if(opcode == 0x28){	// comp
				if( n==0 && i == 1)
					tmp = disp;
				else{
					tmp = memory[disp+2] * 0x100;
					tmp += memory[disp+1] * 0x100;
					tmp += memory[disp];
				}
				set_cc(regi[0], tmp);
			}
			else if(opcode == 0x30){	// jeq
				if(regi[7] == '=')
					pc = disp;
			}
			else if(opcode == 0x3C)		// j
				pc = disp;
			else if(opcode == 0x0C)		// sta
				store(0, disp);
			else if(opcode == 0x74)		// ldt
				load(5, disp, n, i);
			else if(opcode == 0xE0)		// td
				regi[7] = '<';
			else if(opcode == 0xD8)		// rd
				regi[0] = (regi[7] /0x100) * 0x100;
			else if(opcode == 0x54)		// stch
				memory[disp] = regi[0] % 0x100;
			else if(opcode == 0x38){	// jlt
				if(regi[7] == '<')
					pc = disp;
			}
			else if(opcode == 0x10)		// stx
				store(1, disp);
			else if(opcode == 0x4C) 	// rsub
				pc = regi[2];
			else if(opcode == 0x50){	// ldch
				regi[0] = (regi[0] / 0x100) * 0x100;
				regi[0] += 0xFF & memory[disp];
			}
			else if(opcode == 0xDC);
			else{
				printf("Error: Can't find such opcode\n");
				return 0;
			}
		}
		regi[6] = pc;
		if (pc >= prog_lth + start_addr){	// 프로그램 종료
			is_running = 0;
			printf("A : %06X  X : %06X\nL : %06X PC : %06X\nB : %06X  S : %06X\nT : %06X\n", regi[0], regi[1], regi[2], regi[6], regi[3], regi[4], regi[5]);
			printf("\t\tEnd Program\n");
			return 1;
		}
		if(find_bp(pc)){					// break point
			printf("A : %06X  X : %06X\nL : %06X PC : %06X\nB : %06X  S : %06X\nT : %06X\n", regi[0], regi[1], regi[2], regi[6], regi[3], regi[4], regi[5]);
			printf("\t\tStop at checkpoint[%X]\n", regi[6]); 
			return 1;
		}
	}
	return 0;
}


/*------------------------------------------------------------------------------------*/
/*함수 : add_history()*/
/*목적 : 현재 명령어를 history에 추가한다*/
/*리턴값 : 없음*/
/*------------------------------------------------------------------------------------*/
void add_history() {
	history_node* new_history;
	new_history = (history_node*)malloc(sizeof(history_node));
	strcpy(new_history->name, input);
	new_history->next = NULL;
	if (history_list == NULL){
		history_list = new_history;
		last_history = new_history;
	}
	else{
		last_history->next = new_history;
		last_history = new_history;
	}
	return;
}

/*------------------------------------------------------------------------------------*/
/*함수 : input_action()*/
/*목적 : 들어온 명령어 하나에 대한 동작을 수행하고 history를 저장한다*/
/*리턴값 : quit명령어가 수행된 경우 1, 그 외 0*/
/*------------------------------------------------------------------------------------*/
int input_action() {
	int changed = 0;
	if (strcmp("h", input) == 0 || strcmp("help", input) == 0)
		changed = help_action();
	else if (strcmp("d", input) == 0 || strcmp("dir", input) == 0)
		changed = dir_action();
	else if (strcmp("q", input) == 0 || strcmp("quit", input) == 0) {
		changed = quit_action();
		return changed;
	}
	else if (strcmp("hi", input) == 0 || strcmp("history", input) == 0)
		changed = history_action();
	else if (strcmp("du", input_args[0]) == 0 || strcmp("dump", input_args[0]) == 0)
		changed = dump_action();
	else if (strncmp("e ", input, 2) == 0 || strncmp("edit ", input, 5) == 0)
		changed = edit_action();
	else if (strncmp("f ", input, 2) == 0 || strncmp("fill ", input, 5) == 0)
		changed = fill_action();
	else if (strcmp("reset", input) == 0)
		changed = reset_action();
	else if (strncmp("opcode ", input, 7) == 0)
		changed = opcode_action();
	else if (strcmp("opcodelist", input) == 0)
		changed = opcodelist_action();
	else if (strncmp("type ", input, 5) == 0)
		changed = type_action();
	else if (strncmp("assemble ", input, 9) == 0)
		changed = assemble_action();
	else if (strcmp("symbol", input) == 0)
		changed = symbol_action();
	else if (strcmp("progaddr", input_args[0]) == 0)
		changed = progaddr_action();
	else if (strncmp("loader ", input, 7) == 0)
		changed = loader_action();
	else if (strcmp("bp", input_args[0]) == 0)
		changed = bp_action();
	else if (strcmp("run", input) == 0)
		changed = run_action();
	else
		printf("Error: Invalid Input\n");

	if (changed)
		add_history();
	return 0;
}


/* main fucntion */
int main() {
	int is_quit = 0;
	if (!make_hash_table())														// hash table 생성
		return 0;

	symtab = NULL;
	history_list = NULL;														// history_list 변수 초기화
	memory = (unsigned char*)calloc(MAX_MEMORY_SIZE, sizeof(unsigned char));	// memory 생성
	dump_address = 0;															// dump_address 변수 초기화
	start_addr = 0;																// start_addr 변수 초기화
	is_running = 0;																// is_running 변수 초기화
	bp = NULL;																	// bp 변수 초기화

	while (!is_quit) {													
		init_input_variables();													// 입력 변수 메모리할당 및 초기화
		if(get_input())															// 입력
			is_quit = input_action();											// 동작
		free(input);															// 메모리 해제
	}
	return 0;
}
