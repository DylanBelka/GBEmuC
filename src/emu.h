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

struct GBKeys
{
	u8 keys[2];
	u8 colID;
};

void cleanup(void);

void init_gpu(void);
void gpu_cleanup(void);
void init(void);

void run(void);

extern SDL_Window* window;
extern bool is_running;
extern SDL_Surface* window_surf;
extern SDL_Surface* draw_surf; // 256x256 surface that is drawn on (then copied to window_surf)
extern struct GBKeys key_info;

enum KeyGroups
{
	p14 = 0, p15 = 1
};

enum KeyCodes
{
	keyRight = 0x1,
	keyA = 0x1,
	keyLeft = 0x2,
	keyB = 0x2,
	keyUp = 0x4,
	keySelect = 0x4,
	keyDown = 0x8,
	keyStart = 0x8
};

#endif /* GBEMC_EMU_H */
