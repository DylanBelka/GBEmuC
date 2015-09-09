#!/bin/bash
gcc -std=c11 src/common.h src/registers.h src/memory.h src/cpu.h src/cpu.c src/registers.c src/memory.c src/main.c -o gbemc