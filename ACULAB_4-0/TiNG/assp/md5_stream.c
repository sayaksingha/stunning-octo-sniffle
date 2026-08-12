/* md5_stream.c - MD5 on a stream of data */

#include "md5_stream.h"
#include <string.h>

#ifdef TiNGTYPE_STARCORE
#pragma pgm_seg_name "md5_kernel_code_seg"
#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

void md5_stream(MD5_STREAM *ms)
{
 MD5Init(&ms->md5_ctx);
 ms->nc = 0;
}

void md5s_include(MD5_STREAM *ms, uint8_t *cp, unsigned len)
{
 for (;;) {
 	unsigned nc = arlen(ms->buf) - ms->nc;
	if (nc > len) nc = len;
	memcpy(ms->buf + ms->nc, cp, nc);
	cp += nc;
	len -= nc;
	ms->nc += nc;
	if (ms->nc < arlen(ms->buf)) return;
	MD5Update64(&ms->md5_ctx, ms->buf);
	ms->nc = 0;
 }
}

void md5s_final(uint8_t out[16], MD5_STREAM *ms)
{
 MD5Final(out, &ms->md5_ctx, ms->buf, ms->nc);
}
