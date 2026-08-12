/* paranoia.c - do extensive object check on each use */

#include "paranoia.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef TiNG_PARANOIA

STATIC unsigned short checksum(void *obj, unsigned len)
{
 unsigned short *op = obj;
 unsigned sum = (unsigned) op & 0xffff;
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

LOCALDEF void check_object(void *obj, unsigned len, char *file, int line)
{
 PARAMUTX *pmp = obj;
 unsigned short sum;
 obj = (char *) obj + sizeof(MUTX);
 len -= sizeof(MUTX);
 sum = checksum(obj, len);
 if (sum != 0xffff && sum) {
	fprintf(stderr, "%s:%d: Object checksum failed!\n", file, line);
	fprintf(stderr, "Got %04x\n", sum);
	fprintf(stderr, "Last checked at %s:%d\n", pmp->lfile, pmp->lline);
	abort();
 }
}

LOCALDEF void commit_object(void *obj, unsigned len, char *file, int line)
{
 PARAMUTX *pmp = obj;
 obj = (char *) obj + sizeof(MUTX);
 len -= sizeof(MUTX);
 pmp->check = 0;
 pmp->lfile = file;
 pmp->lline = line;
 pmp->check = ~checksum(obj, len);
}

#else

#endif
