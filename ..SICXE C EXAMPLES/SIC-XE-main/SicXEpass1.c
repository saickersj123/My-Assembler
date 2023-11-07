#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
    FILE *f1,*f2,*f3,*f4,*f5,*f6,*f7;
    int pc,startingAdress,compareadresss,flag=0,flag2=0,programLength,opstart,obj,invalidopcode=1,instructionFormat;
    char label[25],opcode[25],operand[25],comparelabel[25],mn[25],mncode[25],objectcode[25];

    //------------------------------------------------------------------
    f1 = fopen("input.txt","r");
    f2 = fopen("symtab.txt","w+");
    f3 = fopen("intermediateFile.txt","w");
    f4 = fopen("symtab.txt","r");
    f5 = fopen("optab.txt","r");
    f6 = fopen("output.txt","w");
    f7 = fopen("proglen.txt","w");

    //------------------------------------------------------------------

    fscanf(f1,"%s%s%X",&label,&opcode,&opstart);
    if(strcmp(opcode,"START")==0)
    {
        startingAdress=opstart;
        pc = startingAdress;
        printf("\t%s\t%s\t%X\n",label,opcode,pc);
        printf("starting address = %X\n\n",startingAdress);
        fprintf(f3,"%X\t%s\t%s\t%X\n",startingAdress,label,opcode,startingAdress);
    }
    else
    {
        pc=0;
    }
    fscanf(f1,"%s%s%s",&label,&opcode,&operand);
    while(strcasecmp(opcode,"END")!=0)
    {
        if(opcode[0]=='+')
        {
            instructionFormat=4;
            for(int i=0;opcode[i]!='\0';i++)
            {
                opcode[i]=opcode[i+1];
            }
        }
        else
        {
            if(opcode[strlen(opcode)-1]=='R')
            {
                instructionFormat = 2;
            }
            else
            {
                instructionFormat=3;
            }

        }
        if(strcasecmp(label,"-")!=0)
        {
            while(!feof(f4))
        {
            //printf("hi");
            fscanf(f4,"%d%s",&compareadresss,&comparelabel);
            if(strcmp(label,comparelabel)==0)
            {
                flag=1;
                printf("\nDUPLICATE SYMBOL FOUND");
                break;
            }
            else
            {
                fscanf(f4,"%s%d",comparelabel,&compareadresss);
            }

        }
        if(flag==0)
        {
            fprintf(f2,"%X\t%s\n",pc,label);
        }

        }

        fscanf(f5,"%s%s",mn,mncode);
         while((strcmp(mn,"-")!=0))
        {

            if(strcmp(opcode,mn)==0)
            {
                strcpy(objectcode,mncode);
                invalidopcode=0;
                break;
            }
            else
            {
                fscanf(f4,"%s%s",comparelabel,&compareadresss);
                fscanf(f5,"%s%s",mn,mncode);
                invalidopcode=1;
                 //pc+=3;
            }

        }
        rewind(f5);
        if(invalidopcode==1)
        {

           // invalidopcode=0;

        }
        if(instructionFormat==4)
        {
            for(int i = strlen(opcode)+1;i>=0;i--)
            {
                opcode[i] = opcode[i-1];
            }
            opcode[0]='+';

        }
         fprintf(f3,"%x\t%s\t%s\t%s\t\n",pc,label,opcode,operand);
            printf("%x\t%s\t%s\t%s\t%s\n",pc,label,opcode,operand,mncode);



                if(strcasecmp(opcode,"WORD")==0)
                {
                    pc+=3;
                }
                else if(strcmp(opcode,"RESW")==0)
                {
                    int check = atoi(operand)*3;
                    pc = pc + (atoi(operand))*3;
                      //  printf("\nLength = %X\n",pc);
                }
                else if(strcmp(opcode,"RESB")==0)
                {
                    pc = pc + atoi(operand);
                }
                else if(strcmp(opcode,"BYTE")==0)
                {
                    if(operand[0]=='X')
                    {
                        pc+=1;
                    }
                    else
                    {
                        int len = strlen(operand)-3;
                    pc = pc + len;
                    }

                }
                else if(invalidopcode==0)
                {
                    if(instructionFormat==3)
                        pc = pc + 3;
                    else if (instructionFormat==4)
                        pc = pc + 4;
                    else
                        pc = pc + 2;

                }
                else
                {
                  printf("\tInvalid Opcode encountered :\n");
                }
                invalidopcode=0;
            fscanf(f1,"%s%s%s",&label,&opcode,&operand);
    }
    fprintf(f3,"%x\t%s\t%s\t%s\t\n",pc,"-","END","-");

     programLength = pc-startingAdress;
        printf("\nThe program length = %X\n",programLength);
        fprintf(f7,"%x",programLength);

}

