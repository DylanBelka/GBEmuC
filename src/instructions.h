#ifndef GBEMC_INSTRUCTIONS_H
#define GBEMC_INSTRUCTIONS_H

#include "common.h"

struct instruction
{
	char *disassembly;
	void *execute;
	int opcode_length;
	//int ticks;
} extern const instructions[256];

void inc(u8 *r);
void dec(u8 *r);
void cp(const u8 val);
void add(const u8 val);
void adc(const u8 val);
void sub(const u8 val);
void sbc(const u8 val);
void and(const u8 val);
void xor(const u8 val);
void or(const u8 val);
void push(u16 r);
void pop(u16 *r);
void add16(u16 *dst, u16 src);
void ret(bool condition);
void call(bool condition);
void rst(const u8 dst);
void jr(bool condition);
void jp(bool condition);
void ld_r_r(u8 instruction, u8 src, bool is_lower_instr);

#define OPCODE_LENGTH() instructions[read_byte(registers.PC)].opcode_length;

#endif // GBEMC_INSTRUCTIONS_H