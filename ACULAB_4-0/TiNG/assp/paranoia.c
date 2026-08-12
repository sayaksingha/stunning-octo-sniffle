/* paranoia.c - do extensive object check on each use */

#ifdef PARANOIA

#include <stdio.h>
#include <stdint.h>
#include "paranoia.h"

static U16 checksum(void *obj, unsigned len)
{
 U16 *op = obj;
 uint_fast32_t sum = (uint_fast32_t) op & 0xffff;
 	// assumes no object is as big as 64K
 for (; len > 1; len -= 2) {
 	sum += *op++;
 }
 if (len & 1) {
 	sum += *(char *) op++;
 }
 sum += sum >> 16;
 return sum;
}

void check_object_fn(void *obj, unsigned len, char *file, int line)
{
 PARACHECK *pmp = obj;
 U16 sum;
 sum = checksum(obj, len);
 if (sum != 0xffff && sum) {
	fprintf(stderr, "%s:%d: Object checksum failed!\n", file, line);
	fprintf(stderr, "Got %04x\n", sum);
	fprintf(stderr, "Last checked at %s:%d\n", pmp->lfile, pmp->lline);
	abort();
 }
}

void commit_object_fn(void *obj, unsigned len, char *file, int line)
{
 PARACHECK *pmp = obj;
 pmp->check = 0;
 pmp->lfile = file;
 pmp->lline = line;
 pmp->check = ~checksum(obj, len);
}

#endif
