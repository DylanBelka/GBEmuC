#ifndef GBEMC_EMU_H
#define GBEMC_EMU_H

#include <SDL.h>

#include "common.h"

#define WINDOW_WIDTH 160			// emulator window dimensions
#define WINDOW_HEIGHT 144

#define GB_WINDOW_WIDTH 160			// dimensions of actual gameboy window
#define GB_WINDOW_HEIGHT 144

#define VIRT_GB_WINDOW_WIDTH 256	// 32x32 (tile) screen buffer
#define VIRT_GB_WINDOW_HEIGHT 256

void cleanup(void);

void init_gpu(void);
void gpu_cleanup(void);
void init(void);

void run(void);

void handle_events(void);

extern SDL_Window* window;
extern SDL_GLContext gl_context;
extern bool is_running;
extern SDL_Surface* window_surf;

#endif /* GBEMC_EMU_H */
