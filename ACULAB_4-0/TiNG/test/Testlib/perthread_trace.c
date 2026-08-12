/* perthread_trace.c - implement separate trace for each thread */
#include "smdrvr.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "perthread_trace.h"

static void *log_newfile_par;
static FILE *(*log_newfile)(void *);
static pthread_key_t logkey;

static int olog_private(const char *fmt, va_list ap)
{
 FILE *f = pthread_getspecific(logkey);
 if (!f) {
 	int e;
 	f = log_newfile(log_newfile_par);
 	if (!f) return 0;	// hopeless
 	e = pthread_setspecific(logkey, f);
 	if (e) {
 		fprintf(stderr, "pthread_setspecific failed: %d\n", e);
 		fprintf(stderr, "TiNG: Cannot record log file\n");
 		fclose(f);
 		return 0;	// hopeless
	}
 }
 vfprintf(f, fmt, ap);
#ifdef TiNGTYPE_WINNT
	// key dtor may not be called, so flush file to ensure we see all log
 fflush(f);
#endif
 return 0;
}

static void key_dtor(void *fp)
{
 FILE *f = fp;
 fclose(f);
}

int TiNGtrace_perthread(FILE *(*newlog)(void *p), void *p)
{
 int e = pthread_key_create(&logkey, key_dtor);
 if (e) return e;
 log_newfile = newlog;
 log_newfile_par = p;
 TiNG_showtrace = olog_private;
 return 0;
}

static FILE *newfile(void *p)
{
 char *logfilebase = p;
 char *fname = malloc(strlen(logfilebase) + 30);
 FILE *f;
 if (!fname) {
	perror("malloc() failed");
	fprintf(stderr, "TiNG: Cannot generate log file\n");
	return 0;
 }
 	// nonportable use of pthread_self(), but saves having to
 	// allocate a suitable unique identifier
 sprintf(fname, "%s.%d", logfilebase, (int) pthread_self());
 f = fopen(fname, "a");
 if (!f) {
	perror("fopen() failed");
	fprintf(stderr, "TiNG: Cannot open log file: %s\n", fname);
	free(fname);
	return 0;
 }
 free(fname);
 return f;
}

int TiNGtrace_file_perthread(char *filebase)
{
 return TiNGtrace_perthread(newfile, filebase);
}
