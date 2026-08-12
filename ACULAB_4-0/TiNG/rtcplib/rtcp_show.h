/* rtcp_show.h - functions to display RTCP data */

#ifndef INCLUDED_RTCP_SHOW_H
#define INCLUDED_RTCP_SHOW_H

#include <stdio.h>
#include "rtcp_accumulate.h"

#ifdef __cplusplus
extern "C" {
#endif

unsigned show_rtcp(FILE *outf, unsigned char *dp, unsigned length);
void show_rtcp_summary(FILE *outf, unsigned char *dp, unsigned length);
void show_prosody_rtcp(FILE *outf, RTCP_BUFFER *rb);

#ifdef __cplusplus
}
#endif

#endif
