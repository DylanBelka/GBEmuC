#ifndef GBEMC_CPU_H
#define GBEMC_CPU_H

#include "common.h"

void reset_cpu(void);
void cpu_tick(void);

void dump_cpu(void);
void cpu_test(void);

struct cpu
{
	bool IME;			// interrupt master enable
	bool is_stopped;	// cpu is stopped
	bool is_halted;		// cpu is halted/ low power mode
	u16 clock_cycles;	// current count of clock cycles
};

extern struct cpu cpu;

#endif /* GBEMC_CPU_H */
