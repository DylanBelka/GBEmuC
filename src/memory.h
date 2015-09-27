#ifndef GBEMC_MEMORY_H
#define GBEMC_MEMORY_H

#include <stdio.h>

#include "common.h"

extern u8 rom[0x4000];			/* 0x0000-0x3FFF */
extern u8 banked_rom[0x4000];	/* 0x4000-0x7FFF */
extern u8 vram[0x2000];			/* 0x8000-0x9FFF */
extern u8 banked_ram[0x2000];	/* 0xA000-0xBFFF (if available) */

extern u8 internal_mem[0x4000]; /* 0xC000-0xFFFF */

void reset_mem(void);

void load_rom(char *rom_name);

u8 read_byte(u16 addr);
u16 read_word(u16 addr);

void write_byte(u16 addr, u8 val);
void write_word(u16 addr, u16 val);

u8* get_byte(u16 addr);

void dma(void);

#endif /* GBEMC_MEMORY_H */
