#include <stdio.h>

#include <GL/glew.h>

#include "gpu.h"
#include "memdefs.h"
#include "common.h"
#include "memory.h"
#include "emu.h"

u8 scanline;

static int num_draws = 0;

void render_full(void)
{
    /*
    if (num_draws > 1000 && FALSE)
    {
        int x, y, i;
        for (int i = CHR_MAP; i < CHR_MAP_END; i++)
        {
            printf("0x%x ", read_byte(i));
        }
        printf("\n");
        getchar();
    }
    */

    num_draws++;
    scanline = 0;
    write_byte(LY, scanline);
}

void draw_scanline(void)
{
    write_byte(LY, scanline);
    scanline++;
}
