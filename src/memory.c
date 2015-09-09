#include "memory.h"
#include "registers.h"

 u8 rom[0x4000];
 u8 banked_rom[0x4000];
 u8 vram[0x2000];
 u8 banked_ram[0x2000];
 u8 wram[0x2000];
 u8 echo_ram[0x2000];
 u8 oam[0x100];
 u8 hram[0x80];

u8 read_byte(u16 addr)
{
	if (addr >= 0x0000 && addr < 0x4000) // ROM bank 0
	{
		return rom[addr - 0x0000];
	}
	else if (addr >= 0x4000 && addr < 0x8000) // ROM bank n
	{
		return rom[addr - 0x4000];
	}
	else if (addr >= 0x8000 && addr < 0xA000) // vram
	{
		return vram[addr - 0x8000];
	}
	else if (addr >= 0xA000 && addr < 0xC000) // RAM bank
	{
		return banked_ram[addr - 0xA000];
	}
	else if (addr >= 0xC000 && addr < 0xE000) // internal (work) ram
	{
		return wram[addr - 0xC000];
	}
	else if (addr > 0xE000 && addr < 0xFE00) // echo of wram
	{
		return echo_ram[addr - 0xE000];
	}
	else if (addr >= 0xFE00 && addr < 0xFEA0) // oam
	{
		return oam[addr - 0xFE00];
	}
	else if (addr >= 0xFF00 && addr <= 0xFFFF) // hram
	{
		return hram[addr - 0xFF00];
	}
}

u16 read_word(u16 addr)
{
	u16 ret = read_byte(addr);
	ret |= read_byte(addr + 1) << 0x8;
	return ret;
}

void write_byte(u16 addr, u8 val)
{
	if (addr >= 0x0000 && addr < 0x4000) // ROM bank 0
	{
		// memory bank controllers
		printf("mbc write at %d with value %d", addr, val);
	}
	else if (addr >= 0x4000 && addr < 0x8000) // ROM bank n
	{
		// more memory bank controllers
		printf("mbc write at %d with value %d", addr, val);
	}
	else if (addr >= 0x8000 && addr < 0xA000) // vram
	{
		vram[addr - 0x8000] = val;
	}
	else if (addr >= 0xA000 && addr < 0xC000) // RAM bank
	{
		banked_ram[addr - 0xA000] = val;
		printf("write to banked ram at %d with value %d", addr, val);
	}
	else if (addr >= 0xC000 && addr < 0xE000) // internal (work) ram
	{
		wram[addr - 0xC000] = val;
		// emulate echo of wram
		echo_ram[(addr + 0x2000) - 0xC000] = val;
	}
	else if (addr > 0xE000 && addr < 0xFE00) // echo of wram
	{
		echo_ram[addr - 0xE000] = val;
		wram[(addr - 0x2000) - 0xE000] = val;
	}
	else if (addr >= 0xFE00 && addr < 0xFEA0) // oam
	{
		oam[addr - 0xFE00] = val;
	}
	else if (addr >= 0xFF00 && addr <= 0xFFFF) // hram
	{
		hram[addr - 0xFF00] = val;
	}
}

void write_word(u16 addr, u16 val)
{
	write_byte(addr, val & 0xFF);
	write_byte(addr + 1, val >> 0x8);
}

void dma(void)
{
	u16 dmaStart = registers.A << 0x8; // get the location that the DMA will be copying from
	for (int i = 0; i < 0x8C; i++) // copy the 0x8C bytes from dmaStart to the OAM
	{
		write_byte(0xFE00 + i, read_byte(dmaStart + i));
	}
}