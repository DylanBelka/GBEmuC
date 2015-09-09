#ifndef GBEMC_MEMORY_H
#define GBEMC_MEMORY_H

#include <stdio.h>

#include "common.h"

extern u8 rom[0x4000];
extern u8 banked_rom[0x4000];
extern u8 vram[0x2000];
extern u8 banked_ram[0x2000];
extern u8 wram[0x2000];
extern u8 echo_ram[0x2000];
extern u8 oam[0x100];
extern u8 hram[0x80];

u8 read_byte(u16 addr);
u16 read_word(u16 addr);

void write_byte(u16 addr, u8 val);
void write_word(u16 addr, u16 val);

void dma(void);

#endif // GBEMC_MEMORY_H