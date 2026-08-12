/* generic_io.c - Unix implementation of operating system independent file ops */

#include <sys/poll.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../generic_io.h"

#define TiNG_DEV_PATH "/dev/aculab/"

static const char version[] = "@(#)$from: " __FILE__ " "
#include "../../gen/version.i"
	" $";

OPENFILE io_invalidfile(void)
{
 return -1;
}

int io_isinvalid(OPENFILE dp)
{
 return dp == -1;
}

void io_tingdev_close(DEVLIST *dp)
{
 for (; dp->root; ) {
	struct device *nx = dp->root->next;
	free(dp->root);
	dp->root = nx;
 }
}

int io_tingdev_open(DEVLIST *dp)
{
 DIR *dir;
 dp->root = 0;
 dp->curdev = 0;
 dir = opendir(TiNG_DEV_PATH);
 if (!dir) return 1;
 for (;;) {
	struct dirent *de;
	de = readdir(dir);
	if (!de) break;
	if (!strncmp(de->d_name, "Prosody_", 8)) {
		struct device **dpp;
		struct device *np = malloc(sizeof(*np) + strlen(de->d_name));
		if (!np) {
			io_tingdev_close(dp);
			closedir(dir);
			return 1;
		}
		strcpy(np->name, de->d_name);
		for (dpp = &dp->root; *dpp; dpp = &(*dpp)->next)
			if (strcmp(np->name, (*dpp)->name) < 0) break;
		np->next = *dpp;
		*dpp = np;
	}
 }
 closedir(dir);
 return 0;
}

char *io_tingdev_next(DEVLIST *dp)
{
 if (!dp->curdev) dp->curdev = dp->root;
 else dp->curdev = dp->curdev->next;
 if (dp->curdev) return dp->curdev->name;
 return 0;
}

int io_openfile(char *name, int flags, int mode, OPENFILE *fp)
{
 *fp = open(name, flags, mode);
 if (*fp == -1) {
	 perror("open failed");
	 return 1;
 }
 return 0;
}

int io_readfile(OPENFILE f, void *buf, unsigned nc, unsigned *nr)
{
 *nr = read(f, buf, nc);
 if ((int) *nr == -1) {
	*nr = 0;
	perror("read failed");
	return 1;
 }
 return 0;
}

int io_writefile(OPENFILE f, void *buf, unsigned nc, unsigned *nw)
{
 *nw = write(f, buf, nc);
 if ((int) *nw == -1) {
	*nw = 0;
	perror("write failed");
	return 1;
 }
 return 0;
}

int io_ioctlfile(OPENFILE f, int cmd, void *ap, unsigned asiz)
{
 int i;
 i = ioctl(f, cmd, ap);
 (void) asiz;
 if (i == -1) {
	perror("ioctl failed");
	return 1;
 }
 return 0;
}

int io_closefile(OPENFILE f)
{
 if (close(f) == -1) {
	perror("close failed");
	return 1;
 }
 return 0;
}

int io_opendev_ting(OPENFILE *dp, char *cardname)
{
 char name[40];
 sprintf(name, TiNG_DEV_PATH "%s", cardname);
 *dp = open(name, O_RDWR);
 if (*dp == (OPENFILE) -1) {
	char *err = strerror(errno);
	if (!err) err = "<Unknown error>";
	fprintf(stderr, "Cannot open %s: %s\n", name, err);
	return 1;
 }
 return 0;
}

int io_inkey(OPENFILE f)
{
 struct pollfd pfd;
 pfd.fd = f;
 pfd.events = POLLIN;
 switch (poll(&pfd, 1, 0)) {
	unsigned char c;
 case -1: perror("poll failed"); break;
 case 0: break;
 case 1:
	switch (read(f, &c, 1)) {
	case 0: return INKEY_EOF;
	case 1: return c;
	default: perror("read failed"); break;
	}
 }
 return INKEY_NONE;
}

int io_stdinfile(OPENFILE *fp)
{
 *fp = 0;
 return 0;
}

void Sleep(int ms)
{
 if (poll(0, 0, ms)) {
	perror("poll failed");
 }
}

void io_lasterr(char *fn)
{
 perror(fn);
}
