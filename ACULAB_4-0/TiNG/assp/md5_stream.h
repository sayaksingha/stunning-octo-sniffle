/* md5_stream.h - MD5 on a stream of data */

#ifndef INCLUDED_MD5_STREAM_H
#define INCLUDED_MD5_STREAM_H

#include "md5.h"

typedef struct {
	MD5_CTX md5_ctx;
	unsigned char buf[64];
	unsigned nc;
} MD5_STREAM;

#include <stdint.h>

void md5_stream(MD5_STREAM *ms);

void md5s_include(MD5_STREAM *ms, uint8_t *cp, unsigned len);

void md5s_final(uint8_t out[16], MD5_STREAM *ms);

#endif
