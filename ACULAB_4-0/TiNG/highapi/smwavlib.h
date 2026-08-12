/* smwavlib.h - WAV file handling */

/*
 * Make sure you use multithreaded C RTL if required. 
 */

/* No support for reading and writing of data.     */
/* Just opening and closing of files with headers. */

#ifndef INCLUDED_SMWAVLIB_H
#define INCLUDED_SMWAVLIB_H

#ifdef TiNGTARGET_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#include "smhlib.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "../libutil/header_prolog.h"
#include "../pubdoc/gen/highapi_wavfile.h"
#include "../libutil/header_epilog.h"

#ifdef __cplusplus
}
#endif

#endif
