#include "cpu.h"
#include "common.h"
#include "registers.h"
#include "memory.h"
#include "instructions.h"
#include "memdefs.h"

struct cpu cpu;

void dump_cpu(void)
{
	#define r(reg) registers.reg
	printf("AF: 0x%x\nBC: 0x%x\nDE: 0x%x\nHL: 0x%x\nPC: 0x%x\nSP: 0x%x\n",
		r(AF), r(BC), r(DE), r(HL), r(PC), r(SP));
	#undef r
}

void cpu_test(void)
{

}

void reset_cpu(void)
{
	write_byte(TIMA, 0x00);
	write_byte(TMA, 0x00);
	write_byte(TAC, 0x00);
	write_byte(LCDC, 0x91);
	write_byte(SCY, 0x00);
	write_byte(SCX, 0x00);
	write_byte(LYC, 0x00);
	write_byte(BGP, 0xFC);
	write_byte(OBP0, 0xFC);
	write_byte(OBP1, 0xFC);
	write_byte(WY, 0x00);
	write_byte(WX, 0x00);
	write_byte(IE, 0x00);
	write_byte(LY, 0x94);

	registers.A = 0x01;
	registers.F.raw = 0xB0;
	registers.BC = 0x13;
	registers.DE = 0xD8;
	registers.HL = 0x14D;
	registers.SP = 0xFFFE;
	registers.PC = 0x100;
}

void interrupt(u16 addr)
{
	push(registers.PC);
	registers.PC = addr;
	cpu.IME = false;
	write_byte(IF, 0x0);
}

void handle_interrupts(void)
{
	u8 int_enable, int_flag;
	int_enable = read_byte(IE);
	int_flag = read_byte(IF);

	if (cpu.IME)
	{
		if ((int_enable & bit0) && (int_flag & bit0)) /* vblank */
		{
			interrupt(0x40);
		}
		else if ((int_enable & bit1) && (int_flag & bit1)) /* lcdc (stat) */
		{
			interrupt(0x48);
		}
		else if ((int_enable & bit2) && (int_flag & bit2)) /* timer overflow */
		{
			interrupt(0x50);
		}
		else if ((int_enable & bit3) && (int_flag & bit3)) /* serial link */
		{
			interrupt(0x58);
		}
		else if ((int_enable & bit4) && (int_flag & bit4)) /* joypad press */
		{
			interrupt(0x60);
		}
	}
}

void print_instruction(u8 instruction)
{
	if (instructions[instruction].opcode_length == 2)
	{
		printf(instructions[instruction].disassembly, read_byte(registers.PC + 1));
		printf("\tat 0x%x\n", registers.PC);
	}
	else if (instructions[instruction].opcode_length == 3)
	{
		printf(instructions[instruction].disassembly, read_word(registers.PC + 1));
		printf("\n(0x%x) = 0x%x", read_word(registers.PC + 1), read_byte(read_word(registers.PC + 1)));
		printf("\tat 0x%x\n", registers.PC);
	}
	else if (instructions[instruction].opcode_length == 0)
	{
		printf(instructions[instruction].disassembly, read_word(registers.PC + 1));
		printf("\tat 0x%x\n", registers.PC);
	}
	else
	{
		printf("%s\tat 0x%x\n\n", instructions[instruction].disassembly, registers.PC);
	}
}

void cpu_tick(void)
{
	handle_interrupts();
	u8 instruction = read_byte(registers.PC);

	/* update clock ticks */
	cpu.clock_cycles += instructions[instruction].ticks;

	/* execute the instruction */
	((void (*)(void))instructions[instruction].execute)();

	/* update PC */
	registers.PC += instructions[instruction].opcode_length;
}
