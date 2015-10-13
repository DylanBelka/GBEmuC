#ifndef GBEMC_COMMON_H
#define GBEMC_COMMON_H

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t u8;
typedef int8_t s8;

typedef uint16_t u16;
typedef int16_t s16;

typedef uint32_t u32;
typedef int32_t s32;

enum bits
{
	bit0 = 0x1,
	bit1 = 0x2,
	bit2 = 0x4,
	bit3 = 0x8,
	bit4 = 0x10,
	bit5 = 0x20,
	bit6 = 0x40,
	bit7 = 0x80
};

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

#endif /* GBEMC_COMMON_H */
