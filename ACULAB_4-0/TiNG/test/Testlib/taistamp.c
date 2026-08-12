#define __STDC_LIMIT_MACROS

#include <stdint.h>
#include <sys/timeb.h>
#include <sys/types.h>

#include "taistamp.h"

void taistamp(char buf[32])
{
 struct timeb now;
 int64_t t = UINT64_C(4611686018427387914);		// 0x40000000 0000000A
 int i;
 ftime(&now);
 *buf++ = '@';
 t += now.time;
 for (i=60; i >= 0; i -= 4) {
 	*buf++ = "0123456789abcdef"[(t >> i) & 0xf];
 }
 t = now.millitm;
 t *= 1000 * 1000;			// to nS
 for (i=28; i >= 0; i -= 4) {
 	*buf++ = "0123456789abcdef"[(t >> i) & 0xf];
 }
 *buf = 0;
}
