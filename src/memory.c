#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "registers.h"
#include "emu.h"

u8 rom[0x4000];
u8 banked_rom[0x4000];
u8 vram[0x2000];
u8 banked_ram[0x2000];

u8 internal_mem[0x4000];

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
	for (i = 0; i < NELEMS(internal_mem); i++)
	{
		internal_mem[i] = 0;
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
	u8 *byte = get_byte(addr);
	if (addr == 0xFF00) // key input
	{
		if (key_info.colID == bit4)
		{
			return key_info.keys[p15] | key_info.colID | 0xC0; // set the upper (unused) bits with 0xC0
		}
		else if (key_info.colID == bit5)
		{
			return key_info.keys[p14] | key_info.colID | 0xC0;
		}
	}
	else if (addr == 0xff04) // div
	{
        return (u8)rand(); // todo: implement div
	}

	if (byte != NULL)
	{
		return *byte;
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
	u8 *byte = get_byte(addr);

	if (addr == 0xFF46) // dma
	{
		dma();
	}
	else if (addr == 0xFF00) // key input
	{
		key_info.colID = registers.A;
		return;
	}
	if (addr < 0x4000) /* ROM bank 0 */
	{
		/* memory bank controllers */
		printf("mbc write at 0x%x addr = 0x%x val =  0x%x instr = 0x%x\n", registers.PC, addr, val, read_byte(registers.PC));
		//getchar();
	}
	else if (addr >= 0x4000 && addr < 0x8000) /* ROM bank n */
	{
		/* more memory bank controllers */
		printf("mbc write at 0x%x addr = 0x%x val =  0x%x\n", registers.PC, addr, val);
		getchar();
	}
	else if (byte != NULL)
	{
		*byte = val;
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

u8* get_byte(u16 addr)
{
	if (addr < 0x4000) /* ROM bank 0 */
	{
		return &rom[addr - 0x0000];
	}
	else if (addr >= 0x4000 && addr < 0x8000) /* ROM bank n */
	{
		return &banked_rom[addr - 0x4000];
	}
	else if (addr >= 0x8000 && addr < 0xA000) /* vram */
	{
		return &vram[addr - 0x8000];
	}
	else if (addr >= 0xA000 && addr < 0xC000) /* RAM bank */
	{
		return &banked_ram[addr - 0xA000];
	}
	else if (addr >= 0xC000) /* internal memory */
	{
        return &internal_mem[addr - 0xC000];
	}
	else
	{
		printf("get_byte at 0x%x to addr: 0x%x\n", registers.PC, addr);
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
