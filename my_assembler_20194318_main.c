#include "my_assembler_20194318.c"
int main(void){
    // 초기화
    if (init_my_assembler() != 0) {
        fprintf(stderr, "Assembler initialization failed.\n");
        return 1;
    }
    //Pass 1 수행
    printf("\nPass 1 started.\n");
    if (assem_pass1() != 0) {
        fprintf(stderr, "Pass 1 failed.\n");
        return 1;
    }
    printf("\nPass 1 Ended.\n");
    // Pass 2 수행
    printf("\nPass 2 started.\n"); 
   if (assem_pass2() != 0) {
        fprintf(stderr, "Pass 1 failed.\n");
        return 1;
    }
    printf("\nPass 2 Ended.\n");
    //print success message
    printf("\nAssembler successfully completed both passes.\n");
    char buf[100];
    FILE *input_obj = fopen("objectprogram.txt", "r");
    while(fgets(buf,sizeof(buf),input_obj)!=NULL){
        printf("%s\n",buf);
    }
    FILE *input_lst = fopen("list.txt", "r");
    while(fgets(buf,sizeof(buf),input_lst)!=NULL){
        printf("%s\n",buf);
    }

    return 0;
}    
