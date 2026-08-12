/* rtcp_event.c - example of how to handle RTCP */
#include "rtcp_event.h"
#include "rtcp_show.h"
#include <memory.h>

// called when RTCP handler event is signalled
int handle_rtcp_event(FILE *outf, RTCP_BUFFER *rb, tSMRTCPHandId rtcphand)
{
 SM_RTCPHAND_GET_DATA_PARMS gdp;
 int r;
 memset(&gdp, 0, sizeof(gdp));
 gdp.rtcphand = rtcphand;
 gdp.data = (char*) rtcp_buffer_end(rb);
 gdp.max_length = rtcp_buffer_needed(rb);
 r = sm_rtcphand_get_data(&gdp);
 if (!r) {
	rtcp_buffer_append(rb, gdp.done_length);
	for (;;) {
		if (rtcp_buffer_needed(rb)) return 0;
		show_prosody_rtcp(outf, rb);
		rtcp_buffer_discard(rb);
	}
 }
 return r;
}
