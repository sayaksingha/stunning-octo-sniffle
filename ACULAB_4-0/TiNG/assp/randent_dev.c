/* randent_dev.c - true random number generator using /dev/urandom */

#include "randent.h"
#include <stdint.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>

static int randdevfd = -1;

void randent_init(void)
{
 randdevfd = open("/dev/urandom", O_RDONLY);
 if (randdevfd < 0) {
 	perror("open(/dev/urandom) failed");
 }
}

unsigned long randent32(void)
{
 uint32_t v;
 int nr = read(randdevfd, &v, 4);
 if (nr < 0) {
 	perror("read from /dev/urandom failed");
 }
 return v;
}

void randent_addrand(void *data, unsigned bytes)
{
 (void) data;
 (void) bytes;
}
