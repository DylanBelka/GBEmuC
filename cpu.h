#ifndef GBEMC_CPU_H
#define GBEMC_CPU_H

struct instruction
{
	char *disassembly;
	void *execute;
	int opcode_length;
	//int ticks;
} extern const instructions[256];

void reset(void);
void cpu_tick(void);

void dumpCPU(void);
void test(void);

#endif // GBEMC_CPU_H