/* tracehistory.c - implement a trace which is saved only on demand */
#include "smdrvr.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tracehistory.h"

	// this is only to show which parts of the code support resizeing
	// if resizing is disabled, then you cannot disable the history
#define TRACE_RESIZEABLE

static pthread_mutex_t logmx = PTHREAD_MUTEX_INITIALIZER;
static char *log_buff;
static size_t log_len, log_offset;
static int log_full = 0;

/* The trace buffer has two states: full and not full. When it is not
 * full, it looks like this:
 *	+---------------------------------------+
 *	|hello|                                 |
 *	+---------------------------------------+
 * with the portion from the start to log_offset holding data and the
 * portion from log_offset to the end being empty. When it is full, it
 * looks like this:
 *	+---------------------------------------+
 *	|fer! |, world, this is a full trace buf|
 *	+---------------------------------------+
 * where the the older part is from log_offset to the end and the data
 * continues from the start of the buffer up to log_offset.
 *
 * So, the range [0..log_offset) always holds the latest part. If log_full,
 * then and [log_offset..log_len) is older trace, otherwise it's empty.
 */

static int olog_history(const char *fmt, va_list ap)
{
 int sts;
 if (!log_buff) return 0;	// no buffer - cannot do anything
 sts = pthread_mutex_lock(&logmx);
 if (!sts) {
	char buff[1024];	// trace buffer had better not be smaller
	unsigned len = vsnprintf(buff, sizeof(buff), fmt, ap);
	unsigned toend = log_len - log_offset;
	if (len < toend) {	// does not reach end
		memcpy(log_buff + log_offset, buff, len);
		log_offset += len;
	} else {	// fills to end - then wrap remainder
		memcpy(log_buff + log_offset, buff, toend);
			// assume rest fits in buffer - if not writes off end
		memcpy(log_buff, buff + toend, log_offset = len - toend);
		log_full = 1;
	}
	sts = pthread_mutex_unlock(&logmx);
 }
 return sts;
}

#ifdef TRACE_RESIZEABLE
/* If we can resize the buffer, we must unwrap it. For example:
 *	+---------------------------------------+
 *	|fer! |, world, this is a full trace buf|
 *	+---------------------------------------+
 * must be changed into:
 *	+---------------------------------------+
 *	|, world, this is a full trace buffer!  |
 *	+---------------------------------------+
 * so that if it is made bigger the extra space is added on the end and not
 * between the two portions of the old data.
 * If the buffer is reduced in size, we also need to copy the data down
 * so that the part discarded is the oldest. For example, this:
 *	+---------------------------------------+
 *	|ull trace buffer!                      |
 *	+---------------------------------------+
 * which becomes:
 *	+-------------------+
 *	|ull trace buffer!  |
 *	+-------------------+
 */

// swap two blocks of data which are the same size
static void swapsameblock(char *s, char *d, unsigned len)
{
 while (len--) {
 	char c = *s;
 	*s++ = *d;
 	*d++ = c;
 }
}

/* swap two adjacent blocks of data which need not be the same size
 * for example, given '3456789012', swap '3456789' and '012' to make
 * '0123456789' we do it like this:
 *	3456789012 swap '012' and '3456789'
 *	0126789345 '012' is done, swap '6789' and '345'
 *	0123459678 '012345' is done, swap '9' and '678'
 *	0123456978 '0123456' is done, swap '9' and '78'
 *	0123456798 '01234567' is done, swap '9' and '8'
 *	0123456789 done
 * at each stage we swap as much as possible from the start of the
 * second block into place, leaving a smaller version of the problem for
 * the next stage, until we end up with two equal sized blocks, which
 * can be swapped directly
 */
static void swapanyblock(char *s, unsigned slen, unsigned dlen)
{
 for (;;) {
 	char *d = s + slen;
 	if (slen <= dlen) {
 		swapsameblock(s, d, slen);
		s += slen;
		dlen -= slen;
 		if (!dlen) return;
	} else {
		swapsameblock(s, d, dlen);
		s += dlen;
		slen -= dlen;
	}
 }
}

#endif

int TiNGtrace_sethistory(size_t len)
{
 static int (*old_tracefn)(const char *fmt, va_list ap);
 int sts = pthread_mutex_lock(&logmx);
 if (!sts) {
	char *newlog;
	if (!old_tracefn) old_tracefn = TiNG_showtrace;
#ifdef TRACE_RESIZEABLE
	if (log_buff) {
		if (log_offset && log_full) {
			swapanyblock(log_buff, log_offset, log_len - log_offset);
			log_offset = 0;
		}
		if (len < log_len) {	// shrinking - move data down
			memmove(log_buff, log_buff + log_len - len, len);
			log_offset = len;
			log_full = 0;
		}
	}
	newlog = realloc(log_buff, len);
	if (newlog) {
		if (len > log_len) {
				// added space - buffer is no longer full
			log_offset = log_len;
			log_full = 0;
		}
		log_buff = newlog;
		log_len = len;
	} else if (!len) {	// ok - just stopped using history
		log_buff = 0;
		log_offset = 0;
		log_len = len;
	} else {
		sts = 1;	// but buffer is still usable at the old size
	}
#else
	log_buff = malloc(len);
	if (!log_buff) sts = 1;
	log_len = len;
#endif
	if (log_buff) TiNG_showtrace = olog_history;
	else TiNG_showtrace = old_tracefn;
	if (pthread_mutex_unlock(&logmx)) sts = 1;
 }
 return sts;
}

int TiNGtrace_savehistory(FILE *histfile)
{
 int sts = pthread_mutex_lock(&logmx);
 if (!sts) {
	if (log_full) {
		fwrite(log_buff + log_offset, 1,
			log_len - log_offset, histfile);
	}
	if (log_offset) fwrite(log_buff, 1, log_offset, histfile);
	log_offset = 0; log_full = 0;	// clear history
	sts = pthread_mutex_unlock(&logmx);
 }
 return sts;
}
