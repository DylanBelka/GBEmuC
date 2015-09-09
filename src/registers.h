#ifndef GBEMC_REGISTERS_H
#define GBEMC_REGISTERS_H

#include "common.h"

struct registers
{
	struct
	{
		union
		{
			struct
			{
				struct
				{
					u8 C : 1;
					u8 N : 1;
					u8 P : 1;
					u8 F3 : 1;
					u8 H : 1;
					u8 F5 : 1;
					u8 Z : 1;
					u8 S : 1;
				} F;
				u8 A;
			};
			u16 AF;
		};
	};
	struct
	{
		union
		{
			struct
			{
				u8 C;
				u8 B;
			};
			u16 BC;
		};
	};
	struct
	{
		union 
		{
			struct
			{
				u8 E;
				u8 D;
			};
			u16 DE;
		};
	};
	struct
	{
		union
		{
			struct
			{
				u8 L;
				u8 H;
			};
			u16 HL;
		};
	};

	u16 SP;
	u16 PC;
} extern registers;

#endif // GBEMC_REGISTERS_H