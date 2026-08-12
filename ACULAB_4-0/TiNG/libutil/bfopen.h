/* bfopen.h - buffered file I/O on a file using a filename */

#ifndef INCLUDED_BFOPEN_H
#define INCLUDED_BFOPEN_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#include "bfile.h"

#ifndef ACUAPI
#define ACUAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Make a BFILE object refer to an open a file. `filename' is the name
 * of the file. Characters in `mode' are interpreted independently as
 * follows:
 * 
 * r	open for reading
 * w	open for writing
 * u	open for update (read and write)
 * a	make all writes (if any) append data to end of file
 * b	open in binary mode (in contrast to text mode).
 * c	file will be created if it does not exist
 * t	file will be truncated to zero length if it exists
 * 
 * Exactly one of 'r', 'w', and 'u' must be included. 'a', 'c' and 't' are
 * not permitted if 'r' is used.
 *
 * The BFILE object must not have previously been associated with a
 * file or other representation (i.e. neither bfopen() nor bfsetup()
 * can have been used).
 */

ACUAPI int bfopen(BFILE *bfilepp, char *filename, char *mode);

#ifdef __cplusplus
}
#endif

#endif
