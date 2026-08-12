/* gasdev.h - Gaussian deviates */

#ifndef INCLUDED_GASDEV_H
#define INCLUDED_GASDEV_H

#include "random.h"

typedef struct {
	RANDMT rand;
	int gotval;
	double savedval;	// only valid if gotval
} GASDEV;

void gasdev_init(GASDEV *gp);

/*
 * returns one normally distributed deviate with zero mean and unit variance
 */
double gasdev(GASDEV *gp);

#endif
