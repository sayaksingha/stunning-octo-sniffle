#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../../TiNG.h"
#include "../../libutil/generic_io.h"

#ifdef TiNGTYPE_LINUX
#include <unistd.h>
#endif

#ifdef TiNGTYPE_QNX
#include <unistd.h>
#endif


#ifdef TiNGTYPE_WINNT
#include "../../libutil/WINNT/lasterr.h"
#include <winbase.h>
#define sleep(x) Sleep((x)*1000)
#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static void printav(struct avdata *ap, struct avdata *oap, char *label)
{
 double dif = ap->num - oap->num;
 double av = (ap->average - (double) oap->average) / dif * oap->num
 	+ ap->average
 	+ (ap->remainder - (double) oap->remainder) / dif;
 double sav = (ap->sqaverage - (double) oap->sqaverage) / dif * oap->num
 	+ ap->sqaverage
 	+ (ap->sqremainder - (double) oap->sqremainder) / dif;
 printf("%s: n=%g av=%g dev=%g max=%d\n", label, dif, av,
 	sqrt(sav - av*av), ap->max);
}

static OPENFILE opendev(unsigned long dest, char *card, unsigned mod)
{
 OPENFILE dev;
 if (io_opendev_ting(&dev, card)) {
	return io_invalidfile();
 } else {
	struct ting_destination dst;
	dst.ioctl_length = sizeof(dst);
	dst.dest = dest;
	dst.modmask = 1 << mod;
	if (io_ioctlfile(dev, IOCTL_TiNG_WRDEST, &dst, sizeof(dst))) {
		printf("ioctl-wrdest failed\n");
		io_closefile(dev);
		dev = io_invalidfile();
	}
	printf("dest = %d, card %s, module %08x, task %08x\n",
		dst.dest, card, dst.modmask, dst.taskid);
 }
 return dev;
}

static int tstats(char *card, unsigned interval, unsigned module)
{
 struct ting_statistics prevstats;
 OPENFILE carddev = opendev(TiNG_DEST_MODULE, card, module);
 if (io_isinvalid(carddev)) return 1;
 memset(&prevstats, 0, sizeof(prevstats));
 for (;;) {
	struct ting_statistics stats;
	memset(&stats, 0, sizeof(stats));
	stats.ioctl_length = sizeof(stats);
	if (io_ioctlfile(carddev, IOCTL_TiNG_RDSTATS, &stats, sizeof(stats))) {
		printf("ioctl-rdstats failed\n");
		return 1;
	}
	printf("\nnumIRQ: tot=%d, bogus=%d, dma=%d, ours=%d\n",
		 stats.numirq_run - prevstats.numirq_run,
		 stats.numirq_bogus - prevstats.numirq_bogus,
		 stats.numirq_dma - prevstats.numirq_dma,
		 stats.numirq_ours - prevstats.numirq_ours);
	printav(&stats.rwsize[0], &prevstats.rwsize[0], "R size");
	printav(&stats.rwsize[1], &prevstats.rwsize[1], "W size");
	printav(&stats.qsize[0], &prevstats.qsize[0], "s2hQ size");
	printav(&stats.qsize[1], &prevstats.qsize[1], "h2sQ size");
	prevstats = stats;
	fflush(stdout);
	sleep(interval);
 }
}

#include "gen/tstats.args.i"

int main(int argc, char **argv)
{
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (!arg.card) {
	fprintf(stderr, "%s: No card name specified\n", progname);
	return 1;
 }
 if (!arg.interval) arg.interval = 1;
 return tstats(arg.card, arg.interval, arg.module);
}
