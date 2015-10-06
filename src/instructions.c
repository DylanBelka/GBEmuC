#include <stdio.h>

#include "instructions.h"
#include "registers.h"
#include "memory.h"
#include "cpu.h"

void update_carry(unsigned int val)
{
	if (val & 0xFF00)
		registers.F.C = 1;
	else
		registers.F.C = 0;
}

void update_halfcarry(unsigned int val)
{
	if (val & 0xF0)
		registers.F.H = 1;
	else
		registers.F.H = 0;
}

void update_zero(unsigned int val)
{
	if (val == 0)
		registers.F.Z = 1;
	else
		registers.F.Z = 0;
}

void inc(u8 *r)
{
	unsigned int result = *r + 1;
	(*r)++;

	update_halfcarry(result);
	update_zero(result);
	registers.F.N = 0;
}

void dec(u8 *r)
{
	unsigned int result = *r - 1;
	(*r)--;

	update_halfcarry(result);
	update_zero(result);
	registers.F.N = 1;
}

void cp(u8 val)
{
	unsigned int result = registers.A - val;
	update_carry(result);
	update_halfcarry(result);
	update_zero(result);
	registers.F.N = 1;
}

void add(u8 val)
{
	unsigned int result = registers.A + val;
	registers.A += val;

	update_carry(result);
	update_halfcarry(result);
	update_zero(result);
	registers.F.N = 0;
}

void adc(u8 val)
{
	unsigned int result = registers.A + val + registers.F.C;
	registers.A += val + registers.F.C;

	update_carry(result);
	update_halfcarry(result);
	update_zero(result);
	registers.F.N = 0;
}

void sub(u8 val)
{
	unsigned int result = registers.A - val;
	registers.A -= val;

	update_carry(result);
	update_halfcarry(result);
	update_zero(result);
}

void sbc(u8 val)
{
	unsigned int result = registers.A - (val + registers.F.C);
	registers.A -= (val + registers.F.C);

	update_carry(result);
	update_halfcarry(result);
	update_zero(result);
}

void and(u8 val)
{
	unsigned int result = registers.A & val;
	registers.A &= val;

	registers.F.C = 0;
	registers.F.H = 1;
	update_zero(result);
	registers.F.N = 0;
}

void xor(u8 val)
{
	unsigned int result = registers.A ^ val;
	registers.A ^= val;

	registers.F.C = 0;
	registers.F.H = 0;
	update_zero(result);
	registers.F.N = 0;
}

void or(u8 val)
{
	unsigned int result = registers.A | val;
	registers.A |= val;

	registers.F.C = 0;
	registers.F.H = 0;
	update_zero(result);
	registers.F.N = 0;
}

void push(u16 r)
{
	registers.SP--;
	write_byte(registers.SP, r & 0xFF);
	registers.SP--;
	write_byte(registers.SP, r >> 0x8);
}

void pop(u16 *r)
{
	*r = (read_byte(registers.SP) & 0xFF) << 0x8;
	registers.SP++;
	*r |= read_byte(registers.SP) & 0xFF;
	registers.SP++;
}

void add16(u16 *dst, u16 src)
{
	unsigned int result = *dst + src;
	*dst += src;

	update_carry(result);
	update_halfcarry(result);
	registers.F.N = 0;
}

void ret(bool condition)
{
	if (condition)
	{
		pop(&registers.PC);
		cpu.clock_cycles += 12;
	}
	else
	{
		registers.PC++;
	}
}

void call(bool condition)
{
	if (condition)
	{
		push(registers.PC + 3);
		registers.PC = read_word(registers.PC + 1);
		cpu.clock_cycles += 12;
	}
	else
	{
		registers.PC += 3;
	}
}

void rst(u8 dst)
{
	push(registers.PC + 1);
	registers.PC = dst;
}

void jr(bool condition)
{
	if (condition)
	{
		registers.PC += (s8)read_byte(registers.PC + 1) + 2;
		cpu.clock_cycles += 4;
	}
	else
	{
		registers.PC += 2;
	}
}

void jp(bool condition)
{
	if (condition)
	{
		registers.PC = read_word(registers.PC + 1);
		cpu.clock_cycles += 4;
	}
	else
	{
		registers.PC += 3;
	}
}

void rotl(u8 *r, int shift)
{
	u8 result;
	result = (*r << shift) | (*r >> (8 - shift));
	*r = result;
}

void rotr(u8 *r, int shift)
{
	u8 result;
	result = (*r >> shift) | (*r << (8 - shift));
	*r = result;
}

void rlc(u8 *r)
{
	rotl(r, 1);

	registers.F.C = (*r >> 0x7) & 0x1;
	*r |= (*r >> 0x7) & 0x1;
	registers.F.H = 0;
	registers.F.N = 0;
}

void rrc(u8 *r)
{
	rotr(r, 1);

	registers.F.C = *r & 0x1;
	*r |= (*r << 0x7) & 0x80;
	registers.F.H = 0;
	registers.F.N = 0;
}

void rl(u8 *r)
{
	rotl(r, 1);

	*r |= registers.F.C;
	registers.F.C = (*r >> 0x7) & 0x1;
	registers.F.H = 0;
	registers.F.N = 0;
}

void rr(u8 *r)
{
	rotr(r, 1);

	*r |= (registers.F.C << 0x7) & 0x80;
	registers.F.C = *r & 0x1;
	registers.F.H = 0;
	registers.F.N = 0;
}

void sla(u8 *r)
{
	*r <<= 1;

	registers.F.C = (*r >> 0x7) & 0x1;
	*r &= ~0x1u;
	registers.F.H = 0;
	registers.F.N = 0;
}

void sra(u8 *r)
{
	*r >>= 1;

	registers.F.C = *r & 0x1;
	registers.F.H = 0;
	registers.F.N = 0;
}

void swap(u8 *r)
{
	u8 swp = ((*r & 0x0F) << 0x4) | ((*r & 0xF0) >> 0x4);
	*r = swp;

	update_zero(swp);
	registers.F.C = 0;
	registers.F.H = 0;
	registers.F.N = 0;
}

void srl(u8 *r)
{
	*r >>= 1;

	registers.F.C = *r & 0x1;
	registers.A &= ~0x80u;
	registers.F.H = 0;
	registers.F.N = 0;
}

void bit(u8 bit, u8 r)
{
	if (r & bit)
		registers.F.Z = 0;
	else
		registers.F.Z = 1;
	registers.F.H = 1;
	registers.F.N = 0;
}

void res(u8 bit, u8 *r)
{
	*r &= ~bit;
}

void set(u8 bit, u8 *r)
{
	*r |= bit;
}

void bit_instruction_impl(u8 instr)
{
	u8 *op1s[] = { &registers.B, &registers.C, &registers.D, &registers.E,
					&registers.H, &registers.L, get_byte(registers.HL), &registers.A,
					&registers.B, &registers.C, &registers.D, &registers.E,
					&registers.H, &registers.L, get_byte(registers.HL), &registers.A };
	u8 op2;
	u8 *op1 = op1s[instr & 0x0F];
	bool is_lower_instr = (instr & 0x0F) < 0x8;

	if ((instr & 0x0F) == 6 || (instr & 0x0F) == 14)
	{
		cpu.clock_cycles += 8;
	}

	if (instr >= 0x40) /* instructions from 0x40-0xFF have non register, constant operands */
	{
		/*
		 those operands go in order from b0-b7 with two bits per 16 byte
		 row in the instruction table
		 first bit is low byte 0x00-0x07
		*/
		u8 op2sLo[] = { 0x1, 0x4, 0x10, 0x40 };
		/* second bit is high byte 0x08-0x0F */
		u8 op2sHi[] = { 0x2, 0x8, 0x20, 0x80 };
		unsigned int op2Index = ((instr >> 0x4) & 0x0F) - 4;
		if (is_lower_instr)
		{
			op2 = op2sLo[op2Index];
		}
		else
		{
			op2 = op2sHi[op2Index];
		}
	}

	switch (instr & 0xF0)
	{
		case 0x00:
		{
			if (is_lower_instr)
				rlc(op1);
			else
				rrc(op1);
			break;
		}
		case 0x10:
		{
			if (is_lower_instr)
				rl(op1);
			else
				rr(op1);
			break;
		}
		case 0x20:
		{
			if (is_lower_instr)
				sla(op1);
			else
				sra(op1);
			break;
		}
		case 0x30:
		{
			if (is_lower_instr)
				swap(op1);
			else
				srl(op1);
			break;
		}
		case 0x40: case 0x50: case 0x60: case 0x70:
		{
			bit(op2, *op1);
			break;
		}
		case 0x80: case 0x90: case 0xA0: case 0xB0:
		{
			res(op2, op1);
			break;
		}
		case 0xC0: case 0xD0: case 0xE0: case 0xF0:
		{
			set(op2, op1);
			break;
		}
	}
}

void daa_impl(void)
{
	unsigned int result = registers.A;
	u8 correction = 0x0;
	if (registers.A > 0x99 || registers.F.C)
	{
		correction |= 0x60;
		registers.F.C = 1;
	}
	else
	{
		correction = 0x0;
		registers.F.C = 0;
	}
	if (((registers.A & 0x0F) > 0x9) || registers.F.H)
	{
		correction |= 0x6;
	}
	if (registers.F.N)
	{
		result = registers.A + correction;
		registers.A += correction;
	}
	else
	{
		result = registers.A - correction;
		registers.A -= correction;
	}
	update_zero(registers.A);
	update_halfcarry(result);
}

void nop(void) { }																	/* 0x00 */
void ld_bc_nn(void) { registers.BC = read_word(registers.PC + 1); }					/* 0x01 */
void ld_pbc_a(void) { write_byte(registers.BC, registers.A); }						/* 0x02 */
void inc_bc(void) { registers.BC++; }												/* 0x03 */
void inc_b(void) { inc(&registers.B); }												/* 0x04 */
void dec_b(void) { dec(&registers.B); }												/* 0x05 */
void ld_b_n(void) { registers.B = read_byte(registers.PC + 1); }					/* 0x06 */
void rlca(void) { rlc(&registers.A); }												/* 0x07 */
void ld_pnn_sp(void) { write_word(read_word(registers.PC + 1), registers.SP); } 	/* 0x08 */
void add_hl_bc(void) { add16(&registers.HL, registers.BC); }						/* 0x09 */
void ld_a_pbc(void) { registers.A = read_byte(registers.BC); }						/* 0x0A */
void dec_bc(void) { registers.BC--; }												/* 0x0B */
void inc_c(void) { inc(&registers.C); }												/* 0x0C */
void dec_c(void) { dec(&registers.C); }												/* 0x0D */
void ld_c_n(void) { registers.C = read_byte(registers.PC + 1); }					/* 0x0E */
void rrca(void) { rrc(&registers.A); }												/* 0x0F */
void stop_cpu(void) { cpu.is_stopped = true; }										/* 0x10 */
void ld_de_nn(void) { registers.DE = read_word(registers.PC + 1); }					/* 0x11 */
void ld_pde_a(void) { write_byte(registers.DE, registers.A); }						/* 0x12 */
void inc_de(void) { registers.DE++; }												/* 0x13 */
void inc_d(void) { inc(&registers.D); }												/* 0x14 */
void dec_d(void) { dec(&registers.D); }												/* 0x15 */
void ld_d_n(void) { registers.D = read_byte(registers.PC + 1); }					/* 0x16 */
void rla(void) { rl(&registers.A); }												/* 0x17 */
void jr_n(void) { jr(true); }														/* 0x18 */
void add_hl_de(void) { add16(&registers.HL, registers.DE); }						/* 0x19 */
void ld_a_pde(void) { registers.A = read_byte(registers.DE); }						/* 0x1A */
void dec_de(void) { registers.DE--; }												/* 0x1B */
void inc_e(void) { inc(&registers.E); }												/* 0x1C */
void dec_e(void) { dec(&registers.E); }												/* 0x1D */
void ld_e_n(void) { registers.E = read_byte(registers.PC + 1); }					/* 0x1E */
void rra(void) { rr(&registers.A); }												/* 0x1F */
void jr_nz_n(void) { jr(!registers.F.Z); }											/* 0x20 */
void ld_hl_nn(void) { registers.HL = read_word(registers.PC + 1); } 				/* 0x21 */
void ldi_phl_a(void) { write_byte(registers.HL, registers.A); registers.HL++; }		/* 0x22 */
void inc_hl(void) { registers.HL++; }												/* 0x23 */
void inc_h(void) { inc(&registers.H); }												/* 0x24 */
void dec_h(void) { dec(&registers.H); }												/* 0x25 */
void ld_h_n(void) { registers.H = read_byte(registers.PC + 1); }					/* 0x26 */
void daa(void) { daa_impl(); }														/* 0x27 */
void jr_z_n(void) { jr(registers.F.Z); }											/* 0x28 */
void add_hl_hl(void) { add16(&registers.HL, registers.HL); }						/* 0x29 */
void ldi_a_phl(void) { registers.A = read_byte(registers.HL); registers.HL++; }		/* 0x2A */
void dec_hl(void) { registers.HL--; }												/* 0x2B */
void inc_l(void) { inc(&registers.L); }												/* 0x2C */
void dec_l(void) { dec(&registers.L); }												/* 0x2D */
void ld_l_n(void) { registers.L = read_byte(registers.PC + 1); }					/* 0x2E */
void cpl(void) { registers.A = ~registers.A; }										/* 0x2F */
void jr_nc_n(void) { jr(!registers.F.C); }											/* 0x30 */
void ld_sp_nn(void) { registers.SP = read_word(registers.PC + 1); }					/* 0x31 */
void ldd_phl_a(void) { write_byte(registers.HL, registers.A); registers.HL--; }		/* 0x32 */
void inc_sp(void) { registers.SP++; }												/* 0x33 */
void inc_phl(void) { inc(get_byte(registers.HL)); }									/* 0x34 */
void dec_phl(void) { dec(get_byte(registers.HL)); }									/* 0x35 */
void ld_phl_n(void) { write_byte(registers.HL, read_byte(registers.PC + 1)); }		/* 0x36 */
void scf(void) { registers.F.C = 1; registers.F.N = 0; registers.H = 0; }			/* 0x37 */
void jr_c_n(void) { jr(registers.F.C); }											/* 0x38 */
void add_hl_sp(void) { add16(&registers.HL, registers.SP); }						/* 0x39 */
void ldd_a_phl(void) { registers.A = read_byte(registers.HL); registers.HL--; }		/* 0x3A */
void dec_sp(void) { registers.SP--; }												/* 0x3B */
void inc_a(void) { inc(&registers.A); }												/* 0x3C */
void dec_a(void) { dec(&registers.A); }												/* 0x3D */
void ld_a_n(void) { registers.A = read_byte(registers.PC + 1); }					/* 0x3E */
void ccf(void) { registers.F.C = !registers.F.C; }									/* 0x3F */
void ld_b_b(void) { registers.B = registers.B; }									/* 0x40 */
void ld_b_c(void) { registers.B = registers.C; }									/* 0x41 */
void ld_b_d(void) { registers.B = registers.D; }									/* 0x42 */
void ld_b_e(void) { registers.B = registers.E; }									/* 0x43 */
void ld_b_h(void) { registers.B = registers.H; }									/* 0x44 */
void ld_b_l(void) { registers.B = registers.L; }									/* 0x45 */
void ld_b_phl(void){registers.B = read_byte(registers.HL); }						/* 0x46 */
void ld_b_a(void) { registers.B = registers.A; }									/* 0x47 */
void ld_c_b(void) { registers.C = registers.B; }									/* 0x48 */
void ld_c_c(void) { registers.C = registers.C; }									/* 0x49 */
void ld_c_d(void) { registers.C = registers.D; }									/* 0x4A */
void ld_c_e(void) { registers.C = registers.E; }									/* 0x4B */
void ld_c_h(void) { registers.C = registers.H; }									/* 0x4C */
void ld_c_l(void) { registers.C = registers.L; }									/* 0x4D */
void ld_c_phl(void){registers.C = read_byte(registers.HL); }						/* 0x4E */
void ld_c_a(void) { registers.C = registers.A; }									/* 0x4F */
void ld_d_b(void) { registers.D = registers.B; }									/* 0x50 */
void ld_d_c(void) { registers.D = registers.C; }									/* 0x51 */
void ld_d_d(void) { registers.D = registers.D; }									/* 0x52 */
void ld_d_e(void) { registers.D = registers.E; }									/* 0x53 */
void ld_d_h(void) { registers.D = registers.H; }									/* 0x54 */
void ld_d_l(void) { registers.D = registers.L; }									/* 0x55 */
void ld_d_phl(void){registers.D = read_byte(registers.HL); }						/* 0x56 */
void ld_d_a(void) { registers.D = registers.A; }									/* 0x57 */
void ld_e_b(void) { registers.E = registers.B; }									/* 0x58 */
void ld_e_c(void) { registers.E = registers.C; }									/* 0x59 */
void ld_e_d(void) { registers.E = registers.D; }									/* 0x5A */
void ld_e_e(void) { registers.E = registers.E; }									/* 0x5B */
void ld_e_h(void) { registers.E = registers.H; }									/* 0x5C */
void ld_e_l(void) { registers.E = registers.L; }									/* 0x5D */
void ld_e_phl(void){registers.E = read_byte(registers.HL); }						/* 0x5E */
void ld_e_a(void) { registers.E = registers.A; }									/* 0x5F */
void ld_h_b(void) { registers.H = registers.B; }									/* 0x60 */
void ld_h_c(void) { registers.H = registers.C; }									/* 0x61 */
void ld_h_d(void) { registers.H = registers.D; }									/* 0x62 */
void ld_h_e(void) { registers.H = registers.E; }									/* 0x63 */
void ld_h_h(void) { registers.H = registers.H; }									/* 0x64 */
void ld_h_l(void) { registers.H = registers.L; }									/* 0x65 */
void ld_h_phl(void){registers.H = read_byte(registers.HL); }						/* 0x66 */
void ld_h_a(void) { registers.H = registers.A; }									/* 0x67 */
void ld_l_b(void) { registers.L = registers.B; }									/* 0x68 */
void ld_l_c(void) { registers.L = registers.C; }									/* 0x69 */
void ld_l_d(void) { registers.L = registers.D; }									/* 0x6A */
void ld_l_e(void) { registers.L = registers.E; }									/* 0x6B */
void ld_l_h(void) { registers.L = registers.H; }									/* 0x6C */
void ld_l_l(void) { registers.L = registers.L; }									/* 0x6D */
void ld_l_phl(void){registers.L = read_byte(registers.HL); }						/* 0x6E */
void ld_l_a(void) { registers.L = registers.A; }									/* 0x6F */
void ld_phl_b(void) { write_byte(registers.HL, registers.B); }						/* 0x70 */
void ld_phl_c(void) { write_byte(registers.HL, registers.C); }						/* 0x71 */
void ld_phl_d(void) { write_byte(registers.HL, registers.D); }						/* 0x72 */
void ld_phl_e(void) { write_byte(registers.HL, registers.E); }						/* 0x73 */
void ld_phl_h(void) { write_byte(registers.HL, registers.H); }						/* 0x74 */
void ld_phl_l(void) { write_byte(registers.HL, registers.L); }						/* 0x75 */
void halt_cpu(void) { cpu.is_halted = true; }										/* 0x76 */
void ld_phl_a(void) { write_byte(registers.HL, registers.A); }						/* 0x77 */
void ld_a_b(void) { registers.A = registers.B; }									/* 0x78 */
void ld_a_c(void) { registers.A = registers.C; }									/* 0x79 */
void ld_a_d(void) { registers.A = registers.D; }									/* 0x7A */
void ld_a_e(void) { registers.A = registers.E; }									/* 0x7B */
void ld_a_h(void) { registers.A = registers.H; }									/* 0x7C */
void ld_a_l(void) { registers.A = registers.L; }									/* 0x7D */
void ld_a_phl(void){registers.A = read_byte(registers.HL); }						/* 0x7E */
void ld_a_a(void) { registers.A = registers.A; }									/* 0x7F */
void add_a_b(void) { add(registers.B); }											/* 0x80 */
void add_a_c(void) { add(registers.C); }											/* 0x81 */
void add_a_d(void) { add(registers.D); }											/* 0x82 */
void add_a_e(void) { add(registers.E); }											/* 0x83 */
void add_a_h(void) { add(registers.H); }											/* 0x84 */
void add_a_l(void) { add(registers.L); }											/* 0x85 */
void add_a_phl(void){add(read_byte(registers.HL)); }								/* 0x86 */
void add_a_a(void) { add(registers.A); }											/* 0x87 */
void adc_a_b(void) { adc(registers.B); }											/* 0x88 */
void adc_a_c(void) { adc(registers.C); }											/* 0x89 */
void adc_a_d(void) { adc(registers.D); }											/* 0x8A */
void adc_a_e(void) { adc(registers.E); }											/* 0x8B */
void adc_a_h(void) { adc(registers.H); }											/* 0x8C */
void adc_a_l(void) { adc(registers.L); }											/* 0x8D */
void adc_a_phl(void){adc(read_byte(registers.HL)); }								/* 0x8E */
void adc_a_a(void) { adc(registers.A); }											/* 0x8F */
void sub_b(void) { sub(registers.B); }												/* 0x90 */
void sub_c(void) { sub(registers.C); }												/* 0x91 */
void sub_d(void) { sub(registers.D); }												/* 0x92 */
void sub_e(void) { sub(registers.E); }												/* 0x93 */
void sub_h(void) { sub(registers.H); }												/* 0x94 */
void sub_l(void) { sub(registers.L); }												/* 0x95 */
void sub_phl(void){sub(read_byte(registers.HL)); }									/* 0x96 */
void sub_a(void) { sub(registers.A); }												/* 0x97 */
void sbc_a_b(void) { sbc(registers.B); }											/* 0x98 */
void sbc_a_c(void) { sbc(registers.C); }											/* 0x99 */
void sbc_a_d(void) { sbc(registers.D); }											/* 0x9A */
void sbc_a_e(void) { sbc(registers.E); }											/* 0x9B */
void sbc_a_h(void) { sbc(registers.H); }											/* 0x9C */
void sbc_a_l(void) { sbc(registers.L); }											/* 0x9D */
void sbc_a_phl(void){sbc(read_byte(registers.HL)); }								/* 0x9E */
void sbc_a_a(void) { sbc(registers.A); }											/* 0x9F */
void and_b(void) { and(registers.B); }												/* 0xA0 */
void and_c(void) { and(registers.C); }												/* 0xA1 */
void and_d(void) { and(registers.D); }												/* 0xA2 */
void and_e(void) { and(registers.E); }												/* 0xA3 */
void and_h(void) { and(registers.H); }												/* 0xA4 */
void and_l(void) { and(registers.L); }												/* 0xA5 */
void and_phl(void){and(read_byte(registers.HL)); }									/* 0xA6 */
void and_a(void) { and(registers.A); }												/* 0xA7 */
void xor_b(void) { xor(registers.B); }												/* 0xA8 */
void xor_c(void) { xor(registers.C); }												/* 0xA9 */
void xor_d(void) { xor(registers.D); }												/* 0xAA */
void xor_e(void) { xor(registers.E); }												/* 0xAB */
void xor_h(void) { xor(registers.H); }												/* 0xAC */
void xor_l(void) { xor(registers.L); }												/* 0xAD */
void xor_phl(void){xor(read_byte(registers.HL)); }									/* 0xAE */
void xor_a(void) { xor(registers.A); }												/* 0xAF */
void or_b(void) { or(registers.B); }												/* 0xB0 */
void or_c(void) { or(registers.C); }												/* 0xB1 */
void or_d(void) { or(registers.D); }												/* 0xB2 */
void or_e(void) { or(registers.E); }												/* 0xB3 */
void or_h(void) { or(registers.H); }												/* 0xB4 */
void or_l(void) { or(registers.L); }												/* 0xB5 */
void or_phl(void){or(read_byte(registers.HL)); }									/* 0xB6 */
void or_a(void) { or(registers.A); }												/* 0xB7 */
void cp_b(void) { cp(registers.B); }												/* 0xB8 */
void cp_c(void) { cp(registers.C); }												/* 0xB9 */
void cp_d(void) { cp(registers.D); }												/* 0xBA */
void cp_e(void) { cp(registers.E); }												/* 0xBB */
void cp_h(void) { cp(registers.H); }												/* 0xBC */
void cp_l(void) { cp(registers.L); }												/* 0xBD */
void cp_phl(void){cp(read_byte(registers.HL)); }									/* 0xBE */
void cp_a(void) { cp(registers.A); }												/* 0xBF */
void ret_nz(void) { ret(!registers.F.Z); }											/* 0xC0 */
void pop_bc(void) { pop(&registers.BC); }											/* 0xC1 */
void jp_nz_nn(void) { jp(!registers.F.Z); }											/* 0xC2 */
void jp_nn(void) { jp(true); }														/* 0xC3 */
void call_nz_nn(void) { call(!registers.F.Z); }										/* 0xC4 */
void push_bc(void) { push(registers.BC); }											/* 0xC5 */
void add_a_n(void) { add(read_byte(registers.PC + 1)); }							/* 0xC6 */
void rst_0x00(void) { rst(0x00); }													/* 0xC7 */
void ret_z(void) { ret(registers.F.Z); }											/* 0xC8 */
void ret_unconditional(void) { ret(true); }											/* 0xC9 */
void jp_z_nn(void) { jp(registers.F.Z); }											/* 0xCA */
void bit_instruction(void) { bit_instruction_impl(read_byte(registers.PC + 1)); }	/* 0xCB */
void call_z_nn(void) { call(registers.F.Z); }										/* 0xCC */
void call_nn(void) { call(true); }													/* 0xCD */
void adc_a_n(void) { adc(read_byte(registers.PC + 1)); }							/* 0xCE */
void rst_0x08(void) { rst(0x08); }													/* 0xCF */
void ret_nc(void) { ret(!registers.F.C); }											/* 0xD0 */
void pop_de(void) { pop(&registers.DE); }											/* 0xD1 */
void jp_nc_nn(void) { jp(!registers.F.C); }											/* 0xD2 */
void call_nc_nn(void) { call(!registers.F.C); }										/* 0xD4 */
void push_de(void) { push(registers.DE); }											/* 0xD5 */
void sub_n(void) { sub(read_byte(registers.PC + 1)); }								/* 0xD6 */
void rst_0x10(void) { rst(0x10); }													/* 0xD7 */
void ret_c(void) { ret(registers.F.C); }											/* 0xD8 */
void reti(void) { ret(true); cpu.IME = true; }										/* 0xD9 */
void jp_c_nn(void) { jp(registers.F.C); }											/* 0xDA */
void call_c_nn(void) { call(registers.F.C); }										/* 0xDB */
void sbc_a_n(void) { sbc(read_byte(registers.PC + 1)); }							/* 0xDE */
void rst_0x18(void) { rst(0x18); }													/* 0xDF */
void ld_zpg_a(void) { write_byte(read_byte(registers.PC + 1) + 0xFF00, registers.A); } /* 0xE0 */
void pop_hl(void) { pop(&registers.HL); }											/* 0xE1 */
void ld_pc_a(void) { write_byte(registers.C + 0xFF00, registers.A); }				/* 0xE2 */
void push_hl(void) { push(registers.HL); }											/* 0xE3 */
void and_n(void) { and(read_byte(registers.PC + 1)); }								/* 0xE4 */
void rst_0x20(void) { rst(0x20); }													/* 0xE5 */
void add_sp_n(void) { add16(&registers.SP, read_byte(registers.PC + 1) & 0xFF); }	/* 0xE8 */
void jp_hl(void) { registers.PC = registers.HL; }									/* 0xE9 */
void ld_pnn_a(void) { write_byte(read_word(registers.PC + 1), registers.A); }		/* 0xEA */
void xor_n(void) { xor(read_byte(registers.PC + 1)); }								/* 0xEE */
void rst_0x28(void) { rst(0x28); }													/* 0xEF */
void ld_a_zpg(void) { registers.A = read_byte((u8)read_byte(registers.PC + 1) + (u16)0xFF00); } /* 0xF0 */
void pop_af(void) { pop(&registers.AF); }											/* 0xF1 */
void ld_a_pc(void) { registers.A = read_byte(registers.C + 0xFF00); }				/* 0xF2 */
void di(void) { cpu.IME = false; }													/* 0xF3 */
void push_af(void) { push(registers.AF); }											/* 0xF5 */
void or_n(void) { or(read_byte(registers.PC + 1)); }								/* 0xF6 */
void rst_0x30(void) { rst(0x30); }													/* 0xF7 */
void ld_hl_sp(void) { registers.HL = registers.SP + read_byte(registers.PC + 1); }	/* 0xF8 */
void ld_sp_hl(void) { registers.SP = registers.HL; }								/* 0xF9 */
void ld_a_pnn(void) { registers.A = read_byte(read_word(registers.PC + 1)); }		/* 0xFA */
void ei(void) { cpu.IME = true; }													/* 0xFB */
void cp_n(void) { cp(read_byte(registers.PC + 1)); }								/* 0xFE */
void rst_0x38(void) { rst(0x38); }													/* 0xFF */

void undef_instr(void)
{
	printf("undefined instruction 0x%x pc = 0x%x", read_byte(registers.PC), registers.PC);
	getchar();
}

struct instruction instructions[256] =
{
	{"nop", nop, 1, 4},
	{"ld bc, 0x%x", ld_bc_nn, 3, 12},
	{"ld (bc), a", ld_pbc_a, 1, 8},
	{"inc bc", inc_bc, 1, 8},
	{"inc b", inc_b, 1, 4},
	{"dec b", dec_b, 1, 4},
	{"ld b, 0x%x", ld_b_n, 2, 8},
	{"rlca", rlca, 1, 4},
	{"ld (0x%x), sp", ld_pnn_sp, 3, 20},
	{"add hl, bc", add_hl_bc, 1, 8},
	{"ld a, (bc)", ld_a_pbc, 1, 8},
	{"dec bc", dec_bc, 1, 8},
	{"inc c", inc_c, 1, 4},
	{"dec c", dec_c, 1, 4},
	{"ld c, 0x%x", ld_c_n, 2, 8},
	{"rrca", rrca, 1, 4},
	{"stop", stop_cpu, 1, 4},
	{"ld de, 0x%x", ld_de_nn, 3, 12},
	{"ld (de), a", ld_pde_a, 1, 8},
	{"inc de", inc_de, 1, 8},
	{"inc d", inc_d, 1, 4},
	{"dec d", dec_d, 1, 4},
	{"ld d, 0x%x", ld_d_n, 2, 8},
	{"rla", rla, 1, 4},
	{"jr 0x%x", jr_n, 0, 8},
	{"add hl, de", add_hl_de, 1, 8},
	{"ld a, (de)", ld_a_pde, 1, 8},
	{"dec de", dec_de, 1, 8},
	{"inc e", inc_e, 1, 4},
	{"dec e", dec_e, 1, 4},
	{"ld e, 0x%x", ld_e_n, 2, 8},
	{"rra", rra, 1, 4},
	{"jr nz, 0x%x", jr_nz_n, 0, 8},
	{"ld hl, 0x%x", ld_hl_nn, 3, 12},
	{"ldi (hl), a", ldi_phl_a, 1, 8},
	{"inc hl", inc_hl, 1, 8},
	{"inc h", inc_h, 1, 4},
	{"dec h", dec_h, 1, 4},
	{"ld h, 0x%x", ld_h_n, 2, 8},
	{"daa", daa, 1, 4},
	{"jr z, 0x%x", jr_z_n, 0, 8},
	{"add hl, hl", add_hl_hl, 1, 8},
	{"ldi a, (hl)", ldi_a_phl, 1, 8},
	{"dec hl", dec_hl, 1, 8},
	{"inc l", inc_l, 1, 4},
	{"dec l", dec_l, 1, 4},
	{"ld l, 0x%x", ld_l_n, 2, 8},
	{"cpl", cpl, 1, 4},
	{"jr nc, 0x%x", jr_nc_n, 0, 8},
	{"ld sp, 0x%x", ld_sp_nn, 3, 12},
	{"ldd (hl), a", ldd_phl_a, 1, 8},
	{"inc sp", inc_sp, 1, 8},
	{"inc (hl)", inc_phl, 1, 12},
	{"dec (hl)", dec_phl, 1, 12},
	{"ld (hl), 0x%x", ld_phl_n, 2, 8},
	{"scf", scf, 1, 4},
	{"jr c, 0x%x", jr_c_n, 0, 8},
	{"add hl, sp", add_hl_sp, 1, 8},
	{"ldd a, (hl)", ldd_a_phl, 1, 8},
	{"dec sp", dec_sp, 1, 8},
	{"inc a", inc_a, 1, 4},
	{"dec a", dec_a, 1, 4},
	{"ld a, 0x%x", ld_a_n, 2, 8},
	{"ccf", ccf, 1, 4},
	{"ld b, b",	ld_b_b, 1, 4 },
	{"ld b, c",	ld_b_c, 1, 4 },
	{"ld b, d",	ld_b_d, 1, 4 },
	{"ld b, e",	ld_b_e, 1, 4 },
	{"ld b, h",	ld_b_h, 1, 4 },
	{"ld b, l",	ld_b_l, 1, 4 },
	{"ld b, (hl)", ld_b_phl, 1, 8 },
	{"ld b, a",	ld_b_a, 1, 4 },
	{"ld c, b",	ld_c_b, 1, 4 },
	{"ld c, c",	ld_c_c, 1, 4 },
	{"ld c, d",	ld_c_d, 1, 4 },
	{"ld c, e",	ld_c_e, 1, 4 },
	{"ld c, h",	ld_c_h, 1, 4 },
	{"ld c, l",	ld_c_l, 1, 4 },
	{"ld c, (hl)", ld_c_phl, 1, 8 },
	{"ld c, a",	ld_c_a, 1, 4 },
	{"ld d, b",	ld_d_b, 1, 4 },
	{"ld d, c",	ld_d_c, 1, 4 },
	{"ld d, d",	ld_d_d, 1, 4 },
	{"ld d, e",	ld_d_e, 1, 4 },
	{"ld d, h",	ld_d_h, 1, 4 },
	{"ld d, l",	ld_d_l, 1, 4 },
	{"ld d, (hl)", ld_d_phl, 1, 8 },
	{"ld d, a",	ld_d_a, 1, 4 },
	{"ld e, b",	ld_e_b, 1, 4 },
	{"ld e, c",	ld_e_c, 1, 4 },
	{"ld e, d",	ld_e_d, 1, 4 },
	{"ld e, e",	ld_e_e, 1, 4 },
	{"ld e, h",	ld_e_h, 1, 4 },
	{"ld e, l",	ld_e_l, 1, 4 },
	{"ld e, (hl)", ld_e_phl, 1, 8 },
	{"ld e, a",	ld_e_a, 1, 4 },
	{"ld h, b",	ld_h_b, 1, 4 },
	{"ld h, c",	ld_h_c, 1, 4 },
	{"ld h, d",	ld_h_d, 1, 4 },
	{"ld h, e",	ld_h_e, 1, 4 },
	{"ld h, h",	ld_h_h, 1, 4 },
	{"ld h, l",	ld_h_l, 1, 4 },
	{"ld h, (hl)", ld_h_phl, 1, 8 },
	{"ld h, a",	ld_h_a, 1, 4 },
	{"ld l, b",	ld_l_b, 1, 4 },
	{"ld l, c",	ld_l_c, 1, 4 },
	{"ld l, d",	ld_l_d, 1, 4 },
	{"ld l, e",	ld_l_e, 1, 4 },
	{"ld l, h",	ld_l_h, 1, 4 },
	{"ld l, l",	ld_l_l, 1, 4 },
	{"ld l, (hl)", ld_l_phl, 1, 8 },
	{"ld l, a",	ld_l_a, 1, 4 },
	{"ld (hl), b",  ld_phl_b, 1, 8 },
	{"ld (hl), c",  ld_phl_c, 1, 8 },
	{"ld (hl), d",  ld_phl_d, 1, 8 },
	{"ld (hl), e",  ld_phl_e, 1, 8 },
	{"ld (hl), h",  ld_phl_h, 1, 8 },
	{"ld (hl), l",  ld_phl_l, 1, 8 },
	{"halt", halt_cpu, 1, 4 },
	{"ld (hl), a",	ld_phl_a, 1, 8 },
	{"ld a, b",	ld_a_b, 1, 4 },
	{"ld a, c",	ld_a_c, 1, 4 },
	{"ld a, d",	ld_a_d, 1, 4 },
	{"ld a, e",	ld_a_e, 1, 4 },
	{"ld a, h",	ld_a_h, 1, 4 },
	{"ld a, l",	ld_a_l, 1, 4 },
	{"ld a, (hl)", ld_a_phl, 1, 8 },
	{"ld a, a",	ld_a_a, 1, 4 },
	{"add a, b",	add_a_b, 1, 4 },
	{"add a, c",	add_a_c, 1, 4 },
	{"add a, d",	add_a_d, 1, 4 },
	{"add a, e",	add_a_e, 1, 4 },
	{"add a, h",	add_a_h, 1, 4 },
	{"add a, l",	add_a_l, 1, 4 },
	{"add a, (hl)", add_a_phl, 1, 8 },
	{"add a, a",	add_a_a, 1, 4 },
	{"adc a, b",	adc_a_b, 1, 4 },
	{"adc a, c",	adc_a_c, 1, 4 },
	{"adc a, d",	adc_a_d, 1, 4 },
	{"adc a, e",	adc_a_e, 1, 4 },
	{"adc a, h",	adc_a_h, 1, 4 },
	{"adc a, l",	adc_a_l, 1, 4 },
	{"adc a, (hl)", adc_a_phl, 1, 8 },
	{"adc a, a",	adc_a_a, 1, 4 },
	{"sub b",	sub_b, 1, 4 },
	{"sub c",	sub_c, 1, 4 },
	{"sub d",	sub_d, 1, 4 },
	{"sub e",	sub_e, 1, 4 },
	{"sub h",	sub_h, 1, 4 },
	{"sub l",	sub_l, 1, 4 },
	{"sub (hl)", sub_phl, 1, 8},
	{"sub a",	sub_a, 1, 4 },
	{"sbc a, b",	sbc_a_b, 1, 4 },
	{"sbc a, c",	sbc_a_c, 1, 4 },
	{"sbc a, d",	sbc_a_d, 1, 4 },
	{"sbc a, e",	sbc_a_e, 1, 4 },
	{"sbc a, h",	sbc_a_h, 1, 4 },
	{"sbc a, l",	sbc_a_l, 1, 4 },
	{"sbc a, (hl)", sbc_a_phl, 1, 8 },
	{"sbc a, a",	sbc_a_a, 1, 4 },
	{"and b",	and_b, 1, 4 },
	{"and c",	and_c, 1, 4 },
	{"and d",	and_d, 1, 4 },
	{"and e",	and_e, 1, 4 },
	{"and h",	and_h, 1, 4 },
	{"and l",	and_l, 1, 4 },
	{"and (hl)", and_phl, 1, 8 },
	{"and a",	and_a, 1, 4 },
	{"xor b",	xor_b, 1, 4 },
	{"xor c",	xor_c, 1, 4 },
	{"xor d",	xor_d, 1, 4 },
	{"xor e",	xor_e, 1, 4 },
	{"xor h",	xor_h, 1, 4 },
	{"xor l",	xor_l, 1, 4 },
	{"xor (hl)", xor_phl, 1, 8},
	{"xor a",	xor_a, 1, 4 },
	{"or b",	or_b, 1, 4 },
	{"or c",	or_c, 1, 4 },
	{"or d",	or_d, 1, 4 },
	{"or e",	or_e, 1, 4 },
	{"or h",	or_h, 1, 4 },
	{"or l",	or_l, 1, 4 },
	{"or (hl)", or_phl, 1, 8 },
	{"or a",	or_a, 1, 4 },
	{"cp b",	cp_b, 1, 4 },
	{"cp c",	cp_c, 1, 4 },
	{"cp d",	cp_d, 1, 4 },
	{"cp e",	cp_e, 1, 4 },
	{"cp h",	cp_h, 1, 4 },
	{"cp l",	cp_l, 1, 4 },
	{"cp (hl)", cp_phl, 1, 8 },
	{"cp a",	cp_a, 1, 4 },
	{"ret nz", ret_nz, 0, 8},
	{"pop bc", pop_bc, 1, 12},
	{"jp nz, 0x%x", jp_nz_nn, 0, 12},
	{"jp nn", jp_nn, 0, 12},
	{"call nz, 0x%x", call_nz_nn, 0, 12},
	{"push bc", push_bc, 1, 16},
	{"add a, 0x%x", add_a_n, 2, 8},
	{"rst 0x00", rst_0x00, 0, 32}, /* rst is actually 1 byte but the PC is never incremented because of it */
	{"ret z", ret_z, 0, 8},
	{"ret", ret_unconditional, 0, 8},
	{"jp z, 0x%x", jp_z_nn, 0, 12},
	{"bit instruction", bit_instruction, 2, 8},
	{"call z, 0x%x", call_z_nn, 0, 12},
	{"call 0x%x", call_nn, 0, 12},
	{"adc a, 0x%x", adc_a_n, 2, 8},
	{"rst 0x08", rst_0x08, 0, 32},
	{"ret nc", ret_nc, 0, 8},
	{"pop de", pop_de, 1, 12},
	{"jp nc, 0x%x", jp_nc_nn, 0, 12},
	{"0xD3 nop", nop, 1, 0},
	{"call nc, 0x%x", call_nc_nn, 0, 12},
	{"push de", push_de, 1, 16},
	{"sub 0x%x", sub_n, 2, 8},
	{"rst 0x10", rst_0x10, 0, 32},
	{"ret c", ret_c, 0, 8},
	{"reti", reti, 0, 8},
	{"jp c, 0x%x", jp_c_nn, 0, 12},
	{"0xDB undefined", undef_instr, 0, 0},
	{"call c, 0x%x", call_c_nn, 0, 12},
	{"0xDD undefined", undef_instr, 0, 0},
	{"sbc a, 0x%x", sbc_a_n, 2, 8},
	{"rst 0x18", rst_0x18, 0, 32},
	{"ld (0xFF00 + 0x%x), a", ld_zpg_a, 2, 12},
	{"pop hl", pop_hl, 1, 12},
	{"ld (c), a", ld_pc_a, 1, 8},
	{"0xE3 nop", nop, 1, 0},
	{"0xE4 undefined", undef_instr, 0, 0},
	{"push hl", push_hl, 1, 16},
	{"and 0x%x", and_n, 2, 8},
	{"rst 0x20", rst_0x20, 0, 32},
	{"add sp, 0x%x", add_sp_n, 2, 16},
	{"jp (hl)", jp_hl, 0, 4},
	{"ld (0x%x), a", ld_pnn_a, 3, 16},
	{"0xEB undefined", undef_instr, 0, 0},
	{"0xEC undefined", undef_instr, 0, 0},
	{"0xED undefined", undef_instr, 0, 0},
	{"xor 0x%x", xor_n, 2, 8},
	{"rst 0x28", rst_0x28, 0, 32},
	{"ld a, (0xFF00 + 0x%x)", ld_a_zpg, 2, 12},
	{"pop af", pop_af, 1, 12},
	{"ld a, (c)", ld_a_pc, 1, 8},
	{"di", di, 1, 4},
	{"0xF4 undefined", undef_instr, 0, 0},
	{"push af", push_af, 1, 16},
	{"or 0x%x", or_n, 2, 8},
	{"rst 0x30", rst_0x30, 0, 32},
	{"ld hl, sp", ld_hl_sp, 2, 12},
	{"ld sp, hl", ld_sp_hl, 1, 8},
	{"ld a, (0x%x)", ld_a_pnn, 3, 16},
	{"ei", ei, 1, 4},
	{"0xFC undefined", undef_instr, 0, 0},
	{"0xFD undefined", undef_instr, 0, 0},
	{"cp 0x%x", cp_n, 2, 8},
	{"rst 0x38", rst_0x38, 0, 32},
};
