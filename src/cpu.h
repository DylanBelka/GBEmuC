#ifndef GBEMC_CPU_H
#define GBEMC_CPU_H

struct instruction
{
	char *disassembly;
	void *execute;
	int opcode_length;
	//int ticks;
} extern const instructions[256];

void reset_cpu(void);
void cpu_tick(void);

void dump_cpu(void);
void cpu_test(void);

#endif // GBEMC_CPU_H