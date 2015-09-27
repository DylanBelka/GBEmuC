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
	printf("AF: %x\nBC: %x\nDE: %x\nHL: %x\n",
		r(AF), r(BC), r(DE), r(HL));
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

static int int_times = 0;

void handle_interrupts(void)
{
	u8 int_enable, int_flag;
	int_enable = read_byte(IE);
	int_flag = read_byte(IF);

	if (cpu.IME)
	{
		if ((int_enable & 0x1) && (int_flag & 0x1)) /* vblank */
		{
			printf("vblank PC = 0x%x\n", registers.PC);
			getchar();
			int_times++;
			interrupt(0x40);
		}
	}
}

/* Interrupting too early */

/* actually might be interrupting too late */
/* interrupting too few times */

static bool ww = false;

void cpu_tick(void)
{
	handle_interrupts();
	u8 instruction = read_byte(registers.PC);

	if (registers.PC == 0x27D2)
	{
		ww = true;
	}
	if (ww)
	{
		/* printf("%s\tat 0x%x\n\n", instructions[instruction].disassembly, registers.PC); */
	}

	/* update clock ticks */
	cpu.clock_cycles += instructions[instruction].ticks;

	/* execute the instruction */
	((void (*)(void))instructions[instruction].execute)();

	/* update PC */
	registers.PC += instructions[instruction].opcode_length;
}
