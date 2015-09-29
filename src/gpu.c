#include <stdio.h>
#include <stdlib.h>

//#include <GL/glew.h>

#include "gpu.h"
#include "memdefs.h"
#include "common.h"
#include "memory.h"
#include "emu.h"

u8 scanline;

// character map holds actual data
// Bit 4 - BG & Window Tile Data Select
// in unsigned mode, tiles range from [0-255]
// at 0x8000-0x8FFF

// in signed mode range [-127,128]
// at 0x8800-0x97FF

// bg maps hold index for character map entry
// Bit 3 - BG Tile Map Display Select
// bg1 0x9800-0x9BFF
// bg2 0x9C00-0x9FFF

void draw_slice(u8 b1, u8 b2, unsigned int *x, unsigned int *y)
{
    for (int i = 0x80; i >= 1; i >>= 1)
	{
        int curr_bit1 = b1 & i;
        int curr_bit2 = b2 & i;
        u32 *pixels = (u32*)window_surf->pixels;

        if (curr_bit1 && curr_bit2) // bit1 (on) and bit2 (on)
		{
			printf("x = %d y = %d\n", *x, *y);
			pixels[*y * window_surf->w + *x] = 0x081820;
		}
		else if (!curr_bit1 && !curr_bit2) // bit1 (off) and bit2 (off)
		{
			//pixels[*y * window_surf->w + *x] = 0xE0F8D0;
		}
		else if (curr_bit1 && !curr_bit2) // bit1 (on) and bit2 (off)
		{
			printf("x = %d y = %d\n", *x, *y);
			pixels[*y * window_surf->w + *x] = 0x88C070;
		}
		else // bit1 (off) bit2 (on)
		{
			printf("x = %d y = %d\n", *x, *y);
			pixels[*y * window_surf->w + *x] = 0x346856;
		}

		(*x)++;
	}
	*x -= 8;
	(*y)++;
}

void draw_background(void)
{
	unsigned int x = 0, y = 0;
	u8 lcdc = read_byte(LCDC);
	bool is_unsigned_chrs = (lcdc & bit4) ? true : false;
	u16 bg_map_data_start, bg_map_data_addr_end;

	if (lcdc & bit3) // background map data
	{
		bg_map_data_start = BG_MAP_1;
		bg_map_data_addr_end = BG_MAP_1_END;
	}
	else
	{
		bg_map_data_start = BG_MAP_0;
		bg_map_data_addr_end = BG_MAP_0_END;
	}
	for (int i = bg_map_data_start; i < bg_map_data_addr_end; i++)
	{
		u16 chr_loc_start;
		if (is_unsigned_chrs)
			chr_loc_start = read_byte(i) * 0x10 + CHR_MAP_UNSIGNED;
		else
			chr_loc_start = ((s8)read_byte(i)) * 0x10 + CHR_MAP_SIGNED;

		for (int j = chr_loc_start; j < chr_loc_start + 0x10; j += 2)
		{
			draw_slice(read_byte(j), read_byte(j + 1), &x, &y);
		}
		x += 8;
		y -= 8;
		if (x == VIRT_GB_WINDOW_WIDTH)
		{
			y += 8;
			x = 0;
		}
	}
}

void draw_sprites(void)
{
	u8 lcdc = read_byte(LCDC);
	bool is_8x16_sprite = (lcdc & bit2) ? true : false;
	for (int i = OAM; i < OAM_END; i += 4)
	{
		/* sprites are offset by (-8, -16)
		 * so a sprite at (0, 0) is offscreen and actually
		 * at (-8, 16)
		 */
        unsigned int y = (read_byte(i) & 0xFF) - 16;
        unsigned int x = (read_byte(i + 1) & 0xFF) - 8;
        u16 chr_loc_start = read_byte(i + 2) * 0x10 + CHR_MAP_UNSIGNED;
        for (int j = chr_loc_start; j < chr_loc_start + 0x10; j += 2)
		{
            draw_slice(read_byte(j), read_byte(j + 1), &x, &y);
		}
		if (is_8x16_sprite) // draw the second half of the sprite
		{
			chr_loc_start += 0x10; // second half of the sprite is located directly after the first
			for (int j = chr_loc_start; j < chr_loc_start + 0x10; j += 2)
			{
				draw_slice(read_byte(j), read_byte(j + 1), &x, &y);
			}
		}
	}
}

void draw_window(void)
{
    printf("drawing window\n");
}

void clear_surface(SDL_Surface *surf)
{
    SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 224, 248, 208));
}

void render_full(void)
{
	clear_surface(window_surf);
	u8 lcdc = read_byte(LCDC);
	if (lcdc & bit7) // is lcd on?
	{
        if (lcdc & bit0) // is background enabled?
		{
			draw_background();
		}
		if (lcdc & bit1) // are sprites enabled?
		{
			draw_sprites();
		}
		if (lcdc & bit5) // is window enabled?
		{
			draw_window();
		}
	}

	scanline = 0;
	write_byte(LY, scanline);
}

void draw_scanline(void)
{
	write_byte(LY, scanline);
	scanline++;
	if (scanline == GB_WINDOW_HEIGHT)
	{
		SDL_UpdateWindowSurface(window);
	}
}
