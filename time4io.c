#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h" 
#include "math.h"

int getsw(void)
{
	volatile int * input = (volatile int*) 0xbf8860d0;
	return((*input & 0xF00) >> 8);
}

int getbtns(void)
{
	volatile int * input = (volatile int*) 0xbf8860d0;
	return((*input & 0xF0) >> 4);
}