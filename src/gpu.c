#include <stdio.h>
#include <stdlib.h>

#include <SDL_video.h>

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

void draw_slice(u8 b1, u8 b2, unsigned int *x, unsigned int *y, bool is_sprite)
{
    for (int i = 0x80; i >= 1; i >>= 1)
	{
        int curr_bit1 = b1 & i;
        int curr_bit2 = b2 & i;
        u32 *pixels = (u32*)window_surf->pixels;
		if ((s32)*x >= 0 && (s32)*y >= 0 &&
			(s32)*x < window_surf->w && (s32)*y < window_surf->h) // make sure the coords are in bounds
		{
			u32 *pixel;
			if (!is_sprite)
			{
				u32 scrolledX, scrolledY;
				// emulate background scrolling
				scrolledX = *x + read_byte(SCX);
				scrolledY = *y + read_byte(SCY);
				pixel = &pixels[scrolledY * window_surf->w + scrolledX];
			}
			else // sprites are not scrolled
			{
				pixel = &pixels[*y * window_surf->w + *x];
			}

			if (curr_bit1 && curr_bit2) // bit1 (on) and bit2 (on)
			{
				*pixel = 0x081820; // "black"
			}
			else if (!is_sprite && !curr_bit1 && !curr_bit2) // bit1 (off) and bit2 (off)
			{
				*pixel = 0xE0F8D0; // "white"
			}
			else if (curr_bit1 && !curr_bit2) // bit1 (on) and bit2 (off)
			{
				*pixel = 0x88C070; // "light grey"
			}
			else if (!is_sprite) // bit1 (off) bit2 (on)
			{
				*pixel = 0x346856; // "dark grey"
			}
		}
		*x += 1;
	}

	*x -= 8;
	*y += 1;
}

void draw_background(void)
{
	u32 x = 0, y = 0;
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
			draw_slice(read_byte(j), read_byte(j + 1), &x, &y, false);
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
        u32 y = (read_byte(i) & 0xFF) - 16;
        u32 x = (read_byte(i + 1) & 0xFF) - 8;
        u16 chr_loc_start = read_byte(i + 2) * 0x10 + CHR_MAP_UNSIGNED;
        for (int j = chr_loc_start; j < chr_loc_start + 0x10; j += 2)
		{
            draw_slice(read_byte(j), read_byte(j + 1), &x, &y, true);
		}
		if (is_8x16_sprite) // draw the second half of the sprite
		{
			chr_loc_start += 0x10; // second half of the sprite is located directly after the first
			for (int j = chr_loc_start; j < chr_loc_start + 0x10; j += 2)
			{
				draw_slice(read_byte(j), read_byte(j + 1), &x, &y, true);
			}
		}
	}
}

void draw_window(void)
{
	u32 x = (read_byte(WX) & 0xFF) - 7;
	u32 y = read_byte(WY) & 0xFF;
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
			draw_slice(read_byte(j), read_byte(j + 1), &x, &y, false);
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
		if (lcdc & bit5) // is window enabled?
		{
			draw_window();
		}
		if (lcdc & bit1) // are sprites enabled?
		{
			draw_sprites();
		}
	}

	scanline = 0;
	write_byte(LY, scanline);
	write_byte(STAT, read_byte(STAT) & ~bit4); // clear STAT after vblank
}

void draw_scanline(void)
{
	write_byte(LY, scanline);
	scanline++;
	write_byte(STAT, (read_byte(STAT) | bit3) & ~bit0 & ~bit1); // update STAT for hblank

	if (scanline == GB_WINDOW_HEIGHT) // vblank
	{
		write_byte(IF, 0x1); /* set vblank interrupt */
		write_byte(STAT, (read_byte(STAT) | bit4) & ~bit3 | bit0); // update STAT for vblank, clear hblank
		SDL_UpdateWindowSurface(window);
	}
}
