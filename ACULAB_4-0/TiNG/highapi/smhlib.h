/* smhlib.h - high level play/record library */
#ifndef INCLUDED_SMHLIB_H
#define INCLUDED_SMHLIB_H

#ifdef TiNGTARGET_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#include "smbesp.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __SMWIN32HLIB__
typedef HANDLE tSM_OPEN_FILE;
#else
typedef FILE *tSM_OPEN_FILE;
#endif

#ifndef TiNG_OVERRIDE_FILEIO
#ifdef TiNGTYPE_WINNT
typedef FILE* (__cdecl *efopen_fn)(const char* filename, const char* mode);
typedef int (__cdecl *efread_fn)(void *ptr, size_t size, size_t nitems, FILE *stream);
typedef int (__cdecl *efwrite_fn)(const void *ptr, size_t size, size_t nitems, FILE *stream);
typedef int (__cdecl *efseek_fn)(FILE *stream, long offset, int whence);
typedef long (__cdecl *eftell_fn)(FILE *stream);
typedef int (__cdecl *eferror_fn)(FILE *stream);
typedef int (__cdecl *efclose_fn)(FILE *stream);
typedef int *(__cdecl *eerrno_ptr_fn)(void);
ACUAPI void ACUTiNG_WINAPI highapi_override_fileio(efopen_fn open, efread_fn read, efwrite_fn write, efseek_fn seek, eftell_fn tell, eferror_fn error, efclose_fn close, eerrno_ptr_fn enp);
#define TiNG_OVERRIDE_FILEIO highapi_override_fileio(fopen, fread, fwrite, fseek, ftell, ferror, fclose, _errno);
#else
#define TiNG_OVERRIDE_FILEIO
#endif //TiNGTYPE_WINNT
#endif //TiNG_OVERRIDE_FILEIO

#include "../libutil/header_prolog.h"
#include "../pubdoc/gen/highapi_playrec.h"
#include "../libutil/header_epilog.h"

#ifdef __cplusplus
}
#endif

#endif
