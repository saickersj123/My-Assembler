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
    for(int i =0; i < sec + 1; i++){
        printf("length of section %d : %04X\n", i, csect_table[i].program_length);
    }
    for(int i = 0; i < 3; i++){
    printf("extdef %d  addr : %04X name : %s\n", i, extDef[i].addr, extDef[i].symbol);
    }
    for(int i = 0; i < 2; i++){
    printf("extref %d  name : %s\n", i, extRef[i].symbol);
    }
    return 0;
}
