#include <stdlib.h>
#include <stdio.h>

#include "cart.h"
#include "memdefs.h"
#include "common.h"

struct cart_info cart_info;

struct cart_info fill_cart_info(void)
{
    struct cart_info ret;
    // read through the header and set up rom
	ret.is_banked = true; // default to banked (less cases)

    u8 cart_type = read_byte(CART_TYPE);
    if (cart_type >= 0x1 && cart_type <= 0x3)
	{
		ret.type = MBC1;
	}
	else if (cart_type >= 0x5 && cart_type <= 0x6)
	{
		ret.type = MBC2;
	}
	else if ((cart_type >= 0x8 && cart_type <= 0x9) || cart_type == 0x0)
	{
		ret.type = no_MBC;
		ret.is_banked = false;
	}
	else if (cart_type >= 0xB && cart_type <= 0xD)
	{
		ret.type = MM01; // im not sure if this is a real mbc
	}
	else if (cart_type >= 0xF && cart_type <= 0x13)
	{
		ret.type = MBC3;
	}
	else if (cart_type >= 0x19 && cart_type <= 0x1E)
	{
		ret.type = MBC5;
	}
	else
	{
		printf("unknown mbc type 0x%x\n", cart_type);
	}

	// set battery
    switch (cart_type)
    {
		case 0x3 : case 0x6 : case 0x9 : case 0xD : case 0xF :
		case 0x10: case 0x13: case 0x1B: case 0x1E:
			ret.has_battery = true; break;
        default:
			ret.has_battery = false; break;
    }

    // has internal ram
    switch (cart_type)
    {
		case 0x2 : case 0x3 : case 0x8 : case 0x9 : case 0x10 :
		case 0x12: case 0x13: case 0x1A: case 0x1B:
			ret.has_ram = true; break;
		default:
            ret.has_ram = false; break;
    }

    // has internal timer
    switch (cart_type)
    {
	case 0xF : case 0x10:
		ret.has_timer = true; break;
	default:
		ret.has_timer = false; break;
    }

    // number of rom banks
    // does not include the fixed rom bank at 0x0000-0x3FFFF
    u8 rom_size = read_byte(CART_ROM_SIZE);
    if (rom_size < 52)
	{
		// number of banks for rom sizes 0-6
		// are just num_banks = (1 << (rom_size + 1)) - 1
        ret.num_rom_banks = (1 << (rom_size + 1)) - 1;
	}
	else
	{
		switch (rom_size)
		{
		case 0x52:
			ret.num_rom_banks = 71;
			break;
		case 0x53:
			ret.num_rom_banks = 79;
			break;
		case 0x54:
			ret.num_rom_banks = 95;
		}
	}

	// initialize banked rom
	printf("num rom banks %d\n", ret.num_rom_banks);

    banked_roms = malloc(sizeof(*banked_roms) * ret.num_rom_banks);
    for (int i = 0; i < ret.num_rom_banks; i++)
	{
        for (int j = 0; j < 0x4000; j++)
		{
			banked_roms[i][j] = 0;
		}
	}

	// number of ram banks
	switch (read_byte(CART_RAM_SIZE))
	{
	case 0x0:
		ret.num_ram_banks = 0;
	case 0x1: case 0x2:
		ret.num_ram_banks = 1;
		break;
	case 0x3:
		ret.num_ram_banks = 4;
		break;
	case 0x4:
		ret.num_ram_banks = 16;
		break;
	}

    cart_info.current_ram_bank = 0;
	cart_info.current_rom_bank = 0;
	cart_info.mem_model = ROM16Mbit_RAM8Kbyte;
	cart_info.address_lines = 0;

	return ret;
}

void init_cart(void)
{
    cart_info = fill_cart_info();

	// initialize banked ram
	banked_rams = malloc(sizeof(*banked_rams) * cart_info.num_ram_banks);
	for (int i = 0; i < cart_info.num_ram_banks; i++)
	{
		for (int j = 0; j < 0x2000; j++)
		{
			banked_rams[i][j] = 0;
		}
	}

	printf("cart initialized succesfully\n");
	printf("mbc type %d\n", cart_info.type);
	printf("ram banks %d\n", cart_info.num_ram_banks);
}

void mbc_write_load_rom(int addr, u8 val)
{
    if (addr % 0x4000 == 0) // does it cross the rom boundary? (ex: going from bank 2 to bank 3)
	{
		cart_info.current_rom_bank++;
		printf("boundary crossed addr = 0x%x bank = %d\n", addr, cart_info.current_rom_bank - 1);
	}
	addr -= (0x4000 * (cart_info.current_rom_bank + 1));
    banked_roms[cart_info.current_rom_bank][addr] = val;
}

void no_MBC_write(u16 addr, u8 val)
{
    printf("no mbc write\n");
    getchar();
}

u8* get_byte_no_mbc(u16 addr)
{
    return &banked_roms[0][addr];
}

void mbc1_write(u16 addr, u8 val)
{
	printf("mbc1 write addr = 0x%x val = 0x%x\n", addr, val);
	getchar();
	if (addr >= 0x6000 && addr <= 0x7FFF) // set memory model
	{
        cart_info.mem_model = (val & 0x1) ? ROM4Mbit_RAM32Kbyte : ROM16Mbit_RAM8Kbyte;
	}
    else if (addr >= 0x2000 && addr <= 0x3FFF) // select rom bank
	{
        if ((val & 0x1F) == 0 || (val & 0x1F) == 1) // both set bank 0
		{
            cart_info.current_rom_bank = 0;
		}
		else
		{
			/*
			 * According to documentation, bank 0 is the fixed bank from 0x0000-0x3FFF
             * but bank 0 as an index to banked_roms is actually the first official bank
             * ie bank 1 in documentation
             * so by subtracting 2 from the value, we get the correct bank in code as an index
             */
			cart_info.current_rom_bank = (val & 0x1F) - 2;
		}
	}
	else if (addr >= 0x4000 && addr < 0x5FFF) // select ram bank
	{
		if (cart_info.mem_model == ROM4Mbit_RAM32Kbyte)
		{
            cart_info.current_ram_bank = (val & 0x3) - 1;
		}
		else // ROM16Mbit_RAM8Kbyte
		{
			printf("set rom address lines addr = 0x%x val = 0x%x\n", addr, val);
		}
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF) // enable ram bank
	{
        // just ignore it and always have ram banks enabled
	}
}

u8* get_rom_byte_mbc1(u16 addr)
{
	return &banked_roms[cart_info.current_rom_bank][addr];
}

void mbc_write(u16 addr, u8 val)
{
	if (cart_info.type == no_MBC)
	{
		no_MBC_write(addr, val);
	}
	if (cart_info.type == MBC1)
	{
		mbc1_write(addr, val);
	}
}

u8* get_rom_byte_mbc(u16 addr)
{
	addr -= 0x4000;
	if (cart_info.type == no_MBC)
	{
        return get_byte_no_mbc(addr);
	}
	if (cart_info.type == MBC1)
	{
		return get_rom_byte_mbc1(addr);
	}
	else
	{
		printf("not mbc1\n");
		return NULL;
	}
}

void mbc_write_ram(u16 addr, u8 val)
{

}

u8* get_ram_byte_mbc(u16 addr)
{
    return NULL;
}
