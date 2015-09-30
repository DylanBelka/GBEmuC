#include <stdio.h>
#include <stdlib.h>

#include "emu.h"
#include "common.h"
#include "memdefs.h"
#include "cpu.h"
#include "gpu.h"
#include "memory.h"

SDL_Window* window;
SDL_GLContext gl_context;
bool is_running = true;
SDL_Surface* window_surf;
SDL_Surface* virt_window_surf;
struct GBKeys key_info;

void handle_sdl_error(char *msg)
{
	printf("SDL Error: %s error code %d\n", msg, SDL_GetError());
	if (window)
		SDL_DestroyWindow(window);
	if (gl_context)
		SDL_GL_DeleteContext(gl_context);
	SDL_Quit();
	exit(-1);
}

void init_video(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		handle_sdl_error("unable to initialize SDL");
	}
	window = SDL_CreateWindow("gbemc", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							 WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		handle_sdl_error("unable to create window");
	}

	window_surf = SDL_GetWindowSurface(window);
	if (window_surf == NULL)
	{
        handle_sdl_error("unable to create window surface");
	}
	SDL_PixelFormat *fmt = window_surf->format;
	virt_window_surf = SDL_CreateRGBSurface(0, VIRT_GB_WINDOW_WIDTH, VIRT_GB_WINDOW_HEIGHT,
											fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
	if (virt_window_surf == NULL)
	{
		handle_sdl_error("unable to create virtual window surface");
	}

	SDL_UpdateWindowSurface(window);
}

void handle_events(void)
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			{
			SDL_Keycode key = e.key.keysym.sym;
			if (key == SDLK_ESCAPE)
			{
				is_running = false;
			}
			if (key == SDLK_UP) // up
			{
				key_info.keys[p14] &= ~keyUp;
			}
			if (key == SDLK_DOWN) // down
			{
				key_info.keys[p14] &= ~keyDown;
			}
			if (key == SDLK_LEFT) // left
			{
				key_info.keys[p14] &= ~keyLeft;
			}
			if (key == SDLK_RIGHT) // right
			{
				key_info.keys[p14] &= ~keyRight;
			}
			if (key == SDLK_z) // a
			{
				key_info.keys[p15] &= ~keyA;
			}
			if (key == SDLK_x) // b
			{
				key_info.keys[p15] &= ~keyB;
			}
			if (key == SDLK_RETURN) // start
			{
				key_info.keys[p15] &= ~keyStart;
			}
			if (key == SDLK_BACKSPACE) // select
			{
				key_info.keys[p15] &= ~keySelect;
			}
			}
			break;
		case SDL_KEYUP:
			{
			SDL_Keycode key = e.key.keysym.sym;
			if (key == SDLK_UP) // up
			{
				key_info.keys[p14] |= keyUp;
			}
			if (key == SDLK_DOWN) // down
			{
				key_info.keys[p14] |= keyDown;
			}
			if (key == SDLK_LEFT) // left
			{
				key_info.keys[p14] |= keyLeft;
			}
			if (key == SDLK_RIGHT) // right
			{
				key_info.keys[p14] |= keyRight;
			}
			if (key == SDLK_z) // a
			{
				key_info.keys[p15] |= keyA;
			}
			if (key == SDLK_x) // b
			{
				key_info.keys[p15] |= keyB;
			}
			if (key == SDLK_RETURN) // start
			{
				key_info.keys[p15] |= keyStart;
			}
			if (key == SDLK_BACKSPACE) // select
			{
				key_info.keys[p15] |= keySelect;
			}
			}
			break;
		}
	}
}

void init(void)
{
	reset_mem();
	reset_cpu();
	init_video();
	key_info.keys[0] = 0x0F;
	key_info.keys[1] = 0x0F;
	key_info.colID = 0x0;
}

void run(void)
{
	const unsigned int hblank_len = 456;
	const unsigned int vblank_len = 4560;

	while (is_running)
	{
		render_full();
		while (scanline != GB_WINDOW_HEIGHT)
		{
			draw_scanline();
			while (cpu.clock_cycles < hblank_len)
			{
				handle_events();
				cpu_tick();
			}
			cpu.clock_cycles = 0;
		}
		write_byte(IF, 0x1); /* set vblank interrupt */
		while (cpu.clock_cycles < vblank_len) /* emulate vblank */
		{
			scanline++;
			write_byte(LY, scanline);
			handle_events();
			cpu_tick();
		}
	}

	cleanup();
}

void cleanup(void)
{
	SDL_DestroyWindow(window);
	SDL_FreeSurface(window_surf);
	SDL_FreeSurface(virt_window_surf);
	SDL_GL_DeleteContext(gl_context);
	SDL_Quit();
}
