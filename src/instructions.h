#ifndef GBEMC_INSTRUCTIONS_H
#define GBEMC_INSTRUCTIONS_H

#include "common.h"

struct instruction
{
	char *disassembly;
	void *execute;
	int opcode_length;
};

extern struct instruction instructions[256];

extern const int instruction_ticks[256];
extern const int bit_instruction_ticks[256];

void push(u16 r);
void pop(u16 *r);

#endif // GBEMC_INSTRUCTIONS_H
