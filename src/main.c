#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "registers.h"
#include "cpu.h"
#include "memory.h"

#define DEBUG

int main(int argc, char **argv)
{
#ifdef DEBUG
    init();
    load_rom("tetris.gb");
    run();
    return 0;
#endif // DEBUG

	if (argc == 2)
	{
	    init();
		load_rom(argv[1]);
		run();
	}
	else
	{
		printf("usage: %s rom_name", argv[0]);
		return 1;
	}
	return 0;
}
