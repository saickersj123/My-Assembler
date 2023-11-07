#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
    FILE *f1,*f2,*f3,*f4,*f5,*f6,*f7;
    int pc,startingAdress,compareadresss,flag=0,flag2=0,opstart,obj,invalidopcode=1,instructionFormat,n,i,x,b,p,e,target,textRecordLength=0;
    char label[25],opcode[25],operand[25],comparelabel[25],comaddr[25],mn[25],mncode[25],objectcode[25],programLength[2],comparesymbol[25];

    //------------------------------------------------------------------
    f1 = fopen("intermediateFile.txt","r");
    f2 = fopen("symtab.txt","r");
    f3 = fopen("optab.txt","r");
    f4 = fopen("output.txt","w");
    f5 = fopen("proglen.txt","r");
    //------------------------------------------------------------------
    //while(!feof(f1))
    //{
        fscanf(f5,"%s",programLength);
    //}

    //rewind(f1);

    fscanf(f1,"%s%s%d",&label,&opcode,&opstart);
    if(strcmp(opcode,"START")==0)
    {
        startingAdress=opstart;
        pc = startingAdress;
        printf("\t%s\t%s\t%d\n",label,opcode,pc);
        printf("starting address = %d\n\n",startingAdress);
    }

    //---------------------Header Record--------------------
    char headerRecord[25]="";
    strcat(headerRecord,"H_00");
    char st[6];
    itoa(startingAdress,st,10);
    strcat(headerRecord,st);
    switch(strlen(programLength))
    {
        case 1 : strcat(headerRecord,"_00000");
                  break;
        case 2 : strcat(headerRecord,"_0000");
                  break;
        case 3 : strcat(headerRecord,"_000");
                  break;
        case 4 : strcat(headerRecord,"_00");
                  break;
        case 5 : strcat(headerRecord,"_0");
                  break;
        case 6 : strcat(headerRecord,"_");
                  break;
    }
    strcat(headerRecord,programLength);
    fprintf(f4,"%s",headerRecord);//----------------------Text Record-----------------
    char textRecord[50]="";
    strcat(textRecord,"T_");
    strcat(textRecord,"  _00");
    strcat(textRecord,st);

    while(!strcmp(opcode,"END")==0)
    {
        invalidopcode=1;
        fscanf(f1,"%x%s%s%s",&pc,&label,&opcode,&operand);

        //-------------Instruction Format -----------------------
        if(opcode[strlen(opcode)-1]=='R')
        {
            instructionFormat = 2;
            n=0;
            i=0;
            x=0;
            b=0;
            p=0;
            e=0;
        }
        else
        {
            if(opcode[0]=='+')
            {
                instructionFormat=4;
                e=1;
                for(int i=0;opcode[i]!='\0';i++)
                {
                    opcode[i]=opcode[i+1];
                }
            }
            else
            {
                e=0;
                instructionFormat=3;

            }
                if(operand[0]=='#')
                {
                    n=0;
                    i=1;
                    x=0;
                    b=0;
                    p=0;
                   // e=0;
                    for(int i=0;operand[i]!='\0';i++)
                    {
                        operand[i]=operand[i+1];
                    }
                }
                else if(operand[0]=='@')
                {
                    n=1;
                    i=0;
                    x=0;
                    b=0;
                    p=0;
                    //e=0;
                    for(int i=0;opcode[i]!='\0';i++)
                    {
                        opcode[i]=opcode[i+1];
                    }
                }
                else if(operand[strlen(operand)-1]=='X' && operand[strlen(operand)-2]==',')
                {
                    n=1;
                    i=1;
                    x=1;
                    b=0;
                    p=1;
                    operand[strlen(operand)-1]='\0';
                    operand[strlen(operand)-1]='\0';
                }
                else
                {
                    n=1;
                    i=1;
                    x=0;
                    b=0;
                    p=1;
                }


        }
        //--------------------------Generating Opcode----------------

        while(!feof(f3))
        {
            fscanf(f3,"%s%x",comparelabel,&compareadresss);
            if(strcmp(opcode,comparelabel)==0)
            {
                obj=compareadresss;
                invalidopcode=0;
                int obj1 = obj / 16;
                char a[10];
                itoa(obj1,a,16);
                strcat(mncode,a);
                obj = obj % 16;
                if(n == 1)
                {
                    obj = obj + 2 ;
                }
                if(i == 1)
                {
                    obj = obj + 1;
                }
                itoa(obj,a,16);
                strcat(mncode,a);

                if(instructionFormat==2)
                {
                    switch(operand[0])
                    {
                        case 'X' : strcat(mncode,"1");
                                 break;
                        case 'A' : strcat(mncode,"0");
                                 break;
                        case 'L' : strcat(mncode,"2");
                                 break;
                        case 'B' : strcat(mncode,"3");
                                 break;
                        case 'S' : strcat(mncode,"4");
                                 break;
                        case 'T' : strcat(mncode,"5");
                                  break;
                        case 'F' : strcat(mncode,"6");
                                  break;
                    }
                    switch(operand[2])
                    {
                        case 'X' : strcat(mncode,"1");
                                 break;
                        case 'A' : strcat(mncode,"0");
                                 break;
                        case 'L' : strcat(mncode,"2");
                                 break;
                        case 'B' : strcat(mncode,"3");
                                 break;
                        case 'S' : strcat(mncode,"4");
                                 break;
                        case 'T' : strcat(mncode,"5");
                                  break;
                        case 'F' : strcat(mncode,"6");
                                  break;
                    }
                }
                else
                {
                    obj = 0;
                    if(x==1)
                        obj=obj+8;
                    if(b==1)
                        obj = obj + 4;
                    if(p==1)
                        obj = obj + 2;
                    if(e==1)
                        obj = obj + 1;
                    itoa(obj,a,16);
                    strcat(mncode,a);
                    if(e==1)
                    {
                        if(n==1 && i==1)
                        {

                             while(!feof(f2))
                            {
                                fscanf(f2,"%x%s",&target,&comparesymbol);
                                if(strcmp(comparesymbol,operand)==0)
                                {
                                    switch(strlen(target))
                                    {
                                        case 1 : strcat(mncode,"_0000");
                                                  break;
                                        case 2 : strcat(mncode,"_000");
                                                  break;
                                        case 3 : strcat(mncode,"_00");
                                                  break;
                                        case 4 : strcat(mncode,"_0");
                                                  break;
                                        case 5 : strcat(mncode,"_");
                                                  break;
                                    }
                                    strcat(mncode,target);
                                }
                            }
                            rewind(f2);
                        }
                        else
                        {
                            switch(strlen(operand))
                            {
                                case 1 : strcat(mncode,"0000");
                                          break;
                                case 2 : strcat(mncode,"000");
                                          break;
                                case 3 : strcat(mncode,"00");
                                          break;
                                case 4 : strcat(mncode,"0");
                                          break;
                            }
                            strcat(mncode,operand);
                        }
                    }
                    else
                    {
                        if(n==1 && i==1)
                        {
                            while(!feof(f2))
                            {
                                fscanf(f2,"%x%s",&target,&comparesymbol);
                                if(strcmp(comparesymbol,operand)==0)
                                {
                                    obj = 0;
                                    obj = target - ( pc + 3 );
                                    itoa(obj,a,16);

                                    if(target<(pc+3))
                                    {
                                        for(int i=0;a[i]!='\0';i++)
                                        {
                                            a[i]=a[i+5];
                                        }
                                    }
                                    switch(strlen(a))
                                    {
                                        case 1 : strcat(mncode,"00");
                                                  break;
                                        case 2 : strcat(mncode,"0");
                                                  break;

                                    }
                                    strcat(mncode,a);
                                    break;
                                }
                            }
                            rewind(f2);
                        }
                        else
                        {
                            switch(strlen(operand))
                            {
                                case 1 : strcat(mncode,"00");
                                          break;
                                case 2 : strcat(mncode,"0");
                                          break;

                            }
                            strcat(mncode,operand);
                        }
                    }
                }

                break;
            }
        }
        if(strcmp(opcode,"BYTE")==0)
        {
            instructionFormat=1;
           if(operand[0]=='X')
           {
               for(int i =0 ; operand[i]!='\0';i++)
               {
                   operand[i]=operand[i+2];
               }
               operand[strlen(operand)-1]='\0';
               strcpy(mncode,operand);
           }
           else
           {
               char ele[2];
               char buffer[20]="";
               for(int i = 2;i<strlen(operand)-1;i++)
               {
                   sprintf(ele,"%x",operand[i]);
                   strcat(buffer,ele);
               }
               strcat(mncode,buffer);
           }

        }

        if((strcmp(opcode,"RESW")!=0 && strcmp(opcode,"RESB")!=0))
        {
            if(instructionFormat==2)
            {
                textRecordLength = textRecordLength + 2;
            }
            else if(instructionFormat==3)
            {
                textRecordLength = textRecordLength + 3;
            }
            else if(instructionFormat==1)
            {
                textRecordLength = textRecordLength+1;
            }
            else{
                textRecordLength = textRecordLength + 4;
            }
        }
        else
        {
            if(textRecordLength!=0)
            {
                char buffer[10];
                sprintf(buffer,"%x",textRecordLength);
                textRecord[2]=buffer[0];
                textRecord[3]=buffer[1];
                textRecordLength=0;
                flag=1;
                fprintf(f4,"\n%s",textRecord);
                strcpy(textRecord,"");
                strcat(textRecord,"T_  ");
            }


        }

        if(textRecordLength < 30 && !(strcmp(opcode,"RESW")==0 || strcmp(opcode,"RESB")==0))
        {
            if(flag==1)
            {
                strcat(textRecord,"_00");
                char buffer[10];
                itoa(pc,buffer,16);
                strcat(textRecord,buffer);
                flag=0;
            }
            strcat(textRecord,"_");
            strcat(textRecord,mncode);
            strcpy(mncode,"");
        }
    rewind(f3);
    //printf("Instruction format = %d %s %s\n",textRecordLength,opcode,mncode);
    strcpy(mncode,"");

   }
   // -----------------------------------------writing last text Record
    char buffer[10];
    sprintf(buffer,"%x",textRecordLength);
    if(strlen(buffer)==1)
    {
        textRecord[2]='0';
        textRecord[3]=buffer[0];
    }
    else
    {
        textRecord[2]=buffer[0];
        textRecord[3]=buffer[1];
    }
    textRecordLength=0;
    fprintf(f4,"\n%s",textRecord);
    //-----------------------------------------writing End Record
    char EndRecord[15]="";
    strcat(EndRecord,"E_00");
    char endaddr[10]="";
    itoa(startingAdress,endaddr,10);
    strcat(EndRecord,endaddr);
    fprintf(f4,"\n%s",EndRecord);
    printf("\nOBJECT GENERATED SUCCESSFULLY!!\n");
}
