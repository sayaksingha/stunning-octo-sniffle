#include "../apilib/trace.h"

#include <string.h>

#if TiNG_TRACE > 0
#define eflog olog
#else
#define eflog printf
#endif

#ifdef __SMWIN32HLIB__
	/* This section is a workaround for various DLL issues.
	 * Note that it is undocumented, so it should not be
	 * used except when specifically instructed to do so.
	 *
	 * It is far better to use the BFILE library with
	 * bfopenhandle() which should achieve the same result
	 * without losing the buffering provided by stdio FILEs.
	 */

typedef void EFILE;

static EFILE *efopen(const char *filename, const char *mode)
{
 HANDLE h;
 DWORD cre = OPEN_EXISTING;
 DWORD rw = 0;
 DWORD share = 0;
 switch (*mode) {
 case 'r':
	cre = OPEN_EXISTING;
	rw |= GENERIC_READ;
 	if (mode[1] == '+') {
		rw |= GENERIC_WRITE;
 	} else {
		share = FILE_SHARE_READ;
	}
 	break;
 case 'w':
	cre = CREATE_ALWAYS;
	rw |= GENERIC_WRITE;
 	if (mode[1] == '+') {
		rw |= GENERIC_READ;
	}
	break;
 case 'a':
	cre = OPEN_ALWAYS;
	rw |= GENERIC_WRITE;
 	if (mode[1] == '+') {
		rw |= GENERIC_READ;
	}
	break;
 }
 h = CreateFile(filename, rw, share, 0, cre,
	FILE_ATTRIBUTE_NORMAL, 0);
 if (h == INVALID_HANDLE_VALUE) {
	if (TiNGtrace) {
 		eflog("efopen(%s, %s) failed: 0x%x\n",
 			filename, mode, GetLastError());
	}
 	return 0;
 }
 return h;
}

static int efread(void *ptr, size_t size, size_t nitems, EFILE *stream)
{
 DWORD readOctets;
 if (!ReadFile(stream, (LPVOID) ptr, size * nitems, &readOctets, NULL)) {
 	if (TiNGtrace) {
		eflog("ReadFile(, %ld, ) failed: 0x%x\n",
			(long) size * nitems, GetLastError());
	}
	return -1;
 }
 return readOctets / size;
}

static int efwrite(const void *ptr, size_t size, size_t nitems, EFILE *stream)
{
 size_t nb = size * nitems;
 DWORD writtenOctets;
 if ((!WriteFile(stream, (LPCVOID) ptr, size * nitems, &writtenOctets, NULL)
		|| nb != (int)writtenOctets)) {
	if (TiNGtrace) {
		eflog("WriteFile(, %ld, ) failed: 0x%x\n",
			(long) nb, GetLastError());
	}
	return -1;
 }
 return nitems;
}

#ifndef SEEK_SET 
#define SEEK_SET FILE_BEGIN
#endif
int efseek(EFILE *stream, long offset, int whence)
{
 if (SetFilePointer(stream, (LONG) offset, NULL, whence) == 0xffffffff) {
	if (TiNGtrace) {
		eflog("SetFilePointer() failed: 0x%x\n", GetLastError());
	}
	return -1;
 }
 return 0;
}

static long eftell(EFILE *stream)
{
 DWORD n = SetFilePointer(stream, 0, 0, FILE_CURRENT);
 if (n == 0xffffffff && TiNGtrace) {
	eflog("SetFilePointer() failed: 0x%x\n", GetLastError());
 }
 return n;
}

static int efclose(EFILE *stream)
{
 if (!CloseHandle(stream)) {
	if (TiNGtrace) {
		eflog("CloseHandle() failed: 0x%x\n", GetLastError());
	}
	return -1;
 }
 return 0;
}

#else

typedef FILE EFILE;

#include <errno.h>
#include <stdio.h>

#ifdef TiNGTYPE_QNX
#include <sys/types.h>
#endif


#ifndef CALL_FUNC
#define CALL_FUNC(x) x
#define GET_ERRNO errno
#endif

static FILE *efopen(const char *filename, const char *mode)
{
 FILE *f = CALL_FUNC(fopen)(filename, mode);
 if (!f && TiNGtrace) {
 	eflog("fopen(%s, %s) failed: %s\n",
 		filename, mode, strerror(GET_ERRNO));
 }
 return f;
}

static int efread(void *ptr, size_t size, size_t nitems, EFILE *stream)
{
 size_t n = CALL_FUNC(fread)(ptr, size, nitems, stream);
 if (!n && ferror(stream) && TiNGtrace) {
	eflog("fread(, %ld, %ld, ) failed: %s\n",
		(long) size, (long) nitems, strerror(GET_ERRNO));
 }
 return n;
}

static int efwrite(const void *ptr, size_t size, size_t nitems, EFILE *stream)
{
 size_t n = CALL_FUNC(fwrite)(ptr, size, nitems, stream);
 if (!n && CALL_FUNC(ferror)(stream) && TiNGtrace) {
	eflog("fwrite(, %ld, %ld, ) failed: %s\n",
		(long) size, (long) nitems, strerror(GET_ERRNO));
 }
 return n;
}

static int efseek(FILE *stream, long offset, int whence)
{
 int n = CALL_FUNC(fseek)(stream, offset, whence);
 if (n == -1 && TiNGtrace) {
	eflog("fseek(, %ld, %d) failed: %s\n",
		offset, whence, strerror(GET_ERRNO));
 }
 return n;
}

static long eftell(FILE *stream)
{
 long n = CALL_FUNC(ftell)(stream);
 if (n == -1 && TiNGtrace) {
	eflog("ftell() failed: %s\n", strerror(GET_ERRNO));
 }
 return n;
}

static int efclose(FILE *stream)
{
 int n = CALL_FUNC(fclose)(stream);
 if (n && TiNGtrace) {
	eflog("fclose() failed: %s\n", strerror(errno));
 }
 return n;
}

#endif
