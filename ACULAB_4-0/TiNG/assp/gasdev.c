/* gasdev.c - Gaussian deviates */

#include <math.h>
#include "gasdev.h"

/*
 * Gaussian Deviates, from Numerical recipes in C, 2nd Edition
 *
 * Modified to perform initialisation separately
 */

void gasdev_init(GASDEV *gp)
{
// srand(time(0));
// ran2_init(&gp->rand, rand());
 ran2_init(&gp->rand, 0);
 gp->iset = 0;
}

/*
 * returns normally distributed deviate with zero mean and unit variance
 */
double gasdev(GASDEV *gp)
{
 if (!gp->iset) {
 	for (;;) {
		double v1 = 2 * ran2(&gp->rand) - 1;
		double v2 = 2 * ran2(&gp->rand) - 1;
		double rsq = v1*v1 + v2*v2;
		if (rsq && rsq <= 1) {
			double fac = sqrt(-2 * log(rsq) / rsq);
			gp->gset = v1 * fac;
			gp->iset = 1;
			return v2 * fac;
		}
	}
 } else {
 	gp->iset = 0;
 	return gp->gset;
 }
}
