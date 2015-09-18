#ifndef GBEMC_EMU_H
#define GBEMC_EMU_H

//#define GLEW_STATIC

#include <SDL.h>
#include <SDL_opengl.h>

#include "common.h"

#define WINDOW_WIDTH 160
#define WINDOW_HEIGHT 144

void cleanup(void);

void init_gpu(void);
void gpu_cleanup(void);
void init(void);

void run(void);

void handle_events(void);

extern SDL_Window* window;
extern SDL_GLContext gl_context;
extern bool is_running;

#endif // GBEMC_EMU_H
