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
    for(int i = 0; i < MAX_EXTDEF; i++){
    printf("extdef %d  addr : %04X name : %s\n", i, extDef[i].addr, extDef[i].symbol);
    }
    for(int i = 0; i < MAX_EXTREF; i++){
    printf("extref %d  name : %s sec : %d\n", i, extRef[i].symbol, extRef[i].sec);
    }
    printf("extref name : %s\n", extRef[2].symbol);
    return 0;
}
