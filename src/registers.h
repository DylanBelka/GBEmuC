#ifndef GBEMC_REGISTERS_H
#define GBEMC_REGISTERS_H

#include "common.h"

struct registers
{
	union
	{
		struct
		{
			struct
			{
				union
				{
					struct
					{
						u8 _n_b0 : 1;
						u8 _n_b1 : 1;
						u8 _n_b2 : 1;
						u8 _n_b3 : 1;
						u8 C : 1;
						u8 H : 1;
						u8 N : 1;
						u8 Z : 1;
					};
					u8 raw;
				};
			} F;
			u8 A;
		};
		u16 AF;
	};
	union
	{
		struct
		{
			u8 C;
			u8 B;
		};
		u16 BC;
	};
	union
	{
		struct
		{
			u8 E;
			u8 D;
		};
		u16 DE;
	};
	union
	{
		struct
		{
			u8 L;
			u8 H;
		};
		u16 HL;
	};

	u16 SP;
	u16 PC;
};

extern struct registers registers;

#endif /* GBEMC_REGISTERS_H */
