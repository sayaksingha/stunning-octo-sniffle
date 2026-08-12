/* reset.c - reset a module */
#include <stdio.h>

#include "../../TiNG.h"
#include "../../libutil/generic_io.h"

#ifdef TiNGTYPE_WINNT
#include "../../libutil/WINNT/lasterr.h"
#include <winbase.h>
#define sleep(x) Sleep((x)*1000)
#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static int reset(char *card, unsigned module)
{
 struct ting_destination dst;
 struct ting_mstate ms;
 OPENFILE dev;
 if (io_opendev_ting(&dev, card)) {
	io_lasterr("error opening device");
	return 1;
 }
 dst.ioctl_length = sizeof(dst);
 dst.dest = TiNG_DEST_MODULE;
 dst.modmask = 1 << module;
 if (io_ioctlfile(dev, IOCTL_TiNG_WRDEST, &dst, sizeof(dst))) {
	printf("ioctl failed: cannot select destination\n");
 }
 ms.ioctl_length = sizeof(ms);
 ms.mstate = MS_RESET;
 if (io_ioctlfile(dev, IOCTL_TiNG_WRMSTATE, &ms, sizeof(ms))) {
	printf("ioctl-wrmstate failed\n");
 }
 io_closefile(dev);
 return 0;
}

#include "gen/reset.args.i"

int main(int argc, char **argv)
{
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (!arg.card) {
	fprintf(stderr, "%s: No card specified\n", progname);
	return 1;
 }
 return reset(arg.card, arg.module);
}
