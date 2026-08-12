/* smtypes.h - types for Prosody API declarations */
/* these are externally visible declarations */

#ifndef INCLUDED_SMTYPES_H
#define INCLUDED_SMTYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tSMChannelId_struct *tSMChannelId;
#define kSMNullChannelId (tSMChannelId) 0
typedef int tSMCardId;
#ifndef _ACU_CARD_ID_
#define _ACU_CARD_ID_
typedef unsigned ACU_CARD_ID;
#endif

typedef struct tSMModuleId_struct *tSMModuleId;
#define kSMNullModuleId (tSMModuleId) 0
#define TiNG_PRINTF_MODULE "%p"

typedef struct tSMCollectorId_struct *tSMCollectorId;
#define kSMNullCollectorId (tSMCollectorId) 0
typedef struct tSMDispatcherId_struct *tSMDispatcherId;
#define kSMNullDispatcherId (tSMDispatcherId) 0

typedef struct tSMDatafeedId_struct *tSMDatafeedId;
#define kSMNullDatafeedId (tSMDatafeedId) 0
typedef struct tSMTDMrxId_struct *tSMTDMrxId;
#define kSMNullTDMrxId (tSMTDMrxId) 0
typedef struct tSMTDMtxId_struct *tSMTDMtxId;
#define kSMNullTDMtxId (tSMTDMtxId) 0

/*
 * Include definitions for portability across different operating systems.
 */

#ifdef TiNGTYPE_LINUX
#define KNOWN_TiNGTYPE
#include "POSIX/visdecl.h"
#endif

#ifdef TiNGTYPE_QNX
#define KNOWN_TiNGTYPE
#include "POSIX/visdecl.h"
#endif


#ifdef TiNGTYPE_WINNT
#define KNOWN_TiNGTYPE
#ifdef TiNG_USE_DLL

#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif

#endif
#ifndef ACUTiNG_WINAPI 
#define ACUTiNG_WINAPI __cdecl
#endif
#include "WINNT/visdecl.h"
#endif

#ifndef KNOWN_TiNGTYPE
#error no TiNGTYPE variant defined
#endif

#ifndef ACUAPI
#define ACUAPI
#endif

	// allow file-scope 'static' to be removed for debugging
#ifndef STATIC
#define STATIC static
#endif

#ifndef ACUTiNG_WINAPI
#define ACUTiNG_WINAPI
#endif

#ifndef TiNGtrace
#include <stddef.h>
#include <stdarg.h>
extern ACUAPI unsigned TiNGtrace;
	// pointer to output function called for each log message
extern ACUAPI int (*TiNG_showtrace)(const char *fmt, va_list ap);
extern ACUAPI int TiNG_vsnprintf (char * s, size_t n, const char * format, va_list arg );
#endif

#define kSMSecCryptDecode			0
#define kSMSecCryptEncode			1
#define kSMSecCryptNewDKey			2
#define kSMSecCryptSetEKeyToDKey	3

typedef int (*tSMSecCallback)	( tSM_INT, tSM_INT, void*, tSM_UT8* );

#include "../libutil/header_prolog.h"
#include "../pubdoc/gen/error.h"
#include "../libutil/header_epilog.h"

#ifdef __cplusplus
}
#endif

#endif
