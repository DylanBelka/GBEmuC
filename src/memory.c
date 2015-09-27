#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "registers.h"

u8 rom[0x4000];
u8 banked_rom[0x4000];
u8 vram[0x2000];
u8 banked_ram[0x2000];
u8 wram[0x2000];
u8 echo_ram[0x2000];
u8 oam[0xA0];
u8 hio[0x80];
u8 hram[0x80];

void reset_mem(void)
{
	#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
	unsigned int i;
	for (i = 0; i < NELEMS(rom); i++)
	{
		rom[i] = 0;
	}
	for (i = 0; i < NELEMS(banked_rom); i++)
	{
		banked_rom[i] = 0;
	}
	for (i = 0; i < NELEMS(vram); i++)
	{
		vram[i] = 0;
	}
	for (i = 0; i < NELEMS(banked_ram); i++)
	{
		banked_ram[i] = 0;
	}
	for (i = 0; i < NELEMS(wram); i++)
	{
		wram[i] = 0;
	}
	for (i = 0; i < NELEMS(echo_ram); i++)
	{
		echo_ram[i] = 0;
	}
	for (i = 0; i < NELEMS(oam); i++)
	{
		oam[i] = 0;
	}
	for (i = 0; i < NELEMS(hio); i++)
	{
		hio[i] = 0;
	}
	for (i = 0; i < NELEMS(hram); i++)
	{
		hram[i] = 0;
	}
	#undef NELEMS
}

void write_byte_load_rom(u16 addr, u8 val)
{
	if (addr < 0x4000) /* ROM bank 0 */
	{
		rom[addr - 0x0000] = val;
	}
	else if (addr >= 0x4000 && addr < 0x8000) /* ROM bank n */
	{
		banked_rom[addr - 0x4000] = val;
	}
	else
	{
		printf("write_byte_load_rom at 0x%x to addr: 0x%x with value: 0x%x", registers.PC, addr, val);
	}
}

void load_rom(char *rom_name)
{
	FILE *fp;
	char c;
	int i = 0;
	int size;
	fp = fopen(rom_name, "rb");
	if (fp == NULL)
	{
		printf("Cannot open file %s", rom_name);
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	printf("ROM size 0x%x\n", size);

	while (fscanf(fp, "%c", &c) != EOF && i < size)
	{
		write_byte_load_rom(i, c);
		i++;
	}
	fclose(fp);
}

u8 read_byte(u16 addr)
{
	if (addr < 0x4000) /* ROM bank 0 */
	{
		return rom[addr - 0x0000];
	}
	else if (addr >= 0x4000 && addr < 0x8000) /* ROM bank n */
	{
		return banked_rom[addr - 0x4000];
	}
	else if (addr >= 0x8000 && addr < 0xA000) /* vram */
	{
		return vram[addr - 0x8000];
	}
	else if (addr >= 0xA000 && addr < 0xC000) /* RAM bank */
	{
		return banked_ram[addr - 0xA000];
	}
	else if (addr >= 0xC000 && addr < 0xE000) /* internal (work) ram */
	{
		return wram[addr - 0xC000];
	}
	else if (addr >= 0xE000 && addr < 0xFE00) /* echo of wram */
	{
		printf("read from echo ram pc = 0x%x addr = 0x%x\n", registers.PC, addr);
		getchar();
		return echo_ram[addr - 0xE000];
	}
	else if (addr >= 0xFE00 && addr < 0xFEA0) /* oam */
	{
		return oam[addr - 0xFE00];
	}
	else if (addr >= 0xFEA0 && addr < 0xFF00) /* unusable memory */
	{
		printf("read to unusable memory at 0x%x addr = 0x%x", registers.PC, addr);
		getchar();
		return 0;
	}
	else if (addr >= 0xFF00 && addr < 0xFF4C) /* hardware io registers */
	{
		return hio[addr - 0xFF00];
	}
	else if (addr >= 0xFF4C && addr < 0xFF80) /* empty, but unusable for io */
	{
		printf("read to unusable memory at 0x%x addr = 0x%x", registers.PC, addr);
		getchar();
		return 0;
	}
	else if (addr >= 0xFF80 && addr <= 0xFFFF) /* hram */
	{
		return hram[addr - 0xFF80];
	}
	else
	{
		printf("read_byte at 0x%x to addr: 0x%x\n", registers.PC, addr);
		getchar();
		return 0;
	}
}

u16 read_word(u16 addr)
{
	return ((read_byte(addr + 1) << 0x8) | (read_byte(addr) & 0xFF));
}

void write_byte(u16 addr, u8 val)
{
	if (addr < 0x4000) /* ROM bank 0 */
	{
		/* memory bank controllers */
		printf("mbc write at 0x%x addr = 0x%x val =  0x%x instr = 0x%x\n", registers.PC, addr, val, read_byte(registers.PC));
		getchar();
	}
	else if (addr >= 0x4000 && addr < 0x8000) /* ROM bank n */
	{
		/* more memory bank controllers */
		printf("mbc write at 0x%x addr = 0x%x val =  0x%x\n", registers.PC, addr, val);
		getchar();
	}
	else if (addr >= 0x8000 && addr < 0xA000) /* vram */
	{
		vram[addr - 0x8000] = val;
	}
	else if (addr >= 0xA000 && addr < 0xC000) /* RAM bank */
	{
		banked_ram[addr - 0xA000] = val;
		printf("write to banked ram at 0x%x with value 0x%x PC = 0x%x\n", addr, val, registers.PC);
		getchar();
	}
	else if (addr >= 0xC000 && addr < 0xE000) /* internal (work) ram */
	{
		wram[addr - 0xC000] = val;
	}
	else if (addr >= 0xE000 && addr < 0xFE00) /* echo of wram */
	{
		printf("write to echo ram pc = 0x%x addr = 0x%x val = 0x%x\n", registers.PC, addr, val);
		getchar();
		echo_ram[addr - 0xE000] = val;
	}
	else if (addr >= 0xFE00 && addr < 0xFEA0) /* oam */
	{
		oam[addr - 0xFE00] = val;
	}
	else if (addr >= 0xFEA0 && addr < 0xFF00)
	{
		/* unusable memory */
	}
	else if (addr >= 0xFF00 && addr < 0xFF80) /* hardware io */
	{
		hio[addr - 0xFF00] = val;
	}
	else if (addr >= 0xFF80 && addr <= 0xFFFF) /* hram */
	{
		hram[addr - 0xFF80] = val;
	}
	else
	{
		printf("write_byte at 0x%x to addr: 0x%x with value: 0x%x", registers.PC, addr, val);
		getchar();
	}
}

void write_word(u16 addr, u16 val)
{
	write_byte(addr, val & 0xFF);
	write_byte(addr + 1, (val >> 0x8) & 0xFF);
}

u8* get_byte_ptr(u16 addr)
{
	if (addr < 0x4000) /* ROM bank 0 */
	{
		return &rom[addr - 0x0000];
	}
	else if (addr >= 0x4000 && addr < 0x8000) /* ROM bank n */
	{
		return &rom[addr - 0x4000];
	}
	else if (addr >= 0x8000 && addr < 0xA000) /* vram */
	{
		return &vram[addr - 0x8000];
	}
	else if (addr >= 0xA000 && addr < 0xC000) /* RAM bank */
	{
		return &banked_ram[addr - 0xA000];
	}
	else if (addr >= 0xC000 && addr < 0xE000) /* internal (work) ram */
	{
		return &wram[addr - 0xC000];
	}
	else if (addr > 0xE000 && addr < 0xFE00) /* echo of wram */
	{
		printf("get byte of echo ram pc = 0x%x addr = 0x%x\n", registers.PC, addr);
		getchar();
		return &echo_ram[addr - 0xE000];
	}
	else if (addr >= 0xFE00 && addr < 0xFEA0) /* oam */
	{
		return &oam[addr - 0xFE00];
	}
	else if (addr >= 0xFF00 && addr <= 0xFFFF) /* hram */
	{
		return &hram[addr - 0xFF00];
	}
	else
	{
		printf("get_byte_ptr at 0x%x to addr: 0x%x\n", registers.PC, addr);
		getchar();
		return NULL;
	}
}

void dma(void)
{
	u16 dma_start = registers.A << 0x8; /* get the location that the DMA will be copying from */
	for (int i = 0; i < 0x8C; i++)		/*  copy the 0x8C bytes from dma_start to the OAM */
	{
		write_byte(0xFE00 + i, read_byte(dma_start + i));
	}
}
