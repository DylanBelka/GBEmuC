#ifndef GBEMC_CART_H
#define GBEMC_CART_H

#include "common.h"
#include "memory.h"

enum MBC_type
{
	no_MBC, MBC1, MBC2, MBC3, MBC5, MM01
};

enum MBC1_memory_model
{
    ROM16Mbit_RAM8Kbyte,
    ROM4Mbit_RAM32Kbyte
};

enum carttypes
{
	ROM_ONLY = 0x00,
	ROM_MBC1,
	ROM_MBC1_RAM,
	ROM_MBC1_RAM_BATT,
	ROM_MBC2 = 0x05,
	ROM_MBC2_BATT,
	ROM_RAM = 0x08,
	ROM_RAM_BATT,
	ROM_MM01 = 0x0B,
	ROM_MM01_SRAM,
	ROM_MM01_SRAM_BATT,
	ROM_MBC3_TIMER_BATT = 0x0F,
	ROM_MBC3_TIMER_RAM_BATT,
	ROM_MBC3,
	ROM_MBC3_RAM,
	ROM_MBC3_RAM_BATT,
	ROM_MBC5 = 0x19,
	ROM_MBC5_RAM,
	ROM_MBC5_RAM_BATT,
	ROM_MBC5_RUMBLE,
	ROM_MBC5_RUMBLE_SRAM,
	ROM_MBC5_RUMLE_SRAM_BATT,
	POCKET_CAMERA,
	BANDAI_TAMA5 = 0x1F,
	HUDSON_HUC_3 = 0xFE,
	HUDSON_HUC3_1
};

struct cart_info
{
	// cart header data
    bool is_banked; 	// is the cart even banked?
    bool has_battery; 	// does the cart have an internal battery?
    bool has_ram;		// does the cart have internal extra ram?
    bool has_timer; 	// does the cart have an internal timer
	enum MBC_type type;
	int num_rom_banks;	// number of rom banks
	int num_ram_banks;	// number of ram banks

	// current cart info
    int current_rom_bank;
    int current_ram_bank;

    int address_lines;

    enum MBC1_memory_model mem_model;
};

extern struct cart_info cart_info;

void init_cart(void);

void mbc_write(u16 addr, u8 val);
u8* get_rom_byte_mbc(u16 addr);

u8* get_ram_byte_mbc(u16 addr);
void mbc_write_ram(u16 addr, u8 val);

void mbc_write_load_rom(int addr, u8 val);

#endif // GBEMC_CART_H
