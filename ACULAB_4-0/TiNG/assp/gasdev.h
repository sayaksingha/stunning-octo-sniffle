/* gasdev.h - Gaussian deviates */

#ifndef INCLUDED_GASDEV_H
#define INCLUDED_GASDEV_H

#include "ran2.h"

typedef struct {
	RAN2 rand;
	int iset;
	double gset;
} GASDEV;

void gasdev_init(GASDEV *gp);

/*
 * returns normally distributed deviate with zero mean and unit variance
 */
double gasdev(GASDEV *gp);

#endif
