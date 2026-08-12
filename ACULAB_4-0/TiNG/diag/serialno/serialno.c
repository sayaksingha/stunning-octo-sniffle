#include <math.h>
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

static int serialno(char *card)
{
 struct ting_cardinfo cardinfo;
 OPENFILE dev;
 if (io_openfile(card, 0, 0, &dev)) {
	fprintf(stderr, "io_openfile(%s) failed\n", card);
	return 1;
 }
 memset(&cardinfo, 0, sizeof(cardinfo));
 cardinfo.ioctl_length = sizeof(cardinfo);
 if (io_ioctlfile(dev, IOCTL_TiNG_RDCARDINFO, &cardinfo, sizeof(cardinfo))) {
	fprintf(stderr, "ioctl-rdcardinfo failed\n");
	return 1;
 }
 printf("%s\n", cardinfo.serialno);
 return 0;
}

#include "gen/serialno.args.i"

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
 return serialno(arg.card);
}
