CC = gcc
CFLAGS = -c -std=c11

gbemc: main.o cpu.o memory.o registers.o
	$(CC) main.o cpu.o memory.o registers.o -o gbemc

main.o: src/main.c
	$(CC) $(CFLAGS) src/main.c

cpu.o: src/cpu.c src/cpu.h
	$(CC) $(CFLAGS) src/cpu.c
	
memory.o: src/memory.c src/memory.h
	$(CC) $(CFLAGS) src/memory.c
	
registers.o: src/registers.c src/registers.h
	$(CC) $(CFLAGS) src/registers.c
	
clean:
	rm *o gbemc