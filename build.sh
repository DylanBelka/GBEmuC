#!/bin/bash
gcc -std=c11 common.h registers.h memory.h cpu.h cpu.c registers.c memory.c main.c -o gbemc