
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define arlen(s) (sizeof(s)/sizeof(*(s)))

#include "../../TiNG.h"
#include "../../libutil/generic_io.h"

#ifdef TiNGTYPE_LINUX
#include <unistd.h>
#endif


#ifdef TiNGTYPE_WINNT
#include <io.h>
#define write _write
#endif

static int enable_trace(OPENFILE dev, int trace_level)
{
 struct ting_destination dest;
 struct ting_trace tt;
 dest.ioctl_length = sizeof(dest);
 dest.dest = TiNG_DEST_TRACEOUT;
 if (io_ioctlfile(dev, IOCTL_TiNG_WRDEST, &dest, sizeof(dest))) {
	fprintf(stderr, "Cannot select trace to enable it\n");
	return 1;
 }
 tt.ioctl_length = sizeof(tt);
 tt.trace_level = trace_level;
 if (io_ioctlfile(dev, IOCTL_TiNG_WRTRACE, &tt, sizeof(tt))) {
	fprintf(stderr, "ioctl-wrtrace failed\n");
	fprintf(stderr, "Cannot set trace level to %d\n", trace_level);
	return 1;
 }
 return 0;
}

static int apitrace(char *card, int trace_level)
{
 OPENFILE dev;
 int err = 1;
 if (io_opendev_ting(&dev, card)) {
	fprintf(stderr, "open(%s) failed\n", card);
	return 1;
 }
 if (!enable_trace(dev, trace_level)) {
	for (;;) {
		char buff[1024];
		unsigned nr;
		int nw;
		err = io_readfile(dev, buff, arlen(buff), &nr);
		if (err) {
			fprintf(stderr, "Cannot read trace from device\n");
			break;
		}
		if (!nr) break;
		nw = write(1, buff, nr);
		if ((unsigned) nw != nr) {
			if (nw == -1) {
				perror("write failed");
			} else {
				fprintf(stderr, "Wrote %d of %d\n", nw, nr);
			}
			err = 1;
			fprintf(stderr, "Error writing trace\n");
			break;
		}
	}
 }
 io_closefile(dev);
 return err;
}

#include "gen/apitrace.args.i"

int main(int argc, char **argv)
{
 DEVLIST dl;
 char *card;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (arg.card) card = arg.card;
 else {
	if (io_tingdev_open(&dl)) {
		io_lasterr("io_tingdev_open");
		fprintf(stderr, "%s: Cannot trace\n",
			progname);
		return 1;
	}
	card = io_tingdev_next(&dl);
	if (!card) {
		fprintf(stderr, "%s: Cannot trace - no Prosody card found\n",
			progname);
		return 1;
	}
 }
 if (apitrace(card, arg.trace_level)) {
	fprintf(stderr, "%s: Error attempting to get Prosody trace\n",
		progname);
	return 1;
 }
 return 0;
}

