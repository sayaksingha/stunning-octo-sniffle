/* gasdev.c - Gaussian deviates */

#include <math.h>
#include "gasdev.h"

/*
 * Generation of Gaussian deviates using standard Box-Muller method
 */

void gasdev_init(GASDEV *gp)
{
 randmt(&gp->rand, 0);
 gp->gotval = 0;
}

/*
 * returns one normally distributed deviate with zero mean and unit variance
 */
double gasdev(GASDEV *gp)
{
 if (!gp->gotval) {
 	for (;;) {
		double x = 2 * randmt_real1(&gp->rand) - 1;
		double y = 2 * randmt_real1(&gp->rand) - 1;
		double rsq = x*x + y*y;
		if (rsq <= 1 && rsq) {
			double fac = sqrt(-2 * log(rsq) / rsq);
			gp->savedval = x * fac;
			gp->gotval = 1;
			return y * fac;
		}
	}
 } else {
 	gp->gotval = 0;
 	return gp->savedval;
 }
}
