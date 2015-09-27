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
bool is_running = TRUE;

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

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    window = SDL_CreateWindow("gbemc", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        handle_sdl_error("unable to create window");
    }
    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == NULL)
    {
        handle_sdl_error("unable to create GL context");
    }

    SDL_GL_SetSwapInterval(1);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    pixels = (u8*)malloc(sizeof(u8) * WINDOW_WIDTH * WINDOW_HEIGHT);
}

void handle_events(void)
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_QUIT:
            is_running = FALSE;
            break;
        case SDL_KEYDOWN:
            break;
        case SDL_KEYUP:
            break;
        }
    }
}

void init(void)
{
    reset_mem();
    reset_cpu();
    init_video();
}

void run(void)
{
    const unsigned int hblank_len = 456;
    const unsigned int vblank_len = 4560;

	while (is_running)
	{
	    render_full();
	    while (scanline != WINDOW_HEIGHT)
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
    SDL_GL_DeleteContext(gl_context);
    SDL_Quit();
}
