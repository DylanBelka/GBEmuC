#ifndef GBEMC_INSTRUCTIONS_H
#define GBEMC_INSTRUCTIONS_H

#include "common.h"

struct instruction
{
	char *disassembly;
	void *execute;
	int opcode_length;
	int ticks;
};

extern struct instruction instructions[256];

void push(u16 r);
void pop(u16 *r);

#endif /* GBEMC_INSTRUCTIONS_H */
