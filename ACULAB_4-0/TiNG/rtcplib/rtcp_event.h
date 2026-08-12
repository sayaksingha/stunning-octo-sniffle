/* rtcp_event.h - example of how to handle RTCP */
#ifndef INLCUDED_RTCPLIB_RTCP_EVENT_H
#define INLCUDED_RTCPLIB_RTCP_EVENT_H

#include "rtcp_accumulate.h"
#include "../apilib/smrtp.h"
#include <stdio.h>

// called when RTCP handler event is signalled
int handle_rtcp_event(FILE *outf, RTCP_BUFFER *rb, tSMRTCPHandId rtcphand);

#endif
