20161577.out: main.o cmdProc.o shell.o memory.o hash.o assembler.o linkLoader.o execute.o linkedList.o
	gcc -Wall -o 20161577.out main.o cmdProc.o shell.o memory.o hash.o assembler.o linkLoader.o execute.o linkedList.o -lm
	@echo "\n>>> To execute, type ./20161577.out\n"

main.o: 20161577.h 20161577.c
	gcc -Wall -c -o main.o 20161577.c -lm

cmdProc.o: 20161577.h cmdProc.c
	gcc -Wall -c -o cmdProc.o cmdProc.c -lm

shell.o: 20161577.h shell.c
	gcc -Wall -c -o shell.o shell.c -lm

memory.o: 20161577.h memory.c
	gcc -Wall -c -o memory.o memory.c -lm

hash.o: 20161577.h hash.c
	gcc -Wall -c -o hash.o hash.c -lm

assembler.o: 20161577.h assembler.c
	gcc -Wall -c -o assembler.o assembler.c -lm

linkLoader.o: 20161577.h linkLoader.h
	gcc -Wall -c -o linkLoader.o linkLoader.c -lm

linkedList.o: 20161577.h linkedList.h shell.h
	gcc -Wall -c -o linkedList.o linkedList.c -lm

execute.o: 20161577.h linkedList.h execute.h
	gcc -Wall -c -o execute.o execute.c -lm

clean:
	-rm *.o
	-rm 20161577.out
