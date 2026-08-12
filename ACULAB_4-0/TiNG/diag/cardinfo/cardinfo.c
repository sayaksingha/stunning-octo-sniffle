#include <math.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "../../TiNG.h"
#include "../../libutil/generic_io.h"

#ifdef TiNGTYPE_LINUX
#include <unistd.h>
#endif


#ifdef TiNGTYPE_WINNT
#include "../../libutil/WINNT/lasterr.h"
#include <winbase.h>
#define sleep(x) Sleep((x)*1000)
#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static char *cardname(unsigned cardtype)
{
 static char *names[] = {
	"S2", "P1", "BR8", "C1", "?", "V1",
 };
 if (cardtype >= arlen(names)) return "?";
 return names[cardtype];
}

static int cardinf(char *card, int quiet)
{
 struct ting_cardinfo cardinfo;
 OPENFILE dev;
 if (io_opendev_ting(&dev, card)) {
	fprintf(stderr, "open(%s) failed\n", card);
	return 1;
 }
 memset(&cardinfo, 0, sizeof(cardinfo));
 cardinfo.ioctl_length = sizeof(cardinfo);
 if (io_ioctlfile(dev, IOCTL_TiNG_RDCARDINFO, &cardinfo, sizeof(cardinfo))) {
	fprintf(stderr, "ioctl-rdcardinfo failed\n");
	io_closefile(dev);
	return 1;
 }
 if (quiet) {
	printf("%s\n", cardinfo.serialno);
 } else {
	printf("%2d: %-6s  io=0x%04lx  mem=0x%08lx  irq=%-2ld  sw=%-2d  '%s'\n",
		cardinfo.cardtype,
		cardname(cardinfo.cardtype),
		(unsigned long) cardinfo.ioaddress,
		(unsigned long) cardinfo.memaddress,
		(long) cardinfo.interrupt,
		cardinfo.switchno,
		cardinfo.serialno);
 }
 io_closefile(dev);
 return 0;
}

#include "gen/cardinfo.args.i"

int main(int argc, char **argv)
{
 int err = 0;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (!arg.card) {
	DEVLIST dl;
	if (io_tingdev_open(&dl)) {
		io_lasterr("io_tingdev_open");
		return 1;
	}
	for (;;) {
		char *card = io_tingdev_next(&dl);
		if (!card) break;
		if (cardinf(card, arg.quiet)) err = 1;
	}
	io_tingdev_close(&dl);
	return err;
 }
 return cardinf(arg.card, arg.quiet);
}
