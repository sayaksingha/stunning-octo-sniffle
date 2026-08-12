/* perthread_trace.h - separate trace for each thread */

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// returns non-zero on error
// sets trace to use a per-thread file, where the newlog function is
// passed the parameter p and must create a new file for the calling
// thread
int TiNGtrace_perthread(FILE *(*newlog)(void *p), void *p);

// returns non-zero on error
// sets trace to use a per-thread file, where filebase is a prefix of
// the filename used by each thread
int TiNGtrace_file_perthread(char *filebase);
#ifdef __cplusplus
}
#endif
