CC = gcc
CFLAGS = -c -std=c11

gbemc: main.o cpu.o memory.o registers.o instructions.o
	$(CC) main.o cpu.o memory.o registers.o instructions.o -o gbemc

main.o: src/main.c
	$(CC) $(CFLAGS) src/main.c

cpu.o: src/cpu.c src/cpu.h
	$(CC) $(CFLAGS) src/cpu.c
	
memory.o: src/memory.c src/memory.h
	$(CC) $(CFLAGS) src/memory.c
	
registers.o: src/registers.c src/registers.h
	$(CC) $(CFLAGS) src/registers.c
	
instructions.o: src/instructions.c src/instructions.h
	$(CC) $(CFLGS) src/instructions.c
	
clean:
	rm *o gbemc