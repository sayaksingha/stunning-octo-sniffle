/* rtcp_accumulate.c - gathering RTCP data from Prosody */

#include "rtcp_accumulate.h"
#include <memory.h>
#include <stdint.h>
#include <string.h>

void rtcp_buffer_init(RTCP_BUFFER *bp)
{
 bp->end = 0;
}

static uint_fast32_t rd32(unsigned char *dp)
{
 unsigned u = *dp++;
 u |= *dp++ << 8;
 u |= *dp++ << 16;
 u |= *dp++ << 24;
 return u;
}

unsigned rtcp_buffer_space(RTCP_BUFFER *bp)
{
 return sizeof(bp->buff) - bp->end;
}

unsigned char *rtcp_buffer_end(RTCP_BUFFER *bp)
{
 return &bp->buff[bp->end];
}

void rtcp_buffer_append(RTCP_BUFFER *rb, unsigned size)
{
 rb->end += size;
}

unsigned rtcp_buffer_needed(RTCP_BUFFER *bp)
{
 uint_fast32_t plen;
 if (bp->end < 4) return 4 - bp->end;
 plen = rd32(&bp->buff[0]);
 plen = (plen + 3) & ~3;	// allow for padding
 if (plen > bp->end - 4) return plen - bp->end + 4;
 return 0;
}

int rtcp_buffer_type(RTCP_BUFFER *bp)
{
 return rd32(&bp->buff[4]);
}

unsigned char *rtcp_buffer_data(RTCP_BUFFER *bp)
{
 return &bp->buff[8];
}

unsigned rtcp_buffer_length(RTCP_BUFFER *bp)
{
 return rd32(&bp->buff[0]);
}

void rtcp_buffer_discard(RTCP_BUFFER *bp)
{
 uint_fast32_t size = rd32(&bp->buff[0]) + 4;
 size = (size + 3) & ~3;
 if (bp->end == size) {
 	bp->end = 0;
 } else {
	memmove(bp->buff, &bp->buff[size], bp->end - size);
	bp->end -= size;
 }
}
