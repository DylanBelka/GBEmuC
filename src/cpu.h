#ifndef GBEMC_CPU_H
#define GBEMC_CPU_H

#include "common.h"

void reset_cpu(void);
void cpu_tick(void);

void dump_cpu(void);
void cpu_test(void);

struct cpu
{
    bool IME;
    bool is_stopped;
    bool is_halted;
    u16 clock_cycles;
};

extern struct cpu cpu;

#endif /* GBEMC_CPU_H */
