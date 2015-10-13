#ifndef GBEMC_GPU_H
#define GBEMC_GPU_H

#include "common.h"

extern u8 scanline;

void render_full(void);
void draw_scanline(void);

#define BLACK		0x081820
#define WHITE		0xE0F8D0
#define LIGHT_GREY	0x88C070
#define DARK_GREY	0x346856

#endif // GBEMC_GPU_H
