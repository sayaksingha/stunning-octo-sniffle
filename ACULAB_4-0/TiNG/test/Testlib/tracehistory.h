/* tracehistory.h - implement a trace which is saved only on demand */

#include <stdio.h>

// returns non-zero on error
// set trace history buffer size to the value specified, which must be at
// least 1024
int TiNGtrace_sethistory(size_t len);

// returns non-zero on error
// saves current trace history to the given file
// and clears the trace history buffer
int TiNGtrace_savehistory(FILE *histfile);
