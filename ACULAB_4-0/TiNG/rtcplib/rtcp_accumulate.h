/* rtcp_accumulate.c - gathering RTCP data from Prosody */

#ifndef INCLUDED_RTCP_ACCUMULATE_H
#define INCLUDED_RTCP_ACCUMULATE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	unsigned end;
	unsigned char buff[1500 + 8];	// chosen as Ethernet MTU + header
} RTCP_BUFFER;

void rtcp_buffer_init(RTCP_BUFFER *bp);
unsigned rtcp_buffer_space(RTCP_BUFFER *bp);
unsigned char *rtcp_buffer_end(RTCP_BUFFER *bp);
void rtcp_buffer_append(RTCP_BUFFER *rb, unsigned size);
unsigned rtcp_buffer_needed(RTCP_BUFFER *bp);
int rtcp_buffer_type(RTCP_BUFFER *bp);
unsigned char *rtcp_buffer_data(RTCP_BUFFER *bp);
unsigned rtcp_buffer_length(RTCP_BUFFER *bp);
void rtcp_buffer_discard(RTCP_BUFFER *bp);

#ifdef __cplusplus
}
#endif

#endif
