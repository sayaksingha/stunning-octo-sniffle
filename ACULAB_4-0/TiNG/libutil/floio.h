/* floio.h - floating point I/O */

#ifndef INCLUDED_FLOIO_H
#define INCLUDED_FLOIO_H

#include <stdio.h>

// read floating point representation:
//	byte 3	   byte 2     byte 1	 byte 0
//	eeee eeee  smmm mmmm  mmmm mmmm  mmmm mmmm
int getflo(FILE *inf, float *f);

// write floating point representation:
//	byte 3	   byte 2     byte 1	 byte 0
//	eeee eeee  smmm mmmm  mmmm mmmm  mmmm mmmm
int putflo(FILE *of, float f);

#endif
