#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "registers.h"
#include "cpu.h"
#include "memory.h"

int main(int argc, char **argv)
{
	registers.A = 0x42;
	registers.F.raw = 0x00;
	printf("%x\n", registers.AF);
	printf("%x\n", registers.BC);
	printf("works");
	cpu_test();
	return 0;
}