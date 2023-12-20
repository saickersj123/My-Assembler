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

    for(int i=0; i < extRefCount; ++i){
        printf("ExtRef Name: %s Addr: %04X Sec: %d\n",extRef[i].symbol, extRef[i].addr, extRef[i].sec);
    }
    for(int i=0; i < extDefCount; ++i){
        printf("ExtDef Name: %s Addr: %04X\n",extDef[i].symbol, extDef[i].addr);
    }
    return 0;
}    
