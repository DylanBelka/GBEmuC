#include "cpu.h"
#include "common.h"
#include "registers.h"
#include "memory.h"
#include "instructions.h"

void dump_cpu(void)
{
	#define r(reg) registers.reg
	printf("AF: %x\nBC: %x\nDE: %x\nHL: %x", 
		r(AF), r(BC), r(DE), r(HL));
	#undef r
}

void cpu_test(void)
{

}

void cpu_tick(void)
{
	u8 instruction = read_byte(registers.PC);
#if 0
	if ((instruction & 0xFF) >= 0x40 && (instruction & 0xFF) <= 0xFB && (instruction & 0xFF) != 0x76)
	{
		u8 ops[] = { registers.B, registers.C, registers.D, registers.E, registers.H, registers.L, 
						read_byte(registers.HL), registers.A,
						registers.B, registers.C, registers.D, registers.E, registers.H, registers.L, 
						read_byte(registers.HL), registers.A };
		u8 op = ops[instruction & 0x0F];
		bool is_lower_instr = (instruction & 0x0F) < 0x08;
		
		switch (instruction & 0xF0)
		{
			case 0x40: case 0x50: case 0x60: case 0x70: // ld r, r
			{
				ld_r_r(instruction, op, is_lower_instr);
				break;
			}
			case 0x80:
			{
				if (is_lower_instr)
					add_a_r(op);
				else
					adc_a_r(op);
				break;
			}
			case 0x90:
			{
				if (is_lower_instr)
					sub_r(op);
				else
					sbc_a_r(op);
				break;
			}
			case 0xA0:
			{
				if (is_lower_instr)
					and_r(op);
				else
					xor_r(op);
				break;
			}
			case 0xB0:
			{
				if (is_lower_instr)
					or_r(op);
				else
					cp(op);
				break;
			}
		}
	}
	else
	{
		((void (*)(void))instructions[instruction].execute)();
	}
#endif
	// update PC
	registers.PC += instructions[instruction].opcode_length;
}