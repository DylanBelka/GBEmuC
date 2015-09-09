#include "cpu.h"

void nop(void) { }

void ld_bc_nn(u16 nn) { registers.BC = nn; }

void ld_pbc_a(void) { write_byte(registers.BC, registers.A); }

void inc_bc(void) { registers.BC++; }

const struct instruction instructions[256] = 
{
	{"NOP", nop, 1}, 
	{"LD BC, nn", ld_bc_nn, 3},
	{"LD (BC), A", ld_pbc_a, 1},
	{"INC BC", inc_bc, 1}
};
