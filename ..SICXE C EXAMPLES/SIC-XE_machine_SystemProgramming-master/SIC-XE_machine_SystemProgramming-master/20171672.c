#include "20171672.h"

/*------------------------------------------------*/
/*Function : InsertNode()*/
/*Purpose : Put user's input into linked list for 'History' command.*/
/*No return value.*/
/*------------------------------------------------*/

void InsertNode(NODE *head, char input[]){
    
    struct NODE *new = malloc(sizeof(struct NODE));
    strcpy(new->command, input);
    new->link = head->link;
    head->link = new;
}

/*------------------------------------------------*/
/*Function : FreeNode()*/
/*Purpose : Free the node.*/
/*No return value.*/
/*------------------------------------------------*/

void FreeNode(NODE *head){

    while(head->link != NULL){
        NODE *tmp = head;
        head = head->link;
        free(tmp);
    }

}

/*------------------------------------------------*/
/*Function : FreeHash()*/
/*Purpose : Free the hashtable.*/
/*No return value.*/
/*------------------------------------------------*/

void FreeHash(){

    for(int i = 0; i < HASHSIZE; i++){
        while(hashtable[i] != NULL){
          OPTAB *tmp = hashtable[i];
          hashtable[i] = hashtable[i]->next;
          free(tmp);
        }
    }
}

/*------------------------------------------------*/
/*
    Function : FreeSHash()
    Purpose : Free the symbol table
    Return value : 1 or 0
*/
/*------------------------------------------------*/

void FreeSHash(){

    for(int i = 0; i < HASHSIZE; i++){
        while(s_hashtable[i] != NULL){
            HASH_SYMTAB *tmp = s_hashtable[i];
            s_hashtable[i] = s_hashtable[i]->next;
            free(tmp);
        }
    }

}

/*------------------------------------------------*/
/*Function : help()*/
/*Purpose : Function 'help' command.*/
/*No return value.*/
/*------------------------------------------------*/

int help(){

	printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\nassemble filename\ntype filename\nsymbol\n");

}

/*------------------------------------------------*/
/*Fucntion : dir()*/
/*Purpose : Function 'dir' command. 
            Print current directory's file name and state. */
/*No return value*/
/*------------------------------------------------*/

    
void dir(DIR *dp, struct dirent *file, struct stat buf){
    dp = opendir(".");

    if(dp == NULL){
      printf("There is no directory.\n");
      return;
    }

    while(1){
      file = readdir(dp);
      if(!file) break;
      
      if(strcmp(file->d_name, ".") == 0) continue;
      if(strcmp(file->d_name, "..") == 0) continue;
      stat(file->d_name, &buf);
      if(buf.st_mode & S_IFDIR){
        printf("%s/   ", file->d_name);
      }
      else if(buf.st_mode & S_IXUSR){
        printf("%s*   ", file->d_name);
      }
      else{
        printf("%s    ", file->d_name);

      }

    }
    
    closedir(dp);

    printf("\n");
}

/*------------------------------------------------*/
/* 
   Function : history()
   Purpose : Function 'history' command.
             Print all the previous valid commands in the linked list.
   No return value.
*/
/*------------------------------------------------*/

int history(NODE *current){
    int count=1;
    while(current != NULL){
      printf("%d  %s\n", count, current->command);
      current = current->link;
      count++;
    }

}

/*------------------------------------------------*/
/*
    Function : dump()
    Purpose : Function 'dump', 'dump start', and 'dump start, end' command.
              Print the memory address, memory content, and content in ASCII
              code.
    No return value.
*/
/*------------------------------------------------*/

void dump(unsigned char *BUFFER, int *START_ADDRESS, int *END_ADDRESS){
  
  int i=0, j = 0, k = 0;
  int MEMORY_NUM = 0, LIMIT_MEMORY = 160;
  int START_X=(*START_ADDRESS)/16, START_Y= (*START_ADDRESS)%16;
  int Hex_Num = START_X, END_SIGN=0, IsCompleteLine = 0;
  unsigned char **NEW_BUFFER;

  NEW_BUFFER = (unsigned char**)malloc(sizeof(unsigned char*)*MEGA_BYTE_ROW);

  for(i=0; i < MEGA_BYTE_ROW; i++){
      NEW_BUFFER[i] = (unsigned char *)malloc(sizeof(unsigned char) * 16);
  }
  if(*END_ADDRESS != 0){
      LIMIT_MEMORY = *END_ADDRESS - *START_ADDRESS; // LIMIT SETTING
  }


  for(i=START_X; i < 11+START_X+LIMIT_MEMORY; i++){
        for(k = 0; k < 16; k++){
            NEW_BUFFER[i][k] = BUFFER[j + *START_ADDRESS];
            j++;
        }
  }


  i=START_X;
  while(1){
      for(k=0; k < 16; k++){
       if(i*16 + k > 0xfffff){
         printf("   ");
         END_SIGN = 1;
       }

       if(MEMORY_NUM == LIMIT_MEMORY){
           END_SIGN = 1;
       }
       if(END_SIGN == 1){
           if(k == 0){
               IsCompleteLine = 1;
               break;
           }
           else printf("   ");
       }
       else{
          if(k == 0) printf("%05x", Hex_Num*16);

          if(i == START_X  && k < START_Y){
            printf("   ");
          }
          else{
            printf(" %02X", NEW_BUFFER[i][k]);
            MEMORY_NUM++;
          }
        }
      }
    if(IsCompleteLine == 1) break;
    printf(" ; ");
    for(k=0; k < 16; k++){
        if(i == START_X && k < START_Y){
          printf(".");
        }
        else{
            if(END_SIGN != 1){
                if(NEW_BUFFER[i][k] >= 0x20 && NEW_BUFFER[i][k] <= 0x7E){
                printf("%C", NEW_BUFFER[i][k]);
                }
                else printf(".");
            }
            else printf(".");
        }
    }
   
    printf("\n");
    if(END_SIGN == 1) break;
    Hex_Num++;
    i++;
  
  }
      printf("\n");

      for(i=0; i < MEGA_BYTE_ROW; i++){
          free(NEW_BUFFER[i]);
      }
      free(NEW_BUFFER);

}

/*------------------------------------------------*/
/*
      Function : edit()
      Purpose : Function 'edit address value' command.
      No return value.
*/
/*------------------------------------------------*/

void edit(unsigned char *BUFFER, int *USER_ADDRESS, int *EDIT_VALUE){
    
    BUFFER[(unsigned int)(*USER_ADDRESS)] = (unsigned char)(*EDIT_VALUE);
    printf("USER ADDRESS is %X\n", *USER_ADDRESS);
    printf("BUFFER is %X\n", BUFFER[(unsigned int)(*USER_ADDRESS)]); 

}

/*------------------------------------------------*/
/*
      Function : fill()
      Purpose : Function 'fill' command.
      No return value.
*/
/*------------------------------------------------*/

void fill(unsigned char *BUFFER, int *START_ADDRESS, int *END_ADDRESS, int *FILL_VALUE){
    int i;

    for(i = *START_ADDRESS; i < *END_ADDRESS; i++){
        BUFFER[i] = (unsigned char)(*FILL_VALUE);
    }

}

/*------------------------------------------------*/
/*
    Function : add_hash()
    Purpose : Insert a node into hashtable of specific index.
    No return value.
*/
/*------------------------------------------------*/

void add_hash(int key, OPTAB* node){
    int index = key % HASHSIZE;
    OPTAB* cur = (OPTAB*)malloc(sizeof(OPTAB));

    if(hashtable[index] == NULL){
        hashtable[index] = node;
    }
    else{
        cur = hashtable[index];
        while(cur->next != NULL){
            cur = cur->next;
        }
        cur->next = node;
    }

}

/*------------------------------------------------*/
/*
    Function : create_hash()
    Purpose : Create hashtable by reading from the file.
    No return value.
*/
/*------------------------------------------------*/

void create_hash(){
    FILE *fp1 = fopen("opcode.txt", "r");
    char code[100], name[100], format[100];
    srand(time(NULL));

    if(fp1 == NULL){
        printf("File Open Error!\n");
        return;
    }

    while(!feof(fp1)){

        fscanf(fp1, "%s%*[ \t]%s%*[ \t]%s\n", code, name, format);
        OPTAB* node = (OPTAB*)malloc(sizeof(OPTAB));
        node->id = rand() % HASHSIZE;
        strcpy(node->code, code);
        strcpy(node->name, name);
        strcpy(node->format, format);
        node->next = NULL;
        add_hash(node->id, node);
    }
    fclose(fp1);
}

/*------------------------------------------------*/
/*    
      Function : opcodelist()
      Purposse : Function 'opcodelist' command.
      No return value.
*/
/*------------------------------------------------*/

void opcodelist(){

    for(int i = 0; i < HASHSIZE; i++){
        printf("%d : ", i);
        if(hashtable[i] != NULL){
            OPTAB* node = hashtable[i];
            while(node->next != NULL){
                printf("[%s, %s] -> ", node->name, node->code);
                node = node->next;
            }
            printf("[%s, %s]", node->name, node->code);
        }

        printf("\n");
    }

}

/*------------------------------------------------*/
/*
    Function : find_opcode()
    Purpose : Function 'opcode menomonic' command.
    Return value : IsCommanded 0 or 1
*/
/*------------------------------------------------*/


int find_opcode(char *INPUT_MN){

    FILE *fp2 = fopen("opcode.txt", "r");
    char code[100], name[100];
    int MAX_SIZE = 100;

    if(fp2 == NULL){
        printf("File Open Error!\n");
        return 0 ;
    }

    while(!feof(fp2)){
        fscanf(fp2, "%s%*[ \t]%s%*[ \t]%*s\n", code, name);
        if(strcmp(name, INPUT_MN) == 0){
            printf("opcode is %s.\n", code);
            fclose(fp2);
            return 1;
        }

    }

    fclose(fp2);
    return 0;

}

/*------------------------------------------------*/
/*
    Function : type()
    Purpose : Function 'type' command.
    Return value : IsCommanded 0 or 1
*/
/*------------------------------------------------*/

int type(DIR *dp, struct dirent *file, struct stat buf, char *AA){

    FILE *fp3;
    char ch;
    dp = opendir(".");
    if(dp == NULL){
        printf("There is no directory.\n");
        closedir(dp);
        return 0;
    }
    printf("\n");
    while(1){
        file = readdir(dp);
        if(!file) {
            printf("There is no file here!\n");
            break;
        }
        if(strcmp(file->d_name, AA) == 0){
            if(buf.st_mode & S_IFDIR) continue;

            fp3 = fopen(AA, "r");
            if(fp3 == NULL){
                closedir(dp);
                return 0;
            }
            while(1){
                ch = fgetc(fp3);
                if(ch == EOF) break;
                putchar(ch);
            }
            printf("\n");   
            fclose(fp3);
            closedir(dp);
            return 1;
        }
    }
    closedir(dp);
    return 0;
}

/*------------------------------------------------*/
/*
    Function : readline() 
    Purpose : Read label from asm file and skip spaces.
    Return value : LABEL
*/
/*------------------------------------------------*/

char* readline(){

    int JNDX = 0;
    LABEL[0] = '\0';
    while(BUFF[INDX] != ' ' && BUFF[INDX] != '\t' && BUFF[INDX] != '\n'){
        LABEL[JNDX++] = BUFF[INDX++];
    }
    LABEL[JNDX] = '\0';
    return(LABEL);

}

/*------------------------------------------------*/
/*
    Function : skip()
    Purpose : To skip space and tab.
    Return value : None.
*/
/*------------------------------------------------*/

void skip(){
    while(BUFF[INDX] == ' ' || BUFF[INDX] == '\t') INDX++;
}


/*------------------------------------------------*/
/*
    Function : readmnmn()
    Purpose : Read mnemonic from the file.
    Return value : MNMN
*/
/*------------------------------------------------*/

char* readmnmn(){
    int JNDX = 0;
    MNMN[0] = '\0';
    while(BUFF[INDX] != ' ' && BUFF[INDX] != '\t' && BUFF[INDX] != '\n'){
        MNMN[JNDX++] = BUFF[INDX++];
    }
    MNMN[JNDX] = '\0';
    return(MNMN);
}

/*------------------------------------------------*/
/*
    Function : readopnd()
    Purpose : Read operand from the file.
    Return value : OPND
*/
/*------------------------------------------------*/

char* readopnd(){
    int temp = 0, flag = 0;
    int JNDX = 0;
    OPND[0] = '\0';
    temp = INDX;
    while(1){
        if(BUFF[temp] == '\0') break;
        if(BUFF[temp] == ',') flag = 1;
        temp++;
    }

    if(flag == 0){
      while((BUFF[INDX] != '\n') && (BUFF[INDX] != '\r') && (BUFF[INDX] != ' ') && (BUFF[INDX] != '\t') && (INDX < BUFF_length)){
          OPND[JNDX] = BUFF[INDX];
          JNDX++;
          INDX++;
      }
      OPND[JNDX] = '\0';
    }
    else{
        while(BUFF[INDX-1] != ','){
            OPND[JNDX] = BUFF[INDX];
            JNDX++;
            INDX++;
        }
        skip();
        while((BUFF[INDX] != '\n') && (BUFF[INDX] != '\r') && (BUFF[INDX] != ' ') && (BUFF[INDX] != '\t') && (INDX < BUFF_length)){
            OPND[JNDX] = BUFF[INDX];
            JNDX++;
            INDX++;
        }
        OPND[JNDX] = '\0';
    }
    return(OPND);
}

/*------------------------------------------------*/
/*
    Function : readprefix()
    Purpose : Read mnemonic and determine what the prefix is.
    Return value : prefix
*/
/*------------------------------------------------*/

int readprefix(char *MNMN){
    prefix = 0;
    switch (MNMN[0]){
      case '+':
        prefix = 1;
        break;
      case '#':
        prefix = 2;
        break;
      case '@':
        prefix = 3;
        break;
      default:
        prefix = 0;
    }
    return prefix;

}

/*------------------------------------------------*/
/*
    Function : searchSYMTAB()
    Purpose : Search label from the symbol table.
    Return value : 1 or 0
*/
/*------------------------------------------------*/

int searchSYMTAB(char* label, int idx){

    if(readprefix(label)){
        label = label + 1;
    }

    for(int i = 0; i <= SYMTAB_counter[idx]; i++){
        if(!strcmp(symboltb[idx][i].label, label)){
           SYMidx = i;
           return 1;
        }
    }

    return 0;
}

/*------------------------------------------------*/
/*
    Function : searchREGTAB()
    Purpose : Search register from the register table.
    Return value : 1 or 0
*/
/*------------------------------------------------*/

int searchREGTAB(char *REG){
    int size = sizeof(REGTAB) / sizeof(Register);
    for(int i = 0; i < size; i++){
        if(!strcmp(REG, REGTAB[i].regname)){
            REGidx = i;
            return 1;
        }
    }
    return 0;
}

/*------------------------------------------------*/
/*
    Function : searchOPTAB()
    Purpose : Search mnemonic from the opcode table.
    Return value : 1 or 0
*/
/*------------------------------------------------*/

int searchOPTAB(char *MNMN){

    if(readprefix(MNMN)){
        MNMN = MNMN + 1;
    }
    
    for(int i = 0; i < HASHSIZE; i++){
        if(hashtable[i] != NULL){
            OPTAB* node = hashtable[i];
            while(node != NULL){
              if(!strcmp(node->name, MNMN)){
                  strcpy(optab.name, node->name);
                  strcpy(optab.code, node->code);
                  strcpy(optab.format, node->format);
                  return 1;
              }
              else node = node->next;
            }
        }
    }

    return 0;
}

/*------------------------------------------------*/
/*
    Function : insertSYMTAB()
    Purpose : Record symbol into symbol table.
    Return value : None
*/
/*------------------------------------------------*/

void insertSYMTAB(char* label){
    if(readprefix(label)){
        label = label + 1;
    }
    
    strcpy(symboltb[0][SYMTAB_counter[0]].label, label);
    symboltb[0][SYMTAB_counter[0]].address = LOCCTR[LOCCTR_counter - 1];
    SYMTAB_counter[0]++;

}

/*------------------------------------------------*/
/*
    Function : IsNum()
    Purpose : To determine whether string is number or not.
    Return value : 1 or 0
*/
/*------------------------------------------------*/

int IsNum(char *c){
    if(readprefix(c)) c += 1;

    for(int i = 0; i < strlen(c); i++){
        if('0' > c[i] || '9' < c[i]){
            if(c[i] == '-') continue;
            return 0;
        }
    }

    return 1;

}

/*------------------------------------------------*/
/*
    Function : IsFloatNum()
    Purpose : To determine whether string is float number or not.
    Return value : 1 or 0
*/
/*------------------------------------------------*/

int IsFloatNum(char *str){
    int f = 0;
    if(readprefix(str)) str += 1;

    for(int i = 0; i < strlen(str); i++){
        if('0' > str[i] || '9' < str[i]){
            if(str[i] == '.' && f == 0){
                f = 1;
                continue;
            }
            if(str[i] == '-') continue;
            return 0;
        }
    }
    return (f != 0) ? 1 : 0;

}

/*------------------------------------------------*/
/*
    Function : TwelveBit()
    Purpose : To do 2's complement when disp is negative.
    Return value : disp
*/
/*------------------------------------------------*/

unsigned long TwelveBit(int disp, int length){

    if(disp >= 0) return disp;   

    if(length == 5) disp ^= 0xFF00000;
    else disp ^= 0xFFFFF000;

    return disp;
}

/*------------------------------------------------*/
/*
    Function : StrToDec
    Purpose : To make string into decimal number
    Return value : dec_num
*/
/*------------------------------------------------*/

int StrToDec(char *c){
    if(readprefix(c)) c+= 1;

    int dec_num = 0;
    char temp[10];
    strcpy(temp, c);

    for(int i = strlen(c) - 1, l = 1; i >= 0; i--){
        if(temp[0] == '-') continue;
        dec_num = dec_num + (int)(temp[i] - '0')*l;
        l = l*10;
    }
    return (temp[0] == '-') ? (-dec_num) : (dec_num);

}

/*------------------------------------------------*/
/*
    Function : StrToFloat()
    Purpose : To make string into float number
    Return value : number
*/
/*------------------------------------------------*/

int StrToFloat(char *c){
    double number = 0;
    for(int i = strlen(c) - 1; i >= 0; i--){
        number /= 10.0;
        number += (c[i] - '0')/10.0;
    }

    return number;
}

/*------------------------------------------------*/
/*
    Function : StrToHex()
    Purpose : To make string into hexadecimal number
    Return value : hex
*/
/*------------------------------------------------*/

int StrToHex(char *c){
    int hex = 0;
    int i,j;
    char temp[10];
    strcpy(temp, c);

    for(i = strlen(temp) - 1, j = 1; i >= 0; i--){
        if(temp[i] >= '0' && temp[i] <= '9') hex = hex + (int)(temp[i] - '0')*j;
        else if(temp[i] >= 'A' && temp[i] <= 'F') hex = hex + (int)(temp[i] - 'A' + 10)*j;
        else if(temp[i] >= 'a' && temp[i] <= 'f') hex = hex + (int)(temp[i] - 'a' + 10)*j;
        j = j*16;

    }
    return (hex);


}

/*------------------------------------------------*/
/*
    Function : findlength()
    Purpose : To compute the length of the string
    Return value : b
*/
/*------------------------------------------------*/

int findlength(char *c){
    unsigned int b;
    char len[32];

    strcpy(len, c);
    if(len[0] == 'C' || len[0] == 'c' && len[1] == '\''){
        for(b = 2; b <= strlen(len); b++){
            if(len[b] == '\''){
                b -= 2;
                break;
            }
        }
    }
    if(len[0] == 'X' || len[0] == 'x' && len[1] == '\'') b = 1;
    return (b);

}

/*------------------------------------------------*/
/*
    Function : add_SYMTAB()
    Purpose : To add symbol into symbol table
    Return value : none
*/
/*------------------------------------------------*/

void add_SYMTAB(int key, HASH_SYMTAB* node){
    int index = key % HASHSIZE;
    HASH_SYMTAB* cur = (HASH_SYMTAB*)malloc(sizeof(HASH_SYMTAB));

    if(s_hashtable[index] == NULL) s_hashtable[index] = node;
    else{
        cur = s_hashtable[index];
        while(cur->next != NULL) cur = cur->next;
        cur->next = node;
    }


}

/*------------------------------------------------*/
/*
    Function : create_SYMTAB()
    Purpose : To create hash table of symbol table
    Return value : none
*/
/*------------------------------------------------*/

void create_SYMTAB(){

    int j;
    srand(time(NULL));

    for(j = 0; j < SYMTAB_counter[0]; j++){

        HASH_SYMTAB* node = (HASH_SYMTAB*)malloc(sizeof(HASH_SYMTAB));
        node->id = rand() % HASHSIZE;
        strcpy(node->label, symboltb[0][j].label);
        node->address = symboltb[0][j].address;
        node->next = NULL;
        add_SYMTAB(node->id, node);
        
    }

}

/*------------------------------------------------*/
/*
    Function : symbol()
    Purpose : Function command 'symbol' 
    Return value : none
*/
/*------------------------------------------------*/

void symbol(){

    int i, j,  k, t;
    char temp[32];
    char lab1[32], lab2[32];
    for(i = 0; i < SYMTAB_counter[0] -1; i++){
        for(j = 0; j < SYMTAB_counter[0] - (i+1); j++){ // bubble sort to rearrange in  descending order
            strcpy(lab1, symboltb[0][j].label);
            strcpy(lab2, symboltb[0][j+1].label);
            if(strcmp(lab1, lab2) < 0){
                strcpy(temp, symboltb[0][j+1].label);
                t = symboltb[0][j+1].address;
                strcpy(symboltb[0][j+1].label, symboltb[0][j].label);
                symboltb[0][j+1].address = symboltb[0][j].address;
                strcpy(symboltb[0][j].label, temp);
                symboltb[0][j].address = t;
            }
        }
    }

    for(j = 0; j < SYMTAB_counter[0]; j++){
        printf("\t%s\t%04X\n", symboltb[0][j].label, symboltb[0][j].address);
    }
    
}

/*------------------------------------------------*/
/*
    Function : createLST()
    Purpose : To create listing file
    Return value : none
*/
/*------------------------------------------------*/

void createLST(char *AA){
    int j, length;
    FILE *fp_lst, *f;
    char *T = NULL, copy[10];
    
    strcpy(copy, AA);
    T = strtok(AA, ".");
    strcat(T, ".lst");
    strcpy(filelst, T);
    fp_lst = fopen(filelst, "w");
    f = fopen(copy, "r");

    for(j = 0; j < list_index[0]; j++){
        length = 0;

        while(1){
          fgets(BUFF, 200, f);
          if(BUFF[0] == '.') fprintf(fp_lst, "%s", BUFF);
          else break;
        }

        if(strlen(list[0][j]->operand) <= 0 && !strcmp(list[0][j]->mnmn, "END")) continue;
        if(!strcmp(list[0][j]->mnmn, "BASE") || !strcmp(list[0][j]->mnmn, "END")){
            fprintf(fp_lst, "    \t%-10s%-10s%-10s\t", list[0][j]->label, list[0][j]->mnmn, list[0][j]->operand);
        }
        else fprintf(fp_lst, "%04X\t%-10s%-10s%-10s\t", list[0][j]->location, list[0][j]->label, list[0][j]->mnmn, list[0][j]->operand);

        if(!strcmp(list[0][j]->mnmn, "START") || !strcmp(list[0][j]->mnmn, "RESW") || !strcmp(list[0][j]->mnmn, "RESB") || !strcmp(list[0][j]->mnmn, "BASE") || !strcmp(list[0][j]->mnmn, "NOBASE") || !strcmp(list[0][j]->mnmn, "END")) {
            fprintf(fp_lst, " ");
        }

        else if(searchOPTAB(list[0][j]->mnmn)){
            if(!strcmp(optab.format, "3/4")){
                if(readprefix(list[0][j]->mnmn)) fprintf(fp_lst, "%08X", list[0][j]->OBJ);
                else fprintf(fp_lst, "%06X", list[0][j]->OBJ);

            }
            else if(!strcmp(optab.format, "2")) fprintf(fp_lst, "%04X", list[0][j]->OBJ);
            
            else if(!strcmp(optab.format,"1")) fprintf(fp_lst, "%02X", list[0][j]->OBJ);

        }
        else{
            if(IsFloatNum(list[0][j]->operand)) fprintf(fp_lst, "%012X", list[0][j]->OBJ);
            else{
                length = findlength(list[0][j]->operand);
                if(length == 1) fprintf(fp_lst, "%02X", list[0][j]->OBJ);
                else if(length == 2) fprintf(fp_lst, "%04X", list[0][j]->OBJ);
                else fprintf(fp_lst, "%06X", list[0][j]->OBJ);
            }
        }
        fprintf(fp_lst, "\n");

    }
    fclose(f);
    fclose(fp_lst);

}

/*------------------------------------------------*/
/*
    Function : createOBJ()
    Purpose : To create object file 
    Return value : none
*/
/*------------------------------------------------*/

void createOBJ(char *AA){
    
    int first_address, last_address, temp_address, temp_format;
    int first_index, last_index;
    int x, y;
    char *T = NULL;
    FILE *fp_obj;
    int j;
    
    char temp_operator[12][10];
    char temp_operand[12][10];
    unsigned int temp_obj[30];

    T = strtok(AA, ".");
    strcat(T, ".obj");
    strcpy(fileobj, T);
    fp_obj = fopen(T, "w");

        j = 0;
        if(!strcmp(list[0][j]->mnmn, "START")){
            fprintf(fp_obj, "H%-6s%06X%06X\n", list[0][j]->label, start_address[0], program_length[0]);
            j++;

        }

        while(1){
            first_address = list[0][j]->location;
            last_address = list[0][j]->location + 29;
            first_index = j;
            for(x = 0, temp_address = first_address; temp_address <= last_address; j++){
                if(!strcmp(list[0][j]->mnmn, "END")) break;
                else if(strcmp(list[0][j]->mnmn, "RESB") && strcmp(list[0][j]->mnmn, "RESW") && strcmp(list[0][j]->mnmn, "BASE")){
                    temp_obj[x] = list[0][j]->OBJ;
                    strcpy(temp_operator[x], list[0][j]->mnmn);
                    strcpy(temp_operand[x], list[0][j]->operand);
                    last_index = j + 1;
                    x++;
                }
                
                temp_address = list[0][j+1]->location;
                if(searchOPTAB(list[0][j+1]->mnmn)){
                    if(readprefix(list[0][j+1]->mnmn)) temp_address++;
                    if(!strcmp(optab.format, "1")) temp_format = 1;
                    if(!strcmp(optab.format, "2")) temp_format = 2;
                    if(!strcmp(optab.format, "3/4")) temp_format = 3;
                    temp_address += temp_format;
                }
                else{
                    if(!strcmp(list[0][j+1]->mnmn, "WORD") || !strcmp(list[0][j+1]->mnmn, "BYTE")){
                        if(IsFloatNum(list[0][j+1]->mnmn)) temp_address += 6;
                        else if(!strcmp(list[0][j+1]->mnmn, "BYTE")) temp_address += findlength(list[0][j+1]->mnmn);
                        else if(!strcmp(list[0][j+1]->mnmn, "WORD")) temp_address += 3;
                    }

                }

            }

            if((list[0][last_index]->location - list[0][first_index]->location) == 0){
                if(!strcmp(list[0][j]->mnmn, "END")) break;
                else continue;

            }
            fprintf(fp_obj, "T%06X%02X", first_address, (list[0][last_index]->location - list[0][first_index]->location));

            for(y = 0; y < x; y++){
                if(strcmp(temp_operator[y], "BYTE") == 0) {
                    if(temp_operand[y][0] == 'X') fprintf(fp_obj, "%02X", temp_obj[y]);
                    else if(IsFloatNum(temp_operand[y])) fprintf(fp_obj, "%012X", temp_obj[y]);
                    else fprintf(fp_obj, "%06X", temp_obj[y]);

                }
                else{
                    if(searchOPTAB(temp_operator[y])){
                        if(!strcmp(optab.format, "3/4")){
                            if(readprefix(temp_operator[y])) fprintf(fp_obj, "%08X", temp_obj[y]);
                            else fprintf(fp_obj, "%06X", temp_obj[y]);

                        }
                        else if(!strcmp(optab.format, "2")) fprintf(fp_obj, "%04X", temp_obj[y]);
                        else if(!strcmp(optab.format, "1")) fprintf(fp_obj, "%02X", temp_obj[y]);
                    }
                    else{
                        if(IsFloatNum(temp_operand[y])) fprintf(fp_obj, "%012X", temp_obj[y]);
                        else fprintf(fp_obj, "%06X", temp_obj[y]);

                    }
                }


            }
          fprintf(fp_obj, "\n");
          if(!strcmp(list[0][j]->mnmn, "END")) break;
        }
    // Modification

      for(j = 0; j < relocation_counter[0]; j++) fprintf(fp_obj, "M%06X%02X\n", Rlist[0][j].address, Rlist[0][j].length);

      fprintf(fp_obj, "E");
      if(searchSYMTAB(end_operand, 0)) fprintf(fp_obj, "%06X\n", symboltb[0][SYMidx].address);
      else fprintf(fp_obj, "\n");

    fclose(fp_obj);
}

/*------------------------------------------------*/
/*
    Function : relocation()
    Purpose : To record the relocation
    Return value : none
*/
/*------------------------------------------------*/

void relocation(char* mnmn, int location, int index){
    
    Rlist[index][relocation_counter[index]].address = location + 1;
    Rlist[index][relocation_counter[index]].length = 3;
    if(readprefix(mnmn)) Rlist[index][relocation_counter[index]].length += 2; // format 4

    relocation_counter[index]++;

}

/*------------------------------------------------*/
/*
    Function : assemble() 
    Purpose : To create listing file and object file from reading  asm file
    Return value : 1 or 0
*/
/*------------------------------------------------*/

int assemble(char *AA){

    FILE *fp4;
    char label[32], opcode[32], operand[32];
    int IsComment = 0, IsEmpty = 0, IsStart = 0, LINE_count = 0;
    int loc = 0, temp_format = 0;
    
    fp4 = fopen(AA, "r");
    if(fp4 == NULL) {
        printf("File open error!\n");
        return 0;
    }

    if(IsAssembled == 1){
        for(int i = 0; i < SYMTAB_counter[0]; i++){
            symboltb[0][i].label[0] = '\0';
            symboltb[0][i].address = 0;
        }
        SYMTAB_counter[0] = 0;
    }
    /* Initialize */
    INDX = 0;
    SYMidx = 0;
    for(int i = 0; i < 100; i ++){
        LOCCTR[i] = 0;
    }
    LOCCTR_counter = 0;
    for(int i = 0; i < 10; i++){
        SYMTAB_counter[i] = 0;
        start_address[i] = 0;
        program_length[i] = 0;
        list_index[i] = 0;
        relocation_counter[i] = 0;
    }

    // PASS 1 
    while(fgets(BUFF, 200, fp4) != NULL){
        list[0][list_index[0]] = (Intermediate*)malloc(sizeof(Intermediate));
        
        IsEmpty = strlen(BUFF);
        BUFF_length = strlen(BUFF);
        INDX = 0;
        strcpy(label, readline());

        if(label[0] == '.') IsComment = 1;
        else IsComment = 0;

        if(IsEmpty > 1 && IsComment == 0){ // this is not a comment line then
            INDX = 0;
            list[0][list_index[0]]->line = *list_index;
            strcpy(label, readline());
            strcpy(list[0][list_index[0]]->label, label);
            skip();

            if(LINE_count == IsStart){ // this is a first line
                strcpy(opcode, readmnmn());
                strcpy(list[0][list_index[0]]->mnmn, opcode);
                if(!strcmp(opcode, "START")){
                    skip();
                    strcpy(operand, readopnd());
                    strcpy(list[0][list_index[0]]->operand, operand);
                    LOCCTR[LOCCTR_counter] = StrToHex(operand);
                    start_address[0] = LOCCTR[LOCCTR_counter]; 
                }
                else{
                    LOCCTR[LOCCTR_counter] = 0;
                    start_address[0] = LOCCTR[LOCCTR_counter];
                }
            }
            else{ // not a first line
                strcpy(opcode, readmnmn());
                strcpy(list[0][list_index[0]]->mnmn, opcode);
                skip();
                strcpy(operand, readopnd());
    
                strcpy(list[0][list_index[0]]->operand, operand);

                if(strcmp(opcode, "END")){ // this is not END
                    if(label[0] != '\0'){
                        if(searchSYMTAB(label, 0)){ // duplicate symbol
                            fclose(fp4);
                            printf("Debugging Error on line %d\n", 5*(LINE_count));
                            SYMidx = 0;
                            return 0;
                        }
                        else{
                            insertSYMTAB(label);
                        } // insert (LABEL, LOCCTR) into SYMTAB
                    }

                    if(searchOPTAB(opcode)){
                        
                        if(strcmp(optab.format, "3/4") == 0) temp_format = 3;
                        if(strcmp(optab.format, "2") == 0) temp_format = 2;
                        if(strcmp(optab.format, "1") == 0) temp_format = 1;
                        LOCCTR[LOCCTR_counter] = loc + temp_format;
                        if(readprefix(opcode)) LOCCTR[LOCCTR_counter] += 1;
                    }
                    else if(!strcmp(opcode, "WORD")){
                        if(IsFloatNum(operand)) LOCCTR[LOCCTR_counter] = loc + 6;
                        else LOCCTR[LOCCTR_counter] = loc + 3;

                    }
                    else if(!strcmp(opcode, "RESW")){
                        LOCCTR[LOCCTR_counter] = loc + 3 * StrToDec(operand);

                    }
                    else if(!strcmp(opcode, "RESB")){
                        LOCCTR[LOCCTR_counter] = loc + StrToDec(operand);
                    }
                    else if(!strcmp(opcode, "BYTE")){
                        if(IsFloatNum(operand)) LOCCTR[LOCCTR_counter] = loc + 6;
                        else LOCCTR[LOCCTR_counter] = loc + findlength(operand);

                    }
                    else if(!strcmp(opcode, "BASE") || !strcmp(opcode, "NOBASE")) LOCCTR[LOCCTR_counter] = loc;
                    else{
                        fclose(fp4);
                        printf("Invalid Operation Code at line %d\n", 5*(LINE_count));
                        return 0;
                    }

                }
               else strcpy(end_operand, operand);     

            }
            loc = LOCCTR[LOCCTR_counter];
            list[0][list_index[0]]->location = LOCCTR[LOCCTR_counter - 1];
            LOCCTR_counter++;
            list_index[0]++;
        }
        if(IsComment == 1) {
            IsStart++;
        }
        
        LINE_count++;
    }
    program_length[0] = LOCCTR[LOCCTR_counter - 2] - LOCCTR[0];

    create_SYMTAB();
  
    // PASS 2
    int disp = 0;
    unsigned long obj, obj_opcode, obj_sign, obj_relative, obj_indexed, obj_address, obj_extended;
    int obj_format, reg_index, IsBase, k;
    unsigned short int temp;
    char reg_name[10];
    char *ch;

        IsBase = -1;
        disp = 0;
        for(int j = 1; j < list_index[0]; j++){
            obj_opcode = 0;
            obj_sign = 0;
            obj_relative = 0;
            obj_indexed = 0;
            obj_address = 0;
            obj_format = 0;
            obj_extended = 0;
            reg_name[0] = '\0';  
            strcpy(opcode, list[0][j]->mnmn);

            if(searchOPTAB(opcode)){
                if(!strcmp(optab.name, "RSUB")){
                    temp = strtoul(optab.code, &ch, 16);
                    list[0][j]->OBJ = ((temp+3) << 16);
                    continue;
                }
                obj_opcode = strtoul(optab.code, &ch, 16);

                if((strcmp(optab.format, "3/4") == 0)){
                    if(readprefix(opcode)){
                        obj_format = 4;
                        obj_extended = 0X00100000;
                    }
                    else obj_format = 3;
                }
                if(strcmp(optab.format, "2") == 0) obj_format = 2;
                if(strcmp(optab.format, "1") == 0) obj_format = 1;


                obj_opcode <<= (8* (obj_format - 1));
                list[0][j]->OBJ = obj_opcode;
                strcpy(operand, list[0][j]->operand);

                if(readprefix(operand)){
                    if(obj_format <= 2){
                        fclose(fp4);
                        printf("Invalid Addressing mode at line %d\n", 5*(list[0][j]->line));
                        return 0;
                    }
                    if(prefix == 2) obj_sign = 0X010000;
                    else if(prefix == 3) obj_sign = 0x020000;

                    obj_sign <<= 8 * (obj_format - 3);
                }
                else if(obj_format >= 3){
                    obj_sign = 0x030000;
                    obj_sign <<= 8 * (obj_format - 3);
                }

                if(obj_format >= 3){
                    if(operand[strlen(operand) - 2] == ',' && operand[strlen(operand) - 1] == 'X'){ // indexed addressing 
                        obj_indexed = 0x008000;
                        obj_indexed <<= 8 * (obj_format - 3);
                        operand[strlen(operand) - 2] = '\0';
                    }

                    if(searchSYMTAB(operand, 0)){
                        // search SYMTAB for operand
                        if(obj_format == 4){
                            obj_address = symboltb[0][SYMidx].address;
                            
                            relocation(list[0][j]->mnmn, list[0][j]->location, 0);
                        }
                        else{ // relative : base or pc
                            disp = symboltb[0][SYMidx].address - list[0][j]->location - obj_format;
                            if(disp >= -2048 && disp <= 2047){ // pc relatvie
                                obj_address = 0x002000;
                                obj_address += TwelveBit(disp, 3);

                            }
                            else{ // base relative
                                disp = symboltb[0][SYMidx].address - IsBase;
                                if(IsBase != -1 && disp >= 0 && disp <= 4095){
                                    obj_address = 0x004000;
                                    obj_address += disp;

                                }
                                else{// relatvie mode fail
                                    fclose(fp4);
                                    printf("Relative Mode Failed at line %d\n", 5*(list[0][j]->line));
                                    return 0;
                                }
                            }
                        }
                    }
                    else{ // symbol not found
                        readprefix(operand);
                        if(prefix == 2 && IsNum(operand)) obj_address = TwelveBit(StrToDec(operand), (obj_format == 4) ? 5 : 3);
                        else {
                            printf("Label doesn't exist at line %d\n", 5*(list[0][j]->line));
                            fclose(fp4);
                            return 0;
                        }
                    }
                }
                else if(obj_format == 2){ // format 2
                    k = 0;
                    reg_index = 0;
                    do{
                        if(operand[k] == ',' || operand[k] =='\0'){
                            reg_name[reg_index] = '\0';

                            if(operand[k] == ',') obj_address <<= 4;
                            
                            if(searchREGTAB(reg_name)) obj_address += REGTAB[REGidx].number;
                            else{
                                if(!strcmp(optab.name, "SVC") || !strcmp(optab.name, "SHIFTL") || !strcmp(optab.name, "SHIFTR")){
                                    if(IsNum(reg_name)) obj_address += StrToDec(reg_name);  
                                
                                }
                                else{
                                    fclose(fp4);
                                    printf("Invalid Register at line %d\n", 5*(list[0][j]->line));
                                    return 0;
                                }     
                            }
                            reg_index = 0;
                        }
                        else{
                          reg_name[reg_index++] = operand[k];
                        }
                    } while (operand[k++] != '\0');

                    if(!strcmp(optab.name, "CLEAR") || !strcmp(optab.name, "TIXR") || !strcmp(optab.name, "SVC")) obj_address <<= 4;

                }

                //Record of Object Code
                obj = obj_opcode + obj_sign + obj_indexed + obj_relative + obj_extended + obj_address;
                list[0][j]->OBJ = obj; 

            }
            else if(!strcmp(opcode, "WORD")){ // convert constant to object code
                strcpy(operand, list[0][j]->operand);
                list[0][j]->OBJ = StrToHex(operand);

            }
            else if(!strcmp(opcode, "BYTE")){
                strcpy(operand, list[0][j]->operand);
                list[0][j]->OBJ = 0;
                  if(operand[0] == 'C' || operand[0] == 'c' && operand[1] == '\''){
                      for(int z = 2; z <= (int)(strlen(operand) - 2); z++){
                          list[0][j]->OBJ += (int)operand[z];
                          list[0][j]->OBJ <<= 8;
                      }
                  }

                  if(operand[0] == 'X' || operand[0] == 'x' && operand[1] == '\''){
                      char *op_ptr;
                      op_ptr = &operand[2];
                      *(op_ptr + 2) = '\0';
                      for(int z = 2; z <= (int)(strlen(operand) - 2); z++){
                          list[0][j]->OBJ += StrToHex(op_ptr);
                          list[0][j]->OBJ <<= 8;
                      }

                  }
                  list[0][j]->OBJ >>= 8;
            }

            else if (!strcmp(opcode, "BASE")){
                strcpy(operand, list[0][j]->operand);
                list[0][j]->OBJ = 0;
                if(searchSYMTAB(operand, 0)) IsBase = symboltb[0][SYMidx].address;
                else{
                    fclose(fp4);
                    printf("No Label is SYMTAB at line %d\n", 5*(list[0][j]->line));
                    return 0;
                }


            }
            else if (!strcmp(opcode, "NOBASE")){
                list[0][j]->OBJ = 0;
                IsBase = -1;
            }

        }

    createLST(AA);
    createOBJ(AA);

    for(int j = 0; j < list_index[0]; j++){
           free(list[0][j]);
    }

    fclose(fp4);

    printf("\noutput file: [%s], [%s]\n\n", filelst, fileobj);
    
    return 1;
}

/*------------------------------------------------*/
/*    
      Function : loader()
      Purposse : Function 'loader' command.
      Return value : 0 or 1.
*/
/*------------------------------------------------*/

int loader(unsigned char * BUFFER, int file_count){
    FILE *fp;
    char name[10], symbol[10], buffer[200], first[2]; 
    char temp_address[10], temp_length[10];
    int es_count = 0;
    unsigned int add, len;
    char *cha;
    int i = 0, index = 0, j=0, flag = 0;
  
    // PASS 1
    CSLTH = 0;
    CSADDR = PROGADDR;
    
    for(i = 0; i < file_count; i++){

        fp = fopen(ARR_LOAD[i+1], "r");
        buffer[0] = '\0';
        
        if(fp == NULL){
            printf("Error : There is no such file.\n");
            return 0;
        }

        while(fgets(buffer, 200, fp) != NULL){
            index = 0;
            j = 0;
            name[0] = '\0';
            symbol[0] = '\0';
            temp_address[0] = '\0';
            temp_length[0] = '\0';
            first[0] = buffer[0];
            first[1] = '\0';

            if(strcmp("H", first) == 0){
                index++;
                j = 0;

                while(buffer[index] != ' ' && buffer[index] != '\t') name[j++] = buffer[index++];
                name[j] = '\0';
                strcpy(estab[es_count].csect, name);
                strcpy(estab[es_count].symbol, " ");
                while(buffer[index] == ' ' || buffer[index] == '\t') index++;
                for(int x = 0; x < 6; x++){
                    temp_address[x] = buffer[index];
                    index++;
                    temp_address[x+1] = '\0';
                }
                add = strtoul(temp_address, &cha, 16);
                estab[es_count].address = add + CSADDR;
                for(int x = 0; x < 6; x++){
                    temp_length[x] = buffer[index];
                    index++;
                    temp_length[x+1] = '\0';
                }
                len = strtoul(temp_length, &cha, 16);
                estab[es_count].length = len;
                es_count++;
            }
            else if(strcmp("D",first) == 0){
                index++;
                while(buffer[index] != '\n'){
                    symbol[0] = '\0';
                    temp_address[0] = '\0';
                    j=0;
        
                  while(buffer[index] != ' ' && buffer[index] != '\t') symbol[j++] = buffer[index++];
                  symbol[j] = '\0';
                  strcpy(estab[es_count].csect, " ");
                  strcpy(estab[es_count].symbol, symbol);
                  while(buffer[index] == ' ' || buffer[index] == '\t') index++;
                  for(int x = 0; x < 6; x++){
                      temp_address[x] = buffer[index];
                      index++;
                      temp_address[x+1] = '\0';
                  }
                  add = strtoul(temp_address, &cha, 16);
                  estab[es_count].address = add + CSADDR;
                  estab[es_count].length = 0;
                  es_count++;
                }
                CSADDR = CSADDR + len;
            }
        }
        fclose(fp);

    }
    

    // PASS 2
    int loc, lent, l =0, k, specified_add;
    char temp_refer[10],  IsExtended[3], dummy_memory[3], mod_memory[10], fin_memory[10];
    unsigned int dummy;
    int temp_memory;
     
    EXECADDR = PROGADDR;
    CSADDR = PROGADDR; 

    for(i = 0; i < file_count; i++){
        fp = fopen(ARR_LOAD[i+1], "r");
        for(int x = 0; x < es_count; x++) estab[x].refer[0] = '\0';
         // initiallize
        while(fgets(buffer, 200, fp) != NULL){
            index = 0;
            j = 0;
            name[0] = '\0';
            symbol[0] = '\0';
            temp_address[0] = '\0';
            temp_length[0] = '\0';
            first[0] = buffer[0];
            first[1] = '\0';
            if(strcmp(first, "H") == 0){
                index++;
                while(buffer[index] != ' ' && buffer[index] != '\t') name[j++] = buffer[index++];
                name[j] = '\0';
                for(int x = 0; x < es_count; x++){
                    if(strcmp(name, estab[x].csect) == 0){
                        loc = estab[x].address;
                        strcpy(estab[x].refer, "01");
                        break;
                    }
                }

                EXECADDR = loc;
            }

            else if(strcmp(first, "T") == 0){
                
                index++;
                for(int x = 0; x < 6; x++){
                    temp_address[x] = buffer[index];
                    index++;
                    temp_address[x+1] = '\0';
                }
                
                for(int x = 0; x < 2; x++){
                    temp_length[x] = buffer[index];
                    index++;
                    temp_length[x+1] = '\0';
                }
                dummy = strtoul(temp_address, &cha, 16);
                loc = EXECADDR + dummy;
                dummy = strtoul(temp_length, &cha, 16);
                lent = dummy;

                for(k = 0; k < (lent)*2; k += 2){
                    for(int x = 0; x < 2; x++){
                        dummy_memory[x] = buffer[index];
                        index++;
                        dummy_memory[x+1] = '\0';
                    }
                    sscanf(dummy_memory, "%02X", &temp_memory);
                    BUFFER[(unsigned int)loc] = (unsigned char)(temp_memory);
                    loc = loc + 1;
                }
            }

            else if(strcmp(first, "R") == 0){
                index++;
                while(buffer[index] != '\n'){
                    j = 0;
                    for(int x = 0; x < 2; x++){
                      temp_refer[j++] = buffer[index++];
                      temp_refer[j+1] = '\0';
                    }
                    j = 0;
                  
                    while(buffer[index] != ' ' && buffer[index] != '\t' && buffer[index] != '\n'){
                        symbol[j++] = buffer[index++];
                        symbol[j+1] = '\0';
                    }
                    while(buffer[index] == ' ' || buffer[index] == '\t') index++;
      
                    for(int x = 0; x < es_count; x++){
                        if(strcmp(symbol, estab[x].symbol) == 0){
                            strcpy(estab[x].refer, temp_refer);
                            flag = 1;
                            break;
                        }
                    }
                    if(flag == 0){
                        printf("Error : There is no corresponding symbol name.\n");
                        return 0;
                    }
                    flag = 0;
                }

            }

            else if(strcmp(first, "M") == 0){
                index++;
                for(int x = 0; x < 6; x++){
                    temp_address[x] = buffer[index];
                    index++;
                    temp_address[x+1] = '\0';
                }
                IsExtended[0] = buffer[8];
                IsExtended[1] = '\0';
                if(strcmp(IsExtended, "5") == 0) lent = 8;  
                else lent = 6; // Format determination

                index += 2;
                dummy = strtoul(temp_address, &cha, 16);
                loc = EXECADDR + dummy;

                if(buffer[index] == '\n'){ // if there's no link 
                    sprintf(mod_memory, "%02X%02X%02X%02X", BUFFER[(unsigned int)(loc-1)], BUFFER[(unsigned int)(loc)], BUFFER[(unsigned int)(loc+1)], BUFFER[(unsigned int)(loc+2)]);
                    dummy = strtoul(mod_memory, &cha, 16);
                    k = dummy + PROGADDR;
                    sprintf(fin_memory, "%08X", k);
                    if(strlen(fin_memory) >= 9){
                        for(int x = 0; x < strlen(fin_memory); x++){
                            fin_memory[x] = fin_memory[x+strlen(fin_memory) - 8];
                        }
                        fin_memory[8] = '\0';
                    }

                    j = 0;
                    for(k = 0; k < strlen(fin_memory); k+=2){
                        for(int x = 0; x < 2; x++){
                            dummy_memory[x] = fin_memory[j];
                            j++;
                            dummy_memory[x+1] = '\0';
                        }
                        sscanf(dummy_memory, "%02X", &temp_memory);
                        BUFFER[(unsigned int)(loc-1)] = (unsigned char)(temp_memory);
                    }   
                }

                else if(buffer[index] != '\n'){ // If there is link
                    while(buffer[index] != '\n') name[j++] = buffer[index++];
                    name[j] = '\0';

                    for(int x = 0; x < strlen(name); x++){
                        symbol[x] = name[x+1];
                        symbol[x+1] = '\0';
                    } // ex) name is +LISTC , symbol is LISTC or +02 and 02

                    for(int x = 0; x < es_count; x++){
                        if((strcmp(symbol, estab[x].symbol) == 0) || (strcmp(symbol, estab[x].csect) == 0) || (strcmp(symbol, estab[x].refer) == 0)){
                            specified_add = estab[x].address;
                            flag = 1;
                            break;
                        }
                    }
                    if(flag == 0){
                        printf("Error : Undefined external symbol.\n");
                        return 0;
                    }
                    flag = 0;
                    mod_memory[0] = '\0';

                    if(name[0] == '+'){
 
                        if(lent == 6){
                            sprintf(mod_memory, "%02X%02X%02X", BUFFER[(unsigned int)loc], BUFFER[(unsigned int)(loc+1)], BUFFER[(unsigned int)(loc+2)]);
                            dummy = strtoul(mod_memory, &cha, 16);
                            k = dummy + specified_add;

                            sprintf(fin_memory, "%06X", k);

                            if(strlen(fin_memory) >= 7){
                                    for(int x = 0; x < strlen(fin_memory); x++){
                                      
                                        fin_memory[x] = fin_memory[x+strlen(fin_memory)-6];
                                    }
                                fin_memory[6] = '\0';
                            }

                        }
                        else{
                            sprintf(mod_memory, "%02X%02X%02X%02X", BUFFER[(unsigned int)(loc-1)], BUFFER[(unsigned int)(loc)], BUFFER[(unsigned int)(loc+1)], BUFFER[(unsigned int)(loc+2)]);
                            dummy = strtoul(mod_memory, &cha, 16);
                            k = dummy + specified_add;
                            sprintf(fin_memory, "%08X", k);
                            if(strlen(fin_memory) >= 9){
                              
                                  for(int x = 0; x < strlen(fin_memory); x++){
                                        fin_memory[x] = fin_memory[x+strlen(fin_memory) - 8];
                                  }
                            
                                fin_memory[8] = '\0';
                            }
                            
                        }

                        j = 0;
                        for(k = 0; k < strlen(fin_memory); k+=2){
                            for(int x = 0; x < 2; x++){
                                dummy_memory[x] = fin_memory[j];
                                j++;
                                dummy_memory[x+1] = '\0';
                            }
                            sscanf(dummy_memory, "%02X", &temp_memory);
                            if(strlen(fin_memory) != 6) BUFFER[(unsigned int)(loc-1)] = (unsigned char)(temp_memory);
                            else BUFFER[(unsigned int)(loc)] = (unsigned char)(temp_memory);
                            loc = loc + 1;
                        }                     
                    }

                    else if(name[0] == '-'){

                        if(lent == 6){
                            sprintf(mod_memory, "%02X%02X%02X", BUFFER[(unsigned int)(loc)], BUFFER[(unsigned int)(loc+1)], BUFFER[(unsigned int)(loc+2)]);
                            dummy = strtoul(mod_memory, &cha, 16);
                            k = dummy - specified_add;
                            sprintf(fin_memory, "%06X", k);
                            if(strlen(fin_memory) >= 7){
                                
                                    for(int x = 0; x < strlen(fin_memory); x++){
                                        fin_memory[x] = fin_memory[x+strlen(fin_memory) - 6];
                                    }
                                fin_memory[6] = '\0';
                            }
                        }
                        else{
                            sprintf(mod_memory, "%02X%02X%02X%02X", BUFFER[(unsigned int)(loc-1)], BUFFER[(unsigned int)(loc)], BUFFER[(unsigned int)(loc+1)], BUFFER[(unsigned int)(loc+2)]);
                            dummy = strtoul(mod_memory, &cha, 16);
                            k = dummy - specified_add;
                            sprintf(fin_memory, "%08X", k);
                            if(strlen(fin_memory) >= 9){
                                    for(int x = 0; x < strlen(fin_memory); x++){
                                        fin_memory[x] = fin_memory[x+strlen(fin_memory)-8];
                                    }
                                fin_memory[8] = '\0';
                            }
                        }

                        j = 0;
                        for(k = 0; k < strlen(fin_memory); k+=2){
                            for(int x = 0; x < 2; x++){
                                dummy_memory[x] = fin_memory[j];
                                j++;
                                dummy_memory[x+1] = '\0';
                            }
                            sscanf(dummy_memory, "%02X", &temp_memory);
                            if(strlen(fin_memory) != 6) BUFFER[(unsigned int)(loc-1)] = (unsigned char)(temp_memory);
                            else BUFFER[(unsigned int)(loc)] = (unsigned char)(temp_memory);
                            loc = loc + 1;
                        }
                        
                    }
                      

                }
            }


        }
            fclose(fp);

    }


  
    printf("\ncontrol     symbol      address     length\n");
    printf("section     name\n");
    printf("---------------------------------------------\n");
    for(i = 0; i < es_count; i++){
        if(estab[i].length != 0){
            printf("%-12s%-12s%04X        %04X\n", estab[i].csect, estab[i].symbol, estab[i].address, estab[i].length);
        }
        else{
            printf("%-12s%-12s%04X\n", estab[i].csect, estab[i].symbol, estab[i].address);
        }

        CSLTH += estab[i].length;
    }
    printf("---------------------------------------------\n");
    printf("                    total length    %04X\n\n", CSLTH);


    return 1;

}

/*------------------------------------------------*/
/*    
      Function : run()
      Purposse : Function 'run' command.
      Return value : 1 or 0
*/
/*------------------------------------------------*/

int run(unsigned char* BUFFER){
    
    int n_flag, i_flag, x_flag, b_flag, p_flag, e_flag, reg1, reg2;
    int format = 0, current =0, opcode = 0;
    unsigned int loc = 0;
    char temp_memory[10];
    int obj_full, disp;
    char *ch;
    char dummy[2];

    int i, j, k;
  

    REG[2] = PROGADDR + CSLTH;
    loc = PROGADDR;
    REG[9] = 0;

    while(REG[8] != PROGADDR + CSLTH){
            // opocde part!

    //    if(BRKPN[current_bp] != '\0' && BRKPN[current_bp] == loc) break;
        
            current = BUFFER[loc];
            opcode = current & 252;
            for(i = 0; i < sizeof(RUNTAB); i++){
                if(opcode == RUNTAB[i].code) format = RUNTAB[i].format;
            } // format checking 
            if(format != 2){
                loc++;
                current = BUFFER[loc];
                e_flag = current & 0x001000;
                if(e_flag) format == 4; // format 4 checking
            }

            switch(format){
              case 2 :
                loc++;
                sprintf(temp_memory, "%02X",  BUFFER[loc]);
                dummy[0] = temp_memory[0];
                dummy[1] = '\0';
                sscanf(dummy, "%d", &reg1);
                dummy[0] = temp_memory[1];
                dummy[1] = '\0';
                sscanf(dummy, "%d", &reg2);
                
                loc++;

                break;

              case 3:
                sprintf(temp_memory, "%02X%02X%02X", BUFFER[loc-1], BUFFER[loc], BUFFER[loc+1]);
                obj_full  = strtoul(temp_memory, &ch, 16);
                sprintf(temp_memory, "%01X%02X", BUFFER[loc], BUFFER[loc+1]);
                for(int x =0; x < 4; x++){
                    temp_memory[x] = temp_memory[x+1];
                }
                temp_memory[4] = '\0';
                disp = strtoul(temp_memory, &ch, 16);

                n_flag = obj_full & 0x020000;
                i_flag = obj_full & 0x010000;
                x_flag = obj_full & 0x008000;
                b_flag = obj_full & 0x004000;
                p_flag = obj_full & 0x002000; // flag checking
                
                loc += 2;
                
                break;
              case 4:
                sprintf(temp_memory, "%02X%02X%02X%02X", BUFFER[loc-1], BUFFER[loc], BUFFER[loc+1], BUFFER[loc+2]);
                obj_full  = strtoul(temp_memory, &ch, 16);
                sprintf(temp_memory, "%02X%02X%02X", BUFFER[loc], BUFFER[loc+1], BUFFER[loc+2]);
                for(int x = 0; x < 6; x++) temp_memory[x] = temp_memory[x+1];
                temp_memory[6] = '\0';
                disp = strtoul(temp_memory, &ch, 16);

                n_flag = obj_full & 0x02000000;
                i_flag = obj_full & 0x01000000;
                x_flag = obj_full & 0x00800000;
                b_flag = obj_full & 0x00400000;
                p_flag = obj_full & 0x00200000;

                loc += 4;
                break;
            }

            REG[8] = loc;

            if(opcode == 0x00){ // LDA
                if(i_flag) REG[0] = BUFFER[disp];
                else REG[0] = BUFFER[BUFFER[disp]];

            }

            if(opcode == 0x68){ // LDB
                if(i_flag) REG[3] = BUFFER[disp];
                else REG[3] = BUFFER[BUFFER[loc]];

            }
            if(opcode == 0x74){ // LDT
                if(i_flag) REG[5] = BUFFER[disp];
                else REG[5] = BUFFER[BUFFER[loc]];


            }
            if(opcode == 0x0C){ // STA
                disp = REG[0];

            }

            if(opcode == 0x14){ // STL
                disp = REG[2];
            }

            if(opcode == 0x10){ // STX
                disp = REG[1];
            }

            if(opcode == 0x3C){ // J
                REG[8] = BUFFER[disp];
            }

            if(opcode == 0x48){ // JSUB
                REG[2] = REG[8];
                REG[8] = BUFFER[disp];
            }

            if(opcode == 0x30){ // JEQ
                if(REG[9] ==0) REG[8] = BUFFER[disp];
            }

            if(opcode == 0x38){ // JLT
                if(REG[9] < 0) REG[8] = BUFFER[disp];

            }
            
            if(opcode == 0x28){ // COMP
                if(REG[reg1] > BUFFER[disp]) REG[9] = -REG[9];
                if(REG[reg1] < BUFFER[disp]) REG[9] = -REG[9];
                if(REG[reg1] == BUFFER[disp]) REG[9] = 0;

            }

            if(opcode == 0xB4) REG[reg1] = 0; //CLEAR
            

            if(opcode == 0xA0){ // COMPR
                if(REG[reg1] > REG[reg2]) REG[9] = -REG[9];
                if(REG[reg1] < REG[reg2]) REG[9] = -REG[9];
                if(REG[reg1] == REG[reg2]) REG[9] = 0;
            }

            if(opcode == 0x50){ // LDCH
                REG[0] <<= 8 * (BUFFER[disp]-1);
            }
            if(opcode == 0x54){ // STCH
                disp <<= 8 * (REG[0] -1);

            }
            if(opcode == 0xB8){ // TIXR
                REG[1] = REG[1] + 1;
                if(REG[1] < REG[reg1]) REG[9] = -REG[9];
                else REG[9] = REG[9];
            }

            if(opcode == 0x4C){ // RSUB
                REG[8] = REG[2];

            }

            if(opcode == 0xE0){ // TD
                REG[9] = -1;
            }

            if(opcode == 0xD8){ // RD
                REG[0] <<= 8 * (0x00);
            }

            if(opcode == 0xDC){ // WD

            }

    }


    REG[8] = PROGADDR + CSLTH;

    printf("A : %06X X : %06X\nL : %06X PC: %06X\nB : %06X S : %06X\nT : %06X\nEnd Program\n\n", REG[0], REG[1], REG[2], REG[8], REG[3], REG[4], REG[5]);
    if(BRKPN[current_bp] != '\0'){
        printf("Stop at checkpoint[%d]\n\n", BRKPN[current_bp]);
        current_bp++;
    }

    return 1;
}


/* Program Start */

int main(){

	char input[MAX_INPUT_SIZE];
    char *TEMP_IN = NULL, *TOK1 = NULL, *TOKEN=NULL, *ORG_IN=NULL, *TOK2 = NULL, *TOK3 = NULL, *ORG = NULL;
    char *AA, *BB, **ARR_FILL, *INPUT_MN, *temp, *O, *ch;
    int IsCommanded=0, IsFirst=0, IsHistory=0, IsOneNum=0;
    int LAST_ADDRESS = 0, LIMIT_ADDRESS=0, EDIT_ADDRESS = 0, EDIT_MEMORY = 0, FILL_START =0, FILL_END =0, FILL_MEMORY = 0;
    int z;

    DIR *dp;
    struct dirent *file;
    struct stat buf;
    unsigned char* BUFFER = (unsigned char*)calloc(MEGA_BYTE, sizeof(unsigned char));
    dp = opendir(".");

	struct NODE *head = malloc(sizeof(struct NODE));
    head->link = NULL;
    NODE *curr;

    create_hash();

	while(1){

        if(IsCommanded == 1){
            if(IsFirst == 0){
              InsertNode(head, input);
              curr = head->link;
              IsFirst = 1;
              IsCommanded = 0;
            }
            else{
                if(IsHistory == 0){
                  InsertNode(curr,input);
                  curr = curr->link;
                  curr->link = NULL;
                }
                IsCommanded = 0;
            }
        }
        IsHistory = 0;
        input[0] = '\0';

        printf("sicsim> ");
        scanf("%[^\n]%*c", input);
        input[strlen(input)] = '\0';

        if(input[0] == 0){
            getchar();
            printf("You should input something. Please input again.\n");
            continue;
        }

        AA = (char*)malloc(sizeof(char)*MAX_ARR_SIZE);
        BB = (char*)malloc(sizeof(char)*MAX_ARR_SIZE);
        ARR_FILL = (char **)malloc(sizeof(char*)*MAX_ARR_SIZE);
        for(int i = 0; i < MAX_ARR_SIZE; i++){
           ARR_FILL[i] = (char*)malloc(sizeof(char) * strlen(input));
        }


        INPUT_MN = (char*)malloc(sizeof(char)*MAX_ARR_SIZE);

        ORG_IN = (char*)malloc(sizeof(char)*strlen(input));
        TEMP_IN = (char*)malloc(sizeof(char)*strlen(input));
        ORG = (char*)malloc(sizeof(char)*strlen(input));
        O = (char*)malloc(sizeof(char)*MAX_ARR_SIZE);
        temp = (char*)malloc(sizeof(char)*MAX_ARR_SIZE);
  
        strcpy(TEMP_IN, input);
        strcpy(ORG_IN, input);
        strcpy(ORG, input);
        strcpy(O, input);
        TOKEN = strtok(TEMP_IN, " \t");
      

		if(strcmp(TOKEN, "q") == 0 || strcmp(TOKEN, "quit") == 0){
            FreeNode(head);
            FreeHash();
            free(BUFFER);
            free(AA);
            free(BB);
            free(TEMP_IN);
            free(ORG_IN);
            free(INPUT_MN);
            free(O);
            free(ORG);
            if(IsAssembled == 1){
                FreeSHash();
            }
			break;
		}
		if(strcmp(TOKEN, "h") == 0 || strcmp(TOKEN, "help") == 0){
            IsCommanded = 1;
			help();
		}
        if(strcmp(TOKEN, "d") == 0 || strcmp(TOKEN, "dir") == 0){
            IsCommanded = 1;
            dir(dp,file,buf);
        }
        if(strcmp(TOKEN, "hi") == 0 || strcmp(TOKEN, "history") == 0){
            if(IsFirst == 0){
              InsertNode(head, input);
              curr = head->link;
              IsFirst = 1;
              IsCommanded = 1;
              IsHistory = 1;
              history(head->link);
            }
            else{
             InsertNode(curr, input);
             curr = curr->link;
             curr->link = NULL;
             history(head->link);
             IsHistory = 1;
             IsCommanded = 1;
            }
        }
  
        if(strcmp(TOKEN, "du") == 0 || strcmp(TOKEN, "dump") == 0){
            IsOneNum = 0;

            if(LAST_ADDRESS > 0xfffff) LAST_ADDRESS = 0;

            TOK3 = strtok(ORG, " \t");
            while(TOK3 != 0){
                strcpy(temp, TOK3);
                TOK3 = strtok(NULL, " \t");
            }

            if(strcmp(temp, "dump") == 0 || strcmp(temp, "du")==0 ){
                dump(BUFFER, &LAST_ADDRESS, &LIMIT_ADDRESS);
                LAST_ADDRESS++;
                IsCommanded = 1;
            } // If it is just "dump " command.
            else{
                TOK1 = strtok(ORG_IN, ",");

                while(TOK1 != 0){
                    strcpy(BB, TOK1);
                    TOK1 = strtok(NULL,",");
                }

                if((strstr(BB, "dump") != NULL) || (strstr(BB, "du") != NULL)) IsOneNum = 1;

                if(IsOneNum == 1){

                  TOK2 = strtok(BB, " \t");
                  while (TOK2 != 0){
                      strcpy(AA, TOK2);
                      TOK2 = strtok(NULL, " \t");
                  }
                  sscanf(AA, "%x", &LAST_ADDRESS);
                  if(LAST_ADDRESS > 0xfffff || LAST_ADDRESS < 0x00000){
                      printf("Invalid Input!\n");
                      continue;
                  }

                  dump(BUFFER, &LAST_ADDRESS, &LIMIT_ADDRESS);
                  LAST_ADDRESS++;
                  IsCommanded = 1;
                } // If it is ' dump AA ' command.
                else{

                    TOK2 = strtok(ORG_IN, " \t");
                    while(TOK2 != 0){
                        strcpy(AA, TOK2);
                        TOK2 = strtok(NULL, " \t");
                    }

                    sscanf(AA, "%x", &LAST_ADDRESS);
                    sscanf(BB, "%x", &LIMIT_ADDRESS);

                    if((LAST_ADDRESS > 0xfffff ) || (LAST_ADDRESS < 0x00000 ) || (LIMIT_ADDRESS > 0xfffff) || (LIMIT_ADDRESS < 0x00000) || (LAST_ADDRESS >= LIMIT_ADDRESS)){
                        //Range Error
                        printf("Invalid Input!\n");
                        continue;
                    }

                    dump(BUFFER, &LAST_ADDRESS, &LIMIT_ADDRESS);

                    LAST_ADDRESS++;
                    IsCommanded = 1;
                    LIMIT_ADDRESS = 0;


                } // If it is ' dump AA , BB ' command.

            }
        }

        if(strcmp(TOKEN, "edit") == 0 || strcmp(TOKEN, "e") == 0){


            if(strcmp(input, "edit") == 0 || strcmp(input, "e") == 0){
                IsCommanded = 0;
                printf("Invalid Input!\n");
                continue;
            }
            else{
                TOK1 = strtok(ORG_IN, ",");
                while(TOK1 != 0){
                    strcpy(BB, TOK1);
                    TOK1 = strtok(NULL, ",");
                }

                TOK2 = strtok(ORG_IN, " \t");
                while(TOK2 != 0){
                    strcpy(AA, TOK2);
                    TOK2 = strtok(NULL, " \t");
                }

                sscanf(AA, "%x", &EDIT_ADDRESS);
                sscanf(BB, "%x", &EDIT_MEMORY);
                if((EDIT_ADDRESS > 0xfffff) || (EDIT_ADDRESS < 0x00000) ||  (EDIT_MEMORY < 0x00) || (EDIT_MEMORY > 0xff)){
                    printf("Invalid Input!\n");
                    continue;
                }
                edit(BUFFER, &EDIT_ADDRESS, &EDIT_MEMORY);
                IsCommanded = 1;
            }
        }

        if(strcmp(TOKEN, "f") == 0 || strcmp(TOKEN, "fill") == 0){
            int j = 0;

            TOK1 = strtok(ORG_IN, ",");
            while(TOK1 != NULL){
                strcpy(ARR_FILL[j], TOK1);
                TOK1 = strtok(NULL, ",");
                j++;
            }

            TOK2 = strtok(ARR_FILL[0], " \t");
            while(TOK2 != 0){
                strcpy(AA, TOK2);
                TOK2 = strtok(NULL, " \t");
            }

            sscanf(AA, "%x", &FILL_START);
            sscanf(ARR_FILL[1], "%x", &FILL_END);
            sscanf(ARR_FILL[2], "%x", &FILL_MEMORY);

            if((FILL_START >= FILL_END) || (FILL_START > 0xfffff) || (FILL_START < 0x00000) || (FILL_END > 0xfffff) || (FILL_END < 0x00000) || (FILL_MEMORY < 0x00) || (FILL_MEMORY > 0xff)){
                printf("Invalid Input!\n");
                continue;
            }

            fill(BUFFER, &FILL_START, &FILL_END, &FILL_MEMORY);
            IsCommanded = 1;

        }

        if(strcmp(TOKEN, "reset") == 0){
            
            BUFFER = (unsigned char*)calloc(MEGA_BYTE, sizeof(unsigned char));
            IsCommanded = 1;
        }

        if(strcmp(TOKEN, "opcodelist") == 0){
            opcodelist();
            IsCommanded = 1;

        }
        
        if(strcmp(TOKEN, "opcode") == 0){

            TOK1 = strtok(ORG_IN, " \t");
            while(TOK1 != 0){
                strcpy(INPUT_MN, TOK1);
                TOK1 = strtok(NULL, " \t");
            }

            IsCommanded = find_opcode(INPUT_MN);

        }

        if(strcmp(TOKEN, "type") == 0){
            
            TOK1 = strtok(ORG_IN, " \t");
            while(TOK1 != 0){
                strcpy(AA, TOK1);
                TOK1 = strtok(NULL, " \t");
            }

            IsCommanded = type(dp, file, buf, AA);
        
        }

        if(strcmp(TOKEN, "assemble") == 0){

            TOK1 = strtok(ORG_IN, " \t");
            while(TOK1 != 0){
                strcpy(AA, TOK1);
                TOK1 = strtok(NULL, " \t");
            }

            IsCommanded = assemble(AA);
            IsAssembled = IsCommanded;

        }
        
        if(strcmp(TOKEN, "symbol") == 0){
            if(IsAssembled == 1){
                symbol();
                IsCommanded = 1;
            }
            else{
                printf("There is no symbol table.\n");
                IsCommanded = 0;
            }
        }

        if(strcmp(TOKEN, "progaddr") == 0){
            char *ch;

            TOK1 = strtok(ORG_IN, " \t");
            while(TOK1 != 0){
                strcpy(AA, TOK1);
                TOK1 = strtok(NULL, " \t");
            }
            PROGADDR = strtoul(AA, &ch, 16);

            IsCommanded = 1;
            printf("\nProgram starting address set to 0x%X.\n\n", PROGADDR);
        }

        if(strcmp(TOKEN, "loader") == 0){

            z = 0;
            for(int i = 0; i < 5; i++){
              ARR_LOAD[i] = (const char*)malloc(sizeof(const char) * MAX_ARR_SIZE);
            }

            TOK1 = strtok(ORG_IN, " \t");
            while(TOK1 != NULL){
                ARR_LOAD[z] = TOK1;
                TOK1 = strtok(NULL, " \t");
                z++;
            }
            z--;
  
            IsCommanded = loader(BUFFER, z);

        }

        if(strcmp(TOKEN, "bp") == 0){

            TOK3 = strtok(ORG, " \t");
            while(TOK3 != 0){
                strcpy(temp, TOK3);
                TOK3 = strtok(NULL, " \t");
            }

            if(strcmp(temp, "bp") == 0){
                if(bp_count == 0) printf("\nno breakpoints set.\n\n");
                else{
                    printf("\nbreakpoints\n----------\n");
                    for(int i = 0 ; i < bp_count; i++) printf("%d\n", BRKPN[i]);
                    printf("\n");
                }
                IsCommanded = 1;
            }
            else{
                TOK1 = strtok(ORG_IN, " \t");

                while(TOK1 != 0){
                    strcpy(BB, TOK1);
                    TOK1 = strtok(NULL," \t");
                }
                TOK2 = strtok(BB, " \t");
                while (TOK2 != 0){
                    strcpy(AA, TOK2);
                    TOK2 = strtok(NULL, " \t");
                }

                if(strcmp(AA, "clear") == 0){
                    // bp clear
                    for(int i =0; i < MAX_INPUT_SIZE; i++)BRKPN[i] = 0;
                    bp_count = 0;
                    printf("\n[ok] clear all breakpoints\n\n");

                }
                else{ // bp 4000
                    BRKPN[bp_count] = strtoul(AA, &ch, 10);
                    bp_count++;
                    printf("\n[ok] create breakpoint %d\n\n", BRKPN[bp_count-1]);
                }
                IsCommanded = 1;
            }
        }
        if(strcmp(TOKEN, "run") == 0){
            IsCommanded = run(BUFFER);
        }

        if(IsCommanded == 0){
            printf("There is no command or invalid input. Please input agian.\n");
            continue;
        }
  
    }

	return 0;

}
