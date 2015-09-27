#ifndef GBEMC_MEM_REGS_H
#define GBEMC_MEM_REGS_H

/* Locations of important locations in memory */
/* $0000-$00FF - Restart and Interrupt Vector Table */
#define RESTART 0x00
#define IRV 0x00
#define RESTART_END 0x00FF
#define IRV_END 0x00FF

/* $0100-$014F - Cartridge Header Area */
#define PROGRAM_START 0x100 /* program(PC) starts here */
/* Nintendo logo */
#define NINTEN_LOGO 0x104
#define NINTEN_LOGO_END 0x0133
/* Game title */
#define TITLE 0x0134
#define TITLE_END 0x013E

#define GAME_DES 0x013F
#define GAME_DES_END 0x0142
/* Is it a GBC Game? */
#define GBC_COMPAT 0x0143
/* Game license code */
#define NEW_LICENSE_CODE 0x0144
#define NEW_LICENSE_CODE_END 0x0145
/* Is it compatable with the Super Gameboy? */
#define SGB_COMPAT 0x0146
/* Cartridge type */
#define CART_TYPE 0x0147
/* Rom size */
#define CART_ROM_SIZE 0x0148
/* External RAM size */
#define CART_RAM_SIZE 0x0149
/* Game destination/ region code */
#define DEST_CODE 0x014A
/* Old license code */
#define OLD_LICENSE_CODE 0x014B
/* Mask ROM version/ type */
#define MASK_ROM_VERSION 0x014C
/* Complement checksum */
#define COMPL_CHECKSUM 0x014D
/* Checksum */
#define CHECKSUM 0x014E
#define CHECKSUM_END 0x014F

/* $0150-$3FFF - ROM Bank 0 */
#define ROM_BANK_0 0x0150
#define ROM_BANK_0_END 0x03FFF

/* $4000-$7FFF - ROM Bank n */
#define ROM_BANK_N 0x4000
#define ROM_BANK_N_END 0x07FFF

#define INTERNAL_MEM 0x8000
#define INTERNAL_MEM_END 0x10000

/* $8000-$97FF - Character RAM (tile RAM) */
#define CHARACTER_RAM 0x8000
#define CHARACTER_RAM_END 0x97FF

#define CHR_MAP 0x8000
#define CHR_MAP_END 0x97FF

#define CHR_MAP_UNSIGNED 0x8000
#define CHR_MAP_UNSIGNED_END 0x8FFF

#define CHR_MAP_SIGNED 0x8800
#define CHR_MAP_SIGNED_END 0x97FF

/* $9800-$9BFF - BG Map Data 0 */
#define BG_MAP_0 0x9800
#define BG_MAP_0_END 0x9BFF

/* $9C00-$9FFF - BG Map Data 1 */
#define BG_MAP_1 0x9C00
#define BG_MAP_1_END 0x9FFF

/* $A000-$BFFF - External (cartridge) RAM */
#define CART_RAM 0xA000
#define CART_RAM_END 0xBFFF

/* $C000-$DFFF - Internal Work RAM */
#define WORK_RAM 0xC000
#define WORK_RAM_END 0xDFFF

/* $E000-$FDFF - Reserved Area/Echo RAM */
#define RES_RAM 0xE000
#define RES_RAM_END 0xFDFF

/* $FE00-$FE9F - Object Attribute Memory (OAM) */
#define OAM 0xFE00
#define OAM_END 0xFE9F

/* $FF00-$FF7F - Hardware I/O Registers */
#define JOYPAD 0xFF00

/* Memory locations of various memory registers */

/* timer registers */
#define DIV	 0xFF04
#define TIMA 0xFF05
#define TMA	 0xFF06
#define TAC  0xFF07

/* sound registers */
#define NR10 0xFF10
#define NR11 0xFF11
#define NR12 0xFF12
#define NR14 0xFF14
#define NR21 0xFF16
#define NR22 0xFF17
#define NR24 0xFF19
#define NR30 0xFF1A
#define NR31 0xFF1B
#define NR32 0xFF1C
#define NR33 0xFF1E
#define NR41 0xFF20
#define NR42 0xFF21
#define NR43 0xFF22
#define NR50 0xFF24
#define NR51 0xFF25
#define NR52 0xFF26

/* display control registers */
#define LCDC 0xFF40 /* lcd control */
#define STAT 0xFF41 /* lcd status */
#define SCY	 0xFF42 /* background scroll y */
#define SCX	 0xFF43 /* background scroll x */
#define LY	 0xFF44 /* lcd y coordinate, Values range from 0->153. 144->153 is the VBlank period. */
#define LYC	 0xFF45 /* ly compare */
#define DMA	 0xFF46 /* DMA transfer */
#define BGP	 0xFF47 /* palette data */
#define OBP0 0xFF48 /* object palette data 0 */
#define OBP1 0xFF49 /* object palette data 1 */
#define WY	 0xFF4A /* window x position */
#define WX	 0xFF4B /* window y position */

#define IF 0xFF0F /* interrupt flag */
#define IE 0xFFFF /* interrupt enable */

/* $FF80-$FFFE - High RAM Area */
#define HIGH_RAM	 0xFF80
#define HIGH_RAM_END 0xFFFE

/* $FFFF - Interrupt Enable Register */
#define IER 0xFFFF /* IE */

#endif /* GBEMC_MEM_REGS_H */
