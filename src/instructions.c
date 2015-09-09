#include "cpu.h"
#include "common.h"
#include "memory.h"
#include "registers.h"

void update_carry(const unsigned int val)
{
	if (val & 0xFF00)
		registers.F.C = 1;
	else
		registers.F.C = 0;
}

void update_halfcarry(const unsigned int val)
{
	if (val & 0xF0)
		registers.F.H = 1;
	else
		registers.F.H = 0;
}

void update_zero(const unsigned int val)
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

void cp(const u8 val)
{
	unsigned int result = registers.A - val;
	update_carry(result);
	update_halfcarry(result);
	update_zero(result);
	registers.F.N = 1;
}

void add(const u8 val)
{
	unsigned int result = registers.A + val;
	registers.A += val;

	update_carry(result);
	update_halfcarry(result);
	update_zero(result);
	registers.F.N = 0;
}

void adc(const u8 val)
{
	unsigned int result = registers.A + val + registers.F.C;
	registers.A += val + registers.F.C;

	update_carry(result);
	update_halfcarry(result);
	update_zero(result);
	registers.F.N = 0;
}

void sub(const u8 val)
{
	unsigned int result = registers.A - val;
	registers.A -= val;

	update_carry(result);
	update_halfcarry(result);
	update_zero(result);
}

void sbc(const u8 val)
{
	unsigned int result = registers.A - (val + registers.F.C);
	registers.A -= (val + registers.F.C);

	update_carry(result);
	update_halfcarry(result);
	update_zero(result);
}

void and(const u8 val)
{
	unsigned int result = registers.A & val;
	registers.A &= val;

	registers.F.C = 0;
	registers.F.H = 1;
	update_zero(result);
	registers.F.N = 0;
}

void xor(const u8 val)
{
	unsigned int result = registers.A ^ val;
	registers.A ^= val;

	registers.F.C = 0;
	registers.F.H = 0;
	update_zero(result);
	registers.F.N = 0;
}

void or(const u8 val)
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

/*
 * For the conditional instructions (ret, call jr, jp)
 * if the condition is true then the length of the opcode is subtracted from the PC
 * to adjust it back down
 * this is done because cpu_tick always adds the opcode length to the PC
*/

void ret(bool condition)
{
	if (condition)
	{
		pop(&registers.PC);
		registers.PC -= OPCODE_LENGTH();
	}
}

void call(bool condition)
{
	if (condition)
	{
		registers.SP--;
		write_byte(registers.SP, ((registers.PC + 3) & 0xFF));
		registers.SP--;
		write_byte(registers.SP, (((registers.PC + 3) >> 0x8) & 0xFF));
		registers.PC = read_word(registers.PC + 1);
		registers.PC -= OPCODE_LENGTH();
	}
}

void rst(const u8 dst)
{
	push(registers.PC + 1);
	registers.PC = dst;
}

void jr(bool condition)
{
	if (condition)
	{
		registers.PC += OPCODE_LENGTH() + read_byte(registers.PC + 1);
		registers.PC -= OPCODE_LENGTH();
	}
}

void jp(bool condition)
{
	if (condition)
	{
		registers.PC = read_word(registers.PC + 1);
		registers.PC -= OPCODE_LENGTH();
	}
}

void ld_r_r(u8 instruction, u8 src, bool is_lower_instr)
{
	#define P_HL NULL
	u8 *dst;
	int dst_index = ((instruction >> 0x4) & 0x0F) - 4;
	
	if (is_lower_instr)
	{
		u8 *dsts[] = { &registers.B, &registers.D, &registers.H, P_HL };
		dst = dsts[dst_index];
	}
	else
	{
		u8 *dsts[] = { &registers.C, &registers.E, &registers.L, &registers.A };
		dst = dsts[dst_index];
	}
	
	if (dst = P_HL) // ld (hl), r
	{
		write_byte(registers.HL, src);
		return;
	}
	
	*dst = src;
	#undef P_HL
}

#undef OPCODE_LENGTH

void nop(void) { }																	// 0x00
void ld_bc_nn(u16 nn) { registers.BC = nn; }										// 0x01
void ld_pbc_a(void) { write_byte(registers.BC, registers.A); }						// 0x02
void inc_bc(void) { registers.BC++; }												// 0x03
void inc_b(void) { inc(&registers.B); }												// 0x04
void dec_b(void) { dec(&registers.B); }												// 0x05
void ld_b_n(void) { registers.B = read_byte(registers.PC + 1); }					// 0x06
void rlca(void) { rlca_impl(); }													// 0x07
void ld_pnn_sp(void) { write_word(read_word(registers.PC + 1), registers.SP); } 	// 0x08
void add_hl_bc(void) { add16(&registers.HL, registers.BC); }						// 0x09
void ld_a_pbc(void) { registers.A = read_byte(registers.BC); }						// 0x0A
void dec_bc(void) { registers.BC--; }												// 0x0B
void inc_c(void) { inc(&registers.C); }												// 0x0C
void dec_c(void) { dec(&registers.C); }												// 0x0D
void ld_c_n(void) { registers.C = read_byte(registers.PC + 1); }					// 0x0E
void rrca(void) { rrca_impl(); }													// 0x0F
void stop(void) { cpu_stop = 1; }													// 0x10
void ld_de_nn(void) { registers.DE = read_word(registers.PC + 1); }					// 0x11
void ld_pde_a(void) { write_byte(registers.DE, registers.A); }						// 0x12
void inc_de(void) { registers.DE++; }												// 0x13
void inc_d(void) { inc(&registers.D); }												// 0x14
void dec_d(void) { dec(&registers.D); }												// 0x15
void ld_d_n(void) { registers.D = read_byte(registers.PC + 1); }					// 0x16
void rla(void) { rla_impl(); }														// 0x17
void jr_n(void) { jr(TRUE); }														// 0x18
void add_hl_de(void) { add16(&registers.HL, registers.DE); }						// 0x19
void ld_a_pde(void) { registers.A = read_byte(registers.DE); }						// 0x1A
void dec_de(void) { registers.DE--; }												// 0x1B
void inc_e(void) { inc(&registers.E); }												// 0x1C
void dec_e(void) { dec(&registers.E); }												// 0x1D
void ld_e_n(void) { registers.E = read_byte(registers.PC + 1); }					// 0x1E
void rra(void) { rra_impl(); }														// 0x1F
void jr_nz_n(void) { jr(!registers.F.Z); }											// 0x20
void ld_hl_nn(void) { registers.HL = read_word(registers.PC + 1); }					// 0x21
void ldi_phl_a(void) { write_byte(registers.HL, registers.A); registers.HL++; }		// 0x22
void inc_hl(void) { registers.HL++; }												// 0x23
void inc_h(void) { inc(&registers.H); }												// 0x24

const struct instruction instructions[256] = 
{
	// dummy instructions while testing
};

#if 0

const struct instruction instructions[256] = 
{
	{"nop", nop, 1},
	{"ld bc, nn", ld_bc_nn, 3},
	{"ld (bc), a", ld_pbc_a, 1},
	{"inc bc", inc_bc, 1},
	{"inc b", inc_b, 1},
	{"dec b", dec_b, 1},
	{"ld b, n", ld_b_n, 2},
	{"rlca", rlca, 1},
	{"ld (nn), sp", ld_pnn_sp, 3},
	{"add hl, bc", add_hl_bc, 1},
	{"ld a, (bc)", ld_a_pbc, 1},
	{"dec bc", dec_bc, 1},
	{"inc c", inc_c, 1},
	{"dec c", dec_c, 1},
	{"ld c, n", ld_c_n, 2},
	{"rrca", rrca, 1},
	{"stop", stop_cpu, 1},
	{"ld de, nn", ld_de_nn, 3},
	{"ld (de), a", ld_pde_a, 1},
	{"inc de", inc_de, 1},
	{"inc d", inc_d, 1},
	{"dec d", dec_d, 1},
	{"ld d, n", ld_d_n, 2},
	{"rla", rla, 1},
	{"jr n", jr_n, 2},
	{"add hl, de", add_hl_de, 1},
	{"ld a, (de)", ld_a_pde, 1},
	{"dec de", dec_de, 1},
	{"ld e, n", ld_e_n, 2},
	{"rra", rra, 1},
	{"jr nz, n", jr_nz_n, 2},
	{"ld hl, nn", ld_hl_nn, 3},
	{"ldi (hl), a", ldi_phl_a, 1},
	{"inc hl", inc_hl, 1},
	{"inc h", inc_h, 1},
	{"dec h", dec_h, 1},
	{"ld h, n", ld_h_n, 2},
	{"daa", daa, 1},
	{"jr z, n", jr_z_n, 2},
	{"add hl, hl", add_hl_hl, 1},
	{"ldi a, (hl)", ldi_a_phl, 1},
	{"dec hl", dec_hl, 1},
	{"inc l", inc_l, 1},
	{"dec l", dec_l, 1},
	{"ld l, n", ld_l_n, 2},
	{"cpl", cpl, 1},
	{"jr nc, n", jr_nc_n, 2},
	{"ld sp, nn", ld_sp_nn, 3},
	{"ldd (hl), a", ldd_phl_a, 1},
	{"inc sp", inc_sp, 1},
	{"inc (hl)", inc_phl, 1},
	{"dec (hl)", dec_phl, 1},
	{"ld (hl), n", ld_phl_n, 2},
	{"scf", scf, 1},
	{"jr c, n", jr_c_n, 2},
	{"add hl, sp", add_hl_sp, 1},
	{"ldd a, (hl)", ldd_a_phl, 1},
	{"dec sp", dec_sp, 1},
	{"inc a", inc_a, 1},
	{"dec a", dec_a, 1},
	{"ld a, n", ld_a_n, 2},
	{"ccf", ccf, 1},
	{"ld r, r", ld_r_r, 1}, // ld register, register (or ld without static operands) 0x40-0x7F not including 0x76 for halt
	{"halt", halt_cpu, 1},
	{"add a, r", add_a_r, 1},
	{"adc a, r", adc_a_r, 1},
	{"sub r", sub_r, 1},
	{"sbc a, r", sbc_a_r, 1},
	{"and r", and_r, 1},
	{"xor r", xor_r, 1},
	{"or r", or_r, 1},
	{"cp r", cp_r, 1},
	{"ret nz", ret_nz, 1},
	{"pop bc", pop_bc, 1},
	{"jp nz, nn", jp_nz_nn, 3},
	{"jp nn", jp_nn, 3},
	{"call nz, nn", call_nz_nn, 3},
	{"push bc", push_bc, 1},
	{"add a, n", add_a_n, 2},
	{"rst 0x00", rst_0x00, 0}, // rst is actually 1 byte but the PC is never incremented because of it
	{"ret z", ret_z, 1},
	{"ret", ret, 1},
	{"jp z, nn", jp_z_nn, 3},
	{"bit instruction", bit_instruction, 2},
	{"call z, nn", call_z_nn, 3},
	{"call nn", call_nn, 3},
	{"adc a, n", adc_a_n, 2},
	{"rst 0x08", rst_0x08, 0},
	{"ret nc", ret_nc, 1},
	{"pop de", pop_de, 1},
	{"jp nc, nn", jp_nc_nn, 3},
	{"0xD3 nop", nop, 1},
	{"call nc, nn", call_nc_nn, 3},
	{"push de", push_de, 1},
	{"sub n", sub_n, 2},
	{"rst 0x10", rst_0x10, 0},
	{"ret c", ret_c, 1},
	{"reti", reti, 1},
	{"jp c, nn", jp_c_nn, 3},
	{"0xDB undefined", undef_instr, 0}, 
	{"call c, nn", call_c_nn, 3},
	{"0xDD undefined", undef_instr, 0},
	{"sbc a, n", sbc_a_n, 2},
	{"rst 0x18", rst_0x18, 0},
	{"ld (0xFF00 + n), a", ld_pzpg_a, 2},
	{"pop hl", pop_hl, 1},
	{"ld (c), a", ld_pc_a, 1},
	{"0xE3 nop", nop, 1},
	{"0xE4 undefined", undef_instr, 0},
	{"push hl", push_hl, 1},
	{"and n", and_n, 2},
	{"rst 0x20", rst_0x20, 0},
	{"add sp, n", add_sp_n, 2},
	{"jp (hl)", jp_hl, 1}, // jp hl
	{"ld (nn), a", ld_pnn_a, 3},
	{"0xEB undefined", undef_instr, 0},
	{"0xEC undefined", undef_instr, 0},
	{"0xED undefined", undef_instr, 0},
	{"xor n", xor_n, 2},
	{"rst 0x28", rst_0x28, 0},
	{"ld a, (0xFF00 + n)", ld_a_zpg, 2},
	{"pop af", pop_af, 1},
	{"ld a, (c)", ld_a_cp, 1},
	{"di", di, 1},
	{"0xF4 undefined", undef_instr, 0},
	{"push af", push_af, 1},
	{"or n", or_n, 2},
	{"rst 0x30", rst_0x30, 0},
	{"ld hl, sp", ld_hl_sp, 1},
	{"ld sp, hl", ld_sp_hl, 1},
	{"ld a, (nn)", ld_a_pnn, 3},
	{"ei", ei, 1},
	{"0xFC undefined", undef_instr, 0},
	{"0xFD undefined", undef_instr, 0},
	{"cp n", cp_n, 2},
	{"rst 0x38", rst_0x38, 0}
};

#endif
